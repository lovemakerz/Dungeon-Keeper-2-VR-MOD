import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent / 'python_deps'))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
pe = pefile.PE(sys.argv[1])
base = pe.OPTIONAL_HEADER.ImageBase
print(f'Base {base:08X}')
for arg in sys.argv[2:]:
    if arg == 'imports':
        for dll in getattr(pe, 'DIRECTORY_ENTRY_IMPORT', []):
            for imp in dll.imports:
                print(f'{imp.address:08X} {dll.dll!r} {imp.name!r}')
    elif arg == 'exports':
        for ex in getattr(pe, 'DIRECTORY_ENTRY_EXPORT').symbols:
            print(f'{ex.address:08X} {ex.name!r}')
    else:
        start, size = (int(x, 16) for x in arg.split(':'))
        for i in Cs(CS_ARCH_X86, CS_MODE_32).disasm(pe.get_data(start, size), base+start):
            print(f'{i.address:08X} {i.bytes.hex():20} {i.mnemonic:8} {i.op_str}')
