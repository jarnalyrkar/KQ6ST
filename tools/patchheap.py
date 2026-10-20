# Enlarge the AGI heap of Sierra ST AGI 1.422 (SIERRA.PRG). Works on the PRG or on a disk image
# (patches every copy in place, file sizes unchanged).
# patchheap.py <in> <out> [heap_kb]   default 128 (original 64)
# Startup code (text 0x1e24): Malloc(0x7d00 screen + 0x6900 priority + 0x10000 heap).
import sys
src, dst = sys.argv[1], sys.argv[2]; kb = int(sys.argv[3]) if len(sys.argv) > 3 else 128
assert 64 <= kb <= 4096 and kb % 64 == 0
ctx_old = bytes.fromhex('203c00007d00' '068000006900' '068000010000' '23c0')
ctx_new = ctx_old[:12] + bytes.fromhex('0680') + (kb * 1024).to_bytes(4, 'big') + ctx_old[18:]
d = bytearray(open(src, 'rb').read())
n = d.count(ctx_old); assert n >= 1, 'original bytes not found (already patched?)'
d = d.replace(ctx_old, ctx_new); open(dst, 'wb').write(d)
print('patched %d copies, heap %d KB' % (n, kb))
