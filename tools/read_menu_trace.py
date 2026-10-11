"""Developer-only decoder of a bounded F11 native menu frame. made by LoVeMaKeRz."""
from pathlib import Path
import struct, json, sys

def decode(path, output):
    path, output = Path(path), Path(output)
    assert path.stat().st_size <= 128*1024*1024, 'Trace too large'
    data=path.read_bytes(); position=0
    def take(n):
        nonlocal position
        assert 0<=n<=len(data)-position, 'Truncated trace'
        result=data[position:position+n];position+=n;return result
    def number():return struct.unpack('<I',take(4))[0]
    def states(limit):
        count=number();assert count<=limit
        return {str(number()):number() for _ in range(count)}
    assert take(8)==b'DK2TRC01'
    version,width,height,count,dropped=(number() for _ in range(5))
    assert version==1 and 0<width<=8192 and 0<height<=8192 and count<=2048
    assert not output.exists(), 'Use a new output directory'
    output.mkdir(parents=True)
    meta={'credit':'made by LoVeMaKeRz','version':version,'width':width,'height':height,'dropped':dropped,'draws':[],'textures':[]}
    geometry=0
    for index in range(count):
        keys=['type','primitives','count','stride','fvf','index_format','index_count','vertex_shader','pixel_shader']
        draw=dict(zip(keys,(number() for _ in keys)))
        assert 16<=draw['stride']<=128 and draw['count']<=2_097_152
        draw['viewport']=list(struct.unpack('<4I2f',take(24)))
        draw['render']=states(256);draw['stages']=[]
        for _ in range(3):
            stage={'copy':number(),'present':number(),'texture':states(32),'sampler':states(13)}
            stage['matrix']=list(struct.unpack('<16f',take(64)));draw['stages'].append(stage)
        vb=number();assert vb==draw['count']*draw['stride'];geometry+=vb;assert geometry<=32*1024*1024
        vertices=take(vb)
        ib=number();geometry+=ib;assert geometry<=32*1024*1024
        indices=take(ib)
        if ib:
            assert draw['index_format'] in [101,102]
            size=4 if draw['index_format']==102 else 2
            assert ib==draw['index_count']*size
            used=sorted(set(struct.unpack('<'+('I' if size==4 else 'H')*draw['index_count'],indices)))
            assert not used or used[-1]<draw['count']
        else:used=list(range(draw['count']))
        pts=[struct.unpack_from('<4f',vertices,i*draw['stride']) for i in used]
        draw['bounds']=[min(p[j] for p in pts) for j in range(4)]+[max(p[j] for p in pts) for j in range(4)] if pts else []
        if draw['fvf']&64:
            colours=[struct.unpack_from('<I',vertices,i*draw['stride']+16)[0] for i in used]
            light=[sum((c>>shift)&255 for shift in [0,8,16])/3 for c in colours]
            draw['diffuse_rgb_mean']=[min(light),sum(light)/len(light),max(light)] if light else []
            draw['zero_diffuse_vertices']=sum(x==0 for x in light)
        draw['vertices']=f'draw_{index:04d}_vertices.bin';draw['indices']=f'draw_{index:04d}_indices.bin'
        (output/draw['vertices']).write_bytes(vertices)
        if indices:(output/draw['indices']).write_bytes(indices)
        meta['draws'].append(draw)
    levels=number();assert levels<=3072
    texture_bytes=0;seen=set()
    for _ in range(levels):
        identity,level,w,h,size=(number() for _ in range(5))
        assert (identity,level) not in seen and 0<w<=4096 and 0<h<=4096 and level<12 and size==w*h*4
        seen.add((identity,level));texture_bytes+=size;assert texture_bytes<=64*1024*1024
        pixels=take(size)
        header=bytearray(54);header[:2]=b'BM';struct.pack_into('<I',header,2,54+size);struct.pack_into('<I',header,10,54)
        struct.pack_into('<IiiHH',header,14,40,w,-h,1,32)
        name=f'texture_{identity:06d}_mip_{level:02d}.bmp';(output/name).write_bytes(header+pixels)
        meta['textures'].append({'copy':identity,'level':level,'width':w,'height':h,'file':name})
    assert position==len(data),'Trailing or malformed trace'
    present={item['copy'] for item in meta['textures'] if item['level']==0}
    meta['missing_copy_ids']=sorted({stage['copy'] for draw in meta['draws'] for stage in draw['stages'] if stage['copy'] and stage['copy'] not in present})
    (output/'menu_trace.json').write_text(json.dumps(meta,indent=2,allow_nan=False),encoding='utf-8')
    return meta

if __name__=='__main__':
    result=decode(sys.argv[1],sys.argv[2])
    print('PASS trace parsed:',len(result['draws']),'native draws,',len(result['textures']),'mip levels; missing copy IDs:',result['missing_copy_ids'])
