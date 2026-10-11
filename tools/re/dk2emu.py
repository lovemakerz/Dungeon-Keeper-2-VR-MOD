# Unicorn harness for DKII-DX.exe 1.70: runs the REAL engine code (camera setup 0x575A00, tile selection 0x574820, projection) on chosen cameras.
import struct,math,pefile
from unicorn import *
from unicorn.x86_const import *
import os
EXE=os.environ.get('DK2_EXE','DKII-DX.exe')   # DKII-DX.exe 1.70 (from a DK2VR report)
pe=pefile.PE(EXE);BASE=pe.OPTIONAL_HEADER.ImageBase
class Emu:
    def __init__(s):
        u=Uc(UC_ARCH_X86,UC_MODE_32);s.u=u
        u.mem_map(BASE,(pe.OPTIONAL_HEADER.SizeOfImage+0xfff)&~0xfff)
        for sec in pe.sections:
            data=sec.get_data()
            u.mem_write(BASE+sec.VirtualAddress,data[:max(0,min(len(data),sec.Misc_VirtualSize or len(data)))])
        s.STACK=0x10000000;u.mem_map(s.STACK,0x100000)
        s.HEAP=0x20000000;u.mem_map(s.HEAP,0x1000000);s.hp=s.HEAP
        s.STOP=0x30000000;u.mem_map(s.STOP,0x1000)
        s.FS=0x31000000;u.mem_map(s.FS,0x1000)
        # GDT with a data segment for FS (SEH frames use fs:[0])
        GDT=0x32000000;u.mem_map(GDT,0x1000)
        def desc(base,limit,access,flags):
            d=limit&0xffff;d|=(base&0xffffff)<<16;d|=(access&0xff)<<40;d|=((limit>>16)&0xf)<<48;d|=(flags&0xf)<<52;d|=((base>>24)&0xff)<<56;return struct.pack('<Q',d)
        u.mem_write(GDT+8*3,desc(s.FS,0xfff,0x93,0x4))   # ring 0 data, 32-bit
        u.reg_write(UC_X86_REG_GDTR,(0,GDT,0x1000,0x0))
        u.mem_write(GDT+8*1,desc(0,0xfffff,0x9b,0xc))   # flat code
        u.mem_write(GDT+8*2,desc(0,0xfffff,0x93,0xc))   # flat data, 32-bit, 4k granularity
        u.reg_write(UC_X86_REG_FS,3*8)
        u.reg_write(UC_X86_REG_SS,2*8);u.reg_write(UC_X86_REG_DS,2*8);u.reg_write(UC_X86_REG_ES,2*8)
        s.hooks={0x57ceb0:s._alloc,0x57d0b0:s._free}
        u.hook_add(UC_HOOK_CODE,s._code)
        s.count=0
    def _alloc(s):
        u=s.u;esp=u.reg_read(UC_X86_REG_ESP);ret=struct.unpack('<I',u.mem_read(esp,4))[0];n=struct.unpack('<I',u.mem_read(esp+4,4))[0]
        p=(s.hp+15)&~15;s.hp=p+max(n,16);u.mem_write(p,b'\0'*max(n,16))
        u.reg_write(UC_X86_REG_EAX,p);u.reg_write(UC_X86_REG_ESP,esp+4);u.reg_write(UC_X86_REG_EIP,ret)
    def _free(s):
        u=s.u;esp=u.reg_read(UC_X86_REG_ESP);ret=struct.unpack('<I',u.mem_read(esp,4))[0]
        u.reg_write(UC_X86_REG_ESP,esp+4);u.reg_write(UC_X86_REG_EIP,ret)
    def _code(s,u,addr,size,ud):
        s.count+=1
        if s.count>20_000_000: u.emu_stop();raise RuntimeError('too many instructions (infinite loop?)')
        h=s.hooks.get(addr)
        if h: h()
    def f32(s,va,v=None):
        if v is None: return struct.unpack('<f',s.u.mem_read(va,4))[0]
        s.u.mem_write(va,struct.pack('<f',v))
    def i32(s,va,v=None):
        if v is None: return struct.unpack('<i',s.u.mem_read(va,4))[0]
        s.u.mem_write(va,struct.pack('<i',v))
    def call(s,fn,args=(),ecx=None,stop_at=None):
        u=s.u;esp=s.STACK+0x80000
        st=b''.join(struct.pack('<I',a) for a in args)
        esp-=len(st);u.mem_write(esp,st);esp-=4;u.mem_write(esp,struct.pack('<I',s.STOP))
        u.reg_write(UC_X86_REG_ESP,esp);u.reg_write(UC_X86_REG_EBP,0)
        if ecx is not None: u.reg_write(UC_X86_REG_ECX,ecx)
        s.count=0
        u.emu_start(fn,stop_at if stop_at else s.STOP)
        return u.reg_read(UC_X86_REG_EAX)
def where(e):
    u=e.u;import capstone
    eip=u.reg_read(UC_X86_REG_EIP)
    md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
    for i in md.disasm(bytes(u.mem_read(eip,16)),eip): return '%08X %s %s'%(i.address,i.mnemonic,i.op_str)
