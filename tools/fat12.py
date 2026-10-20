import sys, struct, os
img=open(sys.argv[1],'rb').read(); out=sys.argv[2] if len(sys.argv)>2 else None
bps,spc,res,nf,nroot,tot,_,spf=struct.unpack('<HBHBHHBH',img[11:24])
fat=img[res*bps:(res+spf)*bps]
rootoff=(res+nf*spf)*bps; dataoff=rootoff+nroot*32
def nxt(c):
    o=c*3//2; v=fat[o]|fat[o+1]<<8
    return v>>4 if c&1 else v&0xfff
def chain(c):
    while 2<=c<0xff0: yield c; c=nxt(c)
def read(c,size):
    b=b''.join(img[dataoff+(x-2)*spc*bps:dataoff+(x-1)*spc*bps] for x in chain(c))
    return b[:size] if size is not None else b
def ls(data,path):
    for i in range(0,len(data),32):
        e=data[i:i+32]
        if e[0]==0: break
        if e[0]==0xe5: continue
        name=e[:8].decode('latin1').rstrip(); ext=e[8:11].decode('latin1').rstrip()
        attr=e[11]; cl,size=struct.unpack('<HI',e[26:32])
        n=name+('.'+ext if ext else '')
        if attr&8: print('VOL',n); continue
        if n in('.','..'): continue
        t,d=struct.unpack('<HH',e[22:26])
        date='%04d-%02d-%02d'%(1980+(d>>9),(d>>5)&15,d&31)
        p=path+'/'+n
        if attr&0x10:
            print('DIR ',p); ls(read(cl,None),p)
        else:
            print('%8d %s %s'%(size,date,p))
            if out:
                os.makedirs(os.path.dirname(out+p),exist_ok=True)
                open(out+p,'wb').write(read(cl,size))
ls(img[rootoff:dataoff],'')
