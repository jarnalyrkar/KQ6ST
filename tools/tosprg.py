# Load TOS PRG: returns (text+data image, tlen, dlen, blen, set of reloc offsets)
import struct
def load(path):
    d=open(path,'rb').read()
    magic,tl,dl,bl,sl=struct.unpack('>HIIII',d[:18])
    img=bytearray(d[28:28+tl+dl]); p=28+tl+dl+sl
    rel=set(); off=struct.unpack('>I',d[p:p+4])[0]; p+=4
    if off:
        rel.add(off)
        while True:
            b=d[p]; p+=1
            if b==0: break
            if b==1: off+=254; continue
            off+=b; rel.add(off)
    return img,tl,dl,bl,rel
