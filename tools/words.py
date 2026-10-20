# Decode AGI WORDS.TOK -> {group: [words]}
import sys, struct
def load(path):
    d=open(path,'rb').read(); p=52; prev=b''; g={}
    while p<len(d)-1:
        keep=d[p]; p+=1; w=bytearray(prev[:keep])
        while True:
            c=d[p]; p+=1; w.append((c&0x7f)^0x7f)
            if c&0x80: break
        grp=struct.unpack('>H',d[p:p+2])[0]; p+=2
        prev=bytes(w); g.setdefault(grp,[]).append(w.decode('latin1'))
    return g
if __name__=='__main__':
    for k,v in sorted(load(sys.argv[1]).items()): print(k,' '.join(v))
