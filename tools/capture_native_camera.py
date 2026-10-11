"""Read-only snapshot of the supported DKII-DX camera for renderer debugging."""
import argparse
import ctypes
import json
import struct
from pathlib import Path
from ctypes import wintypes


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("pid", type=int)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    kernel.OpenProcess.restype = wintypes.HANDLE
    kernel.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p,
                                        ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
    kernel.ReadProcessMemory.restype = wintypes.BOOL
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    process = kernel.OpenProcess(0x1010, False, args.pid)
    if not process:
        raise ctypes.WinError(ctypes.get_last_error())

    def read(address, count):
        buffer = ctypes.create_string_buffer(count)
        received = ctypes.c_size_t()
        if not kernel.ReadProcessMemory(process, address, buffer, count, ctypes.byref(received)):
            raise ctypes.WinError(ctypes.get_last_error())
        if received.value != count:
            raise RuntimeError("Incomplete camera snapshot")
        return buffer.raw

    def floats(address, count):
        return list(struct.unpack("<" + "f" * count, read(address, count * 4)))

    try:
        if read(0x58A6F0, 9) != bytes.fromhex("83ec608b4424645657"):
            raise RuntimeError("Unsupported executable signature")
        data = {
            "world_camera_position": floats(0x77F400, 3),
            "camera_matrix": floats(0x77F40C, 9),
            "scaled_matrix": floats(0x77F458, 9),
            "translation": floats(0x780940, 3),
            "focal": floats(0x77F430, 1)[0],
            "screen_focal_x": floats(0x77F4CC, 1)[0],
            "screen_focal_y": floats(0x78093C, 1)[0],
            "center_x": floats(0x77F4F0, 1)[0],
            "center_y": floats(0x77F930, 1)[0],
            "frontend": struct.unpack("<I", read(0x75AEAF, 4))[0],
        }
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(data, indent=2), encoding="utf-8")
        print(json.dumps(data, indent=2))
    finally:
        kernel.CloseHandle(process)


if __name__ == "__main__":
    main()
