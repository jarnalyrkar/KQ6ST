# AGI v2 logic disassembler. agidis.py <gamedir> <outdir>  -> one .txt per logic + opcode usage summary
import sys, os, json, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import agires
OPS=json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'ops.json')))
COND,ACT=OPS['cond'],OPS['act']
# Sierra ST AGI 1.422 differs from DOS from 174 on: 174 hides the pointer (0x67bc), 175 shows it (0x67dc),
# 177 takes two vars (click capture, text 0x67fc), not DOS set.pri.base(n) / discard.sound(n) / allow.menu(n)
ACT[174] = ['hide.mouse', '']
ACT[175] = ['show.mouse', '']
ACT[177] = ['get.mse.posn', 'vv']
def fmt(t,v): return {'v':'v%d','n':'%d','s':'m%d','o':'o%d'}.get(t,'%d')%v
def dis(code, msgs, usage, num):
    out=[]; pc=0
    while pc<len(code):
        op=code[pc]; start=pc; pc+=1
        if op==0xff:
            conds=[]
            while True:
                c=code[pc]; pc+=1
                if c==0xff: break
                if c in (0xfc,0xfd): conds.append({0xfc:'||',0xfd:'!'}[c]); continue
                if c==0x0e:
                    n=code[pc]; pc+=1
                    w=[struct.unpack('<H',code[pc+2*i:pc+2*i+2])[0] for i in range(n)]; pc+=2*n
                    conds.append('said(%s)'%','.join(map(str,w))); usage['c',c].add(num); continue
                if c>=len(COND): conds.append('BADCOND_%02x'%c); usage['c',c].add(num); continue
                name,p=COND[c]; a=code[pc:pc+len(p)]; pc+=len(p)
                conds.append('%s(%s)'%(name,','.join(fmt(t,x) for t,x in zip(p,a)))); usage['c',c].add(num)
            off=struct.unpack('<h',code[pc:pc+2])[0]; pc+=2
            out.append('%04x: if %s else goto %04x'%(start,' '.join(conds),pc+off))
        elif op==0xfe:
            off=struct.unpack('<h',code[pc:pc+2])[0]; pc+=2
            out.append('%04x: goto %04x'%(start,pc+off))
        else:
            usage['a',op].add(num)
            if op>=len(ACT): out.append('%04x: BADOP_%02x  <-- unknown, decode lost'%(start,op)); break
            name,p=ACT[op]; a=code[pc:pc+len(p)]; pc+=len(p)
            s='%04x: %s(%s)'%(start,name,','.join(fmt(t,x) for t,x in zip(p,a)))
            for t,x in zip(p,a):
                if t=='s' and x in msgs: s+='   ; "%s"'%msgs[x][:70].replace('\n','\\n')
            out.append(s)
    return out
if __name__=='__main__':
    gd,od=sys.argv[1],sys.argv[2]; os.makedirs(od,exist_ok=True)
    usage=collections.defaultdict(set)
    for num,(v,o) in sorted(agires.dirent(gd,'LOGDIR').items()):
        code,msgs=agires.logic(agires.res(gd,v,o))
        lines=dis(code,msgs,usage,num)
        with open(os.path.join(od,'logic%03d.txt'%num),'w') as f:
            f.write('\n'.join(lines)+'\n\n; messages\n'+'\n'.join('m%d: %r'%kv for kv in msgs.items())+'\n')
    for (k,op),ls in sorted(usage.items()):
        tab=COND if k=='c' else ACT
        nm=tab[op][0] if op<len(tab) else '???'
        print('%s %3d 0x%02x %-20s %4d logics %s'%(k,op,op,nm,len(ls),sorted(ls)[:12] if op>=0xa9 or k=='c' and op>0x10 else ''))
