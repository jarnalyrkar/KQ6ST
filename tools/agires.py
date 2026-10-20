# AGI v2 resource reader. agires.py <gamedir> : summary
import sys, struct, os
def dirent(gd, name):
    d=open(os.path.join(gd,name),'rb').read(); out={}
    for i in range(len(d)//3):
        a,b,c=d[i*3:i*3+3]
        if a==0xff: continue
        out[i]=(a>>4, ((a&15)<<16)|(b<<8)|c)
    return out
_vols={}
def res(gd, vol, off):
    if vol not in _vols: _vols[vol]=open(os.path.join(gd,'VOL.%d'%vol),'rb').read()
    v=_vols[vol]; assert v[off]==0x12 and v[off+1]==0x34, (vol,off)
    ln=struct.unpack('<H',v[off+3:off+5])[0]; return v[off+5:off+5+ln]
KEY=b'Avis Durgan'
def logic(data):
    cl=struct.unpack('<H',data[:2])[0]; code=data[2:2+cl]; m=data[2+cl:]
    n=m[0]; end=struct.unpack('<H',m[1:3])[0]
    ptrs=[struct.unpack('<H',m[3+i*2:5+i*2])[0] for i in range(n)]
    base=m[1+2*n+2:]  # text after pointer table... decrypt from after ptrs
    txt=bytes(b^KEY[i%11] for i,b in enumerate(m[3+2*n:]))
    msgs={}
    for i,p in enumerate(ptrs):
        if p==0: continue
        s=p-2-2*n; e=txt.find(b'\0',s); msgs[i+1]=txt[s:e].decode('latin1')
    return code,msgs
if __name__=='__main__':
    gd=sys.argv[1]
    for t in ('LOGDIR','PICDIR','VIEWDIR','SNDDIR'):
        d=dirent(gd,t); sizes=[len(res(gd,*d[k])) for k in d]
        print(t,len(d),'entries max idx',max(d),'max size',max(sizes),'total',sum(sizes))
