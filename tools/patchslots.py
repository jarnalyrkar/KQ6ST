# More save/restore slots for Sierra ST AGI 1.422 (SIERRA.PRG): replaces the slot list function
# (text 0x3118, 12 slots) with tools/slots/slots.c (up to 99 slots, scrolling 12-row list).
# Also replaces the heap release (text 0x4ed4) with tools/slots/release.c, which cuts the sound list
# at the released address (2-bomb crash after a sound was loaded after a view that was discarded).
# And the call of the per-cycle event poll (text 0x4c34) goes through tools/slots/menu.c, which opens
# the menu when the mouse pointer reaches the status line. The colour pointer save/hide/show vectors go to
# tools/slots/pointer.c, which saves what is under the pointer, so hovering no longer erases text.
# The sprite update's screen copy (call at text 0xe2a4) goes through tools/slots/blit.c, which leaves out
# an open message box, so animations no longer paint over it.
# The in-play double click (event post at text 0x66ea) and right click (0x673c) go through tools/slots/act.c:
# a click on a hotspot (slots/hotspots.h) types its command (click to interact); slots/pf.c finds a way there
# around barriers, in a buffer added to the end of the BSS (PFSIZE). The plain click walk (call of 0x6236 at
# 0x668c) goes through act.c's click_walk too: around barriers and out through an edge.
# Number menus (get.num after display()ed "N. ..." lines) take clicks: display (command table entry 103),
# get.num's line input call (text 0x1784) and the line input's key fetch (0xc29e) go through slots/getnum.c.
# The inventory picker (status with flag 13) takes the pointer: its start (0xba66) and event wait (0xbcb8) go
# through slots/inv.c.
# patchslots.py <in SIERRA.PRG> <out SIERRA.PRG>
# The new code is appended to the data segment; BSS references move up by BSHIFT; the old
# function starts with "jmp select_slot". Needs the m68k cross gcc (~/.local/opt/m68k-gcc/env.sh).
import sys, os, struct, subprocess, tempfile
T = os.path.dirname(os.path.abspath(__file__)); S = os.path.join(T, 'slots')
sys.path.insert(0, T); from tosprg import load
src, dst = sys.argv[1], sys.argv[2]
raw = open(src, 'rb').read()
img, tl, dl, bl, rel = load(src)
assert (tl, dl, bl) == (0xe708, 0x3eb8, 0x1dc0), 'not the expected SIERRA.PRG'
FUNC = 0x3118
assert img[FUNC:FUNC+4] == bytes.fromhex('4e56fde0'), 'slot function not found (already patched?)'
REL = 0x4ed4                                  # link a6; move.l 8(a6),heap_top; unlk; rts
assert img[REL:REL+16] == bytes.fromhex('4e56fffc23ee0008000140964e5e4e75'), 'heap release not found'
assert REL + 8 in rel
POLL = 0x4c34                                 # jsr ev_cycle (0xca86), once per interpreter cycle
assert img[POLL:POLL+6] == bytes.fromhex('4eb90000ca86') and POLL + 2 in rel, 'event poll call not found'
PTR = {0x41d4: ('0000e48e', 'ptr_save'), 0x41da: ('0000e4c2', 'ptr_hide'), 0x41e0: ('0000e510', 'ptr_show')}
for v, (old, _) in PTR.items():               # colour driver vectors: jmp save-under / hide / show pointer
    assert img[v:v+6] == bytes.fromhex('4ef9' + old) and v + 2 in rel, 'pointer vector %x not found' % v
BLIT = 0xe2a4                                 # jsr 0xdf40: sprite update, picture buffer -> screen
assert img[BLIT:BLIT+6] == bytes.fromhex('4eb90000df40') and BLIT + 2 in rel, 'sprite blit call not found'
ACT = {0x66ea: 'act_dbl', 0x673c: 'act_right'}   # jsr ev_post (0x24ac): double click / right click in play
for v in ACT:
    assert img[v:v+6] == bytes.fromhex('4eb9000024ac') and v + 2 in rel, 'click post %x not found' % v
CALLS = {0x1784: ('0000c22c', 'getnum_line'), 0xc29e: ('0000265a', 'line_key'),
         0xba66: ('0000678c', 'inv_start'), 0xbcb8: ('00002560', 'inv_stub'),
         0x668c: ('00006236', 'click_walk')}
for v, (old, _) in CALLS.items():
    assert img[v:v+6] == bytes.fromhex('4eb9' + old) and v + 2 in rel, 'call %x not found' % v
DISP = 0xe77a + 103 * 6                       # command table: display -> {long func; nargs; varmask}
assert img[DISP:DISP+6] == bytes.fromhex('000091440300') and DISP in rel, 'display entry not found'
ORG = tl + dl; BSHIFT = 0x7000; DELTA = 0x01000000
PFSIZE = 160 * 168 + 4096 * 2                # pf_grid + pf_queue, after the old BSS end (slots.ld.in)
env = dict(os.environ); gccbin = os.path.expanduser('~/.local/opt/m68k-gcc/root/usr/bin')
env['PATH'] = gccbin + ':' + env['PATH']
env['LD_LIBRARY_PATH'] = os.path.expanduser('~/.local/opt/m68k-gcc/root/usr/lib/x86_64-linux-gnu') + ':' + env.get('LD_LIBRARY_PATH', '')
tmp = tempfile.mkdtemp()
objs = []
for src_c in ('slots.c', 'release.c', 'menu.c', 'pointer.c', 'blit.c', 'act.c', 'getnum.c', 'inv.c', 'pf.c'):
    obj = os.path.join(tmp, src_c[:-2] + '.o'); objs.append(obj)
    subprocess.run(['m68k-linux-gnu-gcc-15', '-m68000', '-mshort', '-O2', '-fomit-frame-pointer', '-ffreestanding',
                    '-fno-builtin', '-nostdlib', '-fno-pic', '-fcall-used-d2', '-fcall-used-a2', '-ffunction-sections',
                    '-Wall', '-c', os.path.join(S, src_c), '-o', obj], check=True, env=env)
def link(base):
    ld = os.path.join(tmp, 'l%x.ld' % base); elf = ld + '.elf'; out = elf + '.bin'
    t = open(os.path.join(S, 'slots.ld.in')).read()
    open(ld, 'w').write(t.replace('@BASE@', hex(base)).replace('@BSHIFT@', hex(BSHIFT)).replace('@ORG@', hex(ORG)))
    subprocess.run(["m68k-linux-gnu-ld", "--no-warn-rwx-segments", "-T", ld, '-o', elf] + objs, check=True, env=env)
    subprocess.run(['m68k-linux-gnu-objcopy', '-O', 'binary', '-j', '.blob', elf, out], check=True, env=env)
    nm = subprocess.run(['m68k-linux-gnu-nm', elf], check=True, env=env, capture_output=True, text=True).stdout
    syms = {l.split()[2]: int(l.split()[0], 16) for l in nm.splitlines() if len(l.split()) == 3}
    return open(out, 'rb').read(), syms
a, sa = link(0); b, sb = link(DELTA)
assert len(a) == len(b) and len(a) <= BSHIFT, (len(a), BSHIFT)
entry = sa['select_slot']
newrel = []; o = 0
while o + 4 <= len(a):
    x, y = struct.unpack('>I', a[o:o+4])[0], struct.unpack('>I', b[o:o+4])[0]
    if (y - x) & 0xffffffff == DELTA: newrel.append(ORG + o); o += 4
    else: assert a[o:o+2] == b[o:o+2], hex(o); o += 2
blob = a + bytes(BSHIFT - len(a))
# shift every relocated long that points into the BSS
img = bytearray(img)
for r in rel:
    v = struct.unpack('>I', img[r:r+4])[0]
    if v >= tl + dl: img[r:r+4] = struct.pack('>I', v + BSHIFT)
img[FUNC:FUNC+6] = b'\x4e\xf9' + struct.pack('>I', entry)
img[REL:REL+6] = b'\x4e\xf9' + struct.pack('>I', sa['heap_release'])
img[POLL+2:POLL+6] = struct.pack('>I', sa['menu_poll'])
img[BLIT+2:BLIT+6] = struct.pack('>I', sa['sprite_blit'])
for v, (_, name) in PTR.items(): img[v+2:v+6] = struct.pack('>I', sa[name])
for v, name in ACT.items(): img[v+2:v+6] = struct.pack('>I', sa[name])
for v, (_, name) in CALLS.items(): img[v+2:v+6] = struct.pack('>I', sa[name])
img[DISP:DISP+4] = struct.pack('>I', sa['disp_hook'])
allrel = sorted((set(rel) - {REL + 8}) | {FUNC + 2, REL + 2} | set(newrel))
img += blob
fix = bytearray(struct.pack('>I', allrel[0])); prev = allrel[0]
for r in allrel[1:]:
    d = r - prev; prev = r
    while d > 254: fix.append(1); d -= 254
    fix.append(d)
fix += b'\0'
hdr = bytearray(raw[:28]); struct.pack_into('>III', hdr, 2, tl, dl + BSHIFT, bl + PFSIZE)
open(dst, 'wb').write(bytes(hdr) + bytes(img) + bytes(fix))
print('blob %d bytes at text+0x%x, entry 0x%x, %d new relocs, BSS +0x%x' % (len(a), ORG, entry, len(newrel), BSHIFT))
if os.environ.get('SLOTS_MAP'):                # symbol map (text-relative addresses) for debugging in Hatari
    open(os.environ['SLOTS_MAP'], 'w').write(''.join('%06x %s\n' % (v, k) for k, v in sorted(sa.items(), key=lambda i: i[1])))
