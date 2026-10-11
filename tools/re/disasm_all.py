import pefile,capstone,pickle,sys
pe=pefile.PE('/tmp/dk2s3/DKII-DX.exe');base=pe.OPTIONAL_HEADER.ImageBase
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
out=[]
for s in pe.sections:
  if s.Characteristics&0x20000000:
    data=s.get_data();va=base+s.VirtualAddress;off=0
    while off<len(data):
      got=False
      for i in md.disasm(data[off:],va+off):
        out.append((i.address,i.mnemonic,i.op_str));off=i.address+i.size-va;got=True
      if off<len(data): off+=1
pickle.dump(out,open('/tmp/re/ins.pkl','wb'));print(len(out))
