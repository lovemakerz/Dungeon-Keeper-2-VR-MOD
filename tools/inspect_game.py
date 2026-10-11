import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent / 'python_deps'))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

path = Path(__file__).resolve().parents[1] / 'Dungeon Keeper 2' / 'DKII-DX.exe'
pe = pefile.PE(str(path))
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
for arg in sys.argv[1:]:
    start, size = (int(x, 16) for x in arg.split(':'))
    print(f'Address {start:08X}, size {size:X}')
    for i in decoder.disasm(pe.get_data(start - pe.OPTIONAL_HEADER.ImageBase, size), start):
        print(f'{i.address:08X} {i.bytes.hex():20} {i.mnemonic:8} {i.op_str}')
