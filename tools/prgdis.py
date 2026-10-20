# Linear 68000 disassembly of a TOS PRG text segment. prgdis.py <prg> <out.s>
# Lines: "<textoffset>: <hex>  <insn>"; relocated longs marked "; R".
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from capstone import Cs, CS_ARCH_M68K, CS_MODE_M68K_000, CS_MODE_BIG_ENDIAN
from tosprg import load
img, tl, dl, bl, rel = load(sys.argv[1])
md = Cs(CS_ARCH_M68K, CS_MODE_M68K_000 | CS_MODE_BIG_ENDIAN)
out = []; pc = 0
while pc < tl:
    ins = next(md.disasm(bytes(img[pc:pc+10]), pc), None)
    if ins is None:
        out.append('%05x: %s  dc.w' % (pc, img[pc:pc+2].hex())); pc += 2; continue
    r = '    ; R' if any(o in rel for o in range(pc, pc + ins.size - 3)) else ''
    out.append('%05x: %-20s %s %s%s' % (pc, ins.bytes.hex(), ins.mnemonic, ins.op_str, r)); pc += ins.size
open(sys.argv[2], 'w').write('\n'.join(out) + '\n')
