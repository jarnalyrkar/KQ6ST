import sys,struct; sys.path.insert(0,sys.argv[2])
from tosprg import load
img,tl,dl,bl,rel=load(sys.argv[1])
print('text',hex(tl),'data',hex(dl),'bss',hex(bl),'relocs',len(rel))
# find runs of relocated longs at constant stride in data
rs=sorted(r for r in rel if r>=tl)
for stride in (4,6,8,10,12):
    best=[]
    for r in rs:
        n=0
        while r+n*stride in rel: n+=1
        if n>40: best.append((n,r))
    best.sort(reverse=True)
    seen=set()
    for n,r in best[:8]:
        if any(abs(r-s)<stride*n for s in seen): continue
        seen.add(r); print('stride',stride,'count',n,'at',hex(r),'data+',hex(r-tl))
