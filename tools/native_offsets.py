from pathlib import Path
import sys
root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root/'research/Flame/mapping/ida'))
import sgmap
structs, _ = sgmap.parse_file(root/'research/Flame/mapping/DKII_EXE_v170.sgmap')
for struct in structs:
    if struct.name not in sys.argv[1:]: continue
    print(struct.name, 'size', hex(struct.size))
    offset=struct.calc_fields_offs()
    for field in struct.fields:
        print(f'{offset:04X} {field.name}')
        offset += field.type.get_size()
