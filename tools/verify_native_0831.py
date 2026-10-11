import sys, pefile, capstone
p=pefile.PE(sys.argv[1]); b=p.OPTIONAL_HEADER.ImageBase
sites={
 0x583710:'8b4f7c',0x5837dc:'8b4f7c',0x583c6a:'8b517c',0x583e2c:'8b4e7c',
 0x583f05:'8b4e7c',0x58486f:'8b4a7c',0x584ced:'8b4e7c',
 0x584e75:'8b567c',0x58508e:'8b4e7c',0x58520f:'8b4e7c',
 0x5855f8:'8b5e7c',0x585d00:'8b477c'
}
for va,sig in sites.items():
 actual=p.get_data(va-b,3); assert actual==bytes.fromhex(sig),(hex(va),actual.hex(),sig)
print('PASS: all 12 native geometry LOD reader signatures match the supplied 1.70 executable.')
checks={0x5849f8:'c7467c00000000',0x5849ff:'0f85ab000000d9442410',0x584aad:'89467c',
 0x577f10:'81eca400000053',0x57839a:'c20800',0x58b190:'83ec148b44241c'}
for va,sig in checks.items():assert p.get_data(va-b,len(bytes.fromhex(sig)))==bytes.fromhex(sig)
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
ins=list(cs.disasm(bytes.fromhex('e9ac00000090'),0x5849ff))
assert ins[0].mnemonic=='jmp' and ins[0].op_str=='0x584ab0'
print('PASS: model/projection hooks and LOD bypass signatures match; branch lands after LOD computation.')
