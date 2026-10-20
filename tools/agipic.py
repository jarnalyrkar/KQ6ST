# AGI v2 picture renderer (port of ScummVM engines/agi/picture.cpp), for placing click-to-interact hotspots.
# agipic.py <gamedir> <outdir> [pic ...] -> outdir/picNNN.png: visual (top) and priority/control screen
# (bottom), 160x168 game pixels shown 8x4 each, grid every 10 game pixels with labels.
# Priority colours: 0 black = barrier, 1 = conditional barrier, 2 = signal, 3 = water, 4-15 = bands.
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from agires import dirent, res

W, H = 160, 168
EGA = [(0,0,0),(0,0,170),(0,170,0),(0,170,170),(170,0,0),(170,0,170),(170,85,0),(170,170,170),
       (85,85,85),(85,85,255),(85,255,85),(85,255,255),(255,85,85),(255,85,255),(255,255,85),(255,255,255)]
CIRCLE_LIST = [0, 1, 4, 9, 16, 25, 37, 50]
CIRCLE_DATA = [0x8000,
    0x0000, 0xE000, 0x0000,
    0x7000, 0xF800, 0x0F800, 0x0F800, 0x7000,
    0x3800, 0x7C00, 0x0FE00, 0x0FE00, 0x0FE00, 0x7C00, 0x3800,
    0x1C00, 0x7F00, 0x0FF80, 0x0FF80, 0x0FF80, 0x0FF80, 0x0FF80, 0x7F00, 0x1C00,
    0x0E00, 0x3F80, 0x7FC0, 0x7FC0, 0x0FFE0, 0x0FFE0, 0x0FFE0, 0x7FC0, 0x7FC0, 0x3F80, 0x1F00, 0x0E00,
    0x0F80, 0x3FE0, 0x7FF0, 0x7FF0, 0x0FFF8, 0x0FFF8, 0x0FFF8, 0x0FFF8, 0x0FFF8, 0x7FF0, 0x7FF0, 0x3FE0, 0x0F80,
    0x07C0, 0x1FF0, 0x3FF8, 0x7FFC, 0x7FFC, 0x0FFFE, 0x0FFFE, 0x0FFFE, 0x0FFFE, 0x0FFFE, 0x7FFC, 0x7FFC, 0x3FF8, 0x1FF0, 0x07C0]

def render(data):
    vis = bytearray([15]) * (W * H); pri = bytearray([4]) * (W * H)
    st = dict(off=0, scr=False, pr=False, sc=15, pc=4, pat=0, patnum=0)
    def put(x, y):
        if 0 <= x < W and 0 <= y < H:
            if st['scr']: vis[y * W + x] = st['sc']
            if st['pr']: pri[y * W + x] = st['pc']
    def nb():
        b = data[st['off']]; st['off'] += 1; return b
    def param():
        if st['off'] >= len(data) or data[st['off']] >= 0xf0: return None
        return nb()
    def coords():
        x = param()
        if x is None: return None
        y = param()
        if y is None: return None
        return x, y
    def line(x1, y1, x2, y2):
        x1 = min(max(x1, 0), W - 1); x2 = min(max(x2, 0), W - 1); y1 = min(max(y1, 0), H - 1); y2 = min(max(y2, 0), H - 1)
        if x1 == x2:
            for y in range(min(y1, y2), max(y1, y2) + 1): put(x1, y)
            return
        if y1 == y2:
            for x in range(min(x1, x2), max(x1, x2) + 1): put(x, y1)
            return
        sx = 1 if x2 >= x1 else -1; dx = abs(x2 - x1); sy = 1 if y2 >= y1 else -1; dy = abs(y2 - y1)
        if dy > dx: i = det = dy; ex = dy // 2; ey = 0
        else: i = det = dx; ex = 0; ey = dx // 2
        x, y = x1, y1; put(x, y)
        while True:
            ey += dy
            if ey >= det: ey -= det; y += sy
            ex += dx
            if ex >= det: ex -= det; x += sx
            put(x, y); i -= 1
            if i <= 0: break
    def fillok(x, y):
        if not (0 <= x < W and 0 <= y < H): return False
        if not st['pr'] and st['scr'] and st['sc'] != 15: return vis[y * W + x] == 15
        if st['pr'] and not st['scr'] and st['pc'] != 4: return pri[y * W + x] == 4
        return st['scr'] and vis[y * W + x] == 15 and st['sc'] != 15
    def fill(x, y):
        if not st['scr'] and not st['pr']: return
        stack = [(x, y)]
        while stack:
            px, py = stack.pop()
            if not fillok(px, py): continue
            c = px - 1
            while fillok(c, py): c -= 1
            up = down = True; c += 1
            while fillok(c, py):
                put(c, py)
                if fillok(c, py - 1):
                    if up: stack.append((c, py - 1)); up = False
                else: up = True
                if fillok(c, py + 1):
                    if down: stack.append((c, py + 1)); down = False
                else: down = True
                c += 1
    def pattern(x, y):
        size = st['pat'] & 7; ci = CIRCLE_LIST[size]
        px = x * 2 - size; px = max(px, 0); px = min(px, W * 2 - 2 * size); px //= 2; fx = px
        py = y - size; py = max(py, 0); py = min(py, 167 - 2 * size)
        t = (st['patnum'] | 1) & 0xff
        fy = py + 2 * size + 1; width = (2 * size + 1) << 1
        while py < fy:
            word = CIRCLE_DATA[ci]; ci += 1; cnt = 0
            while cnt <= width:
                if (st['pat'] & 0x10) or ((0x8000 >> (cnt >> 1)) & word):
                    if st['pat'] & 0x20:
                        b = t & 1; t >>= 1
                        if b: t ^= 0xb8
                    if not (st['pat'] & 0x20) or (t & 3) == 2: put(px, py)
                px += 1; cnt += 4
            px = fx; py += 1
    while st['off'] < len(data):
        op = nb()
        if op == 0xf0: st['sc'] = nb(); st['scr'] = True
        elif op == 0xf1: st['scr'] = False
        elif op == 0xf2: st['pc'] = nb(); st['pr'] = True
        elif op == 0xf3: st['pr'] = False
        elif op in (0xf4, 0xf5):
            c = coords()
            if c is None: continue
            x1, y1 = c; put(x1, y1); ymode = op == 0xf4
            while True:
                v = param()
                if v is None: break
                if ymode: line(x1, y1, x1, v); y1 = v
                else: line(x1, y1, v, y1); x1 = v
                ymode = not ymode
        elif op == 0xf6:
            c = coords()
            if c is None: continue
            x1, y1 = c; put(x1, y1)
            while True:
                c = coords()
                if c is None: break
                line(x1, y1, *c); x1, y1 = c
        elif op == 0xf7:
            c = coords()
            if c is None: continue
            x1, y1 = c; put(x1, y1)
            while True:
                d = param()
                if d is None: break
                dx = d >> 4; dy = d & 15
                if dx & 8: dx = -(dx & 7)
                if dy & 8: dy = -(dy & 7)
                line(x1, y1, x1 + dx, y1 + dy); x1 += dx; y1 += dy
        elif op == 0xf8:
            while True:
                c = coords()
                if c is None: break
                fill(*c)
        elif op == 0xf9: st['pat'] = nb()
        elif op == 0xfa:
            while True:
                if st['pat'] & 0x20:
                    n = param()
                    if n is None: break
                    st['patnum'] = n
                c = coords()
                if c is None: break
                pattern(*c)
        elif op == 0xff: break
    return vis, pri

def image(vis, pri):
    from PIL import Image, ImageDraw
    SX, SY = 8, 4
    im = Image.new('RGB', (W * SX, H * SY * 2 + 4), (60, 60, 60))
    for half, buf in enumerate((vis, pri)):
        small = Image.new('RGB', (W, H))
        small.putdata([EGA[c] for c in buf])
        im.paste(small.resize((W * SX, H * SY), Image.NEAREST), (0, half * (H * SY + 4)))
    d = ImageDraw.Draw(im)
    for half in range(2):
        oy = half * (H * SY + 4)
        for x in range(0, W, 10):
            d.line([(x * SX, oy), (x * SX, oy + H * SY - 1)], fill=(255, 0, 0) if x % 50 == 0 else (255, 255, 255))
            d.text((x * SX + 2, oy + 1), str(x), fill=(255, 255, 0))
        for y in range(0, H, 10):
            d.line([(0, oy + y * SY), (W * SX - 1, oy + y * SY)], fill=(255, 0, 0) if y % 50 == 0 else (255, 255, 255))
            d.text((2, oy + y * SY + 1), str(y), fill=(255, 255, 0))
    return im

if __name__ == '__main__':
    gd, out = sys.argv[1], sys.argv[2]; os.makedirs(out, exist_ok=True)
    d = dirent(gd, 'PICDIR')
    for n in ([int(a) for a in sys.argv[3:]] or sorted(d)):
        vis, pri = render(res(gd, *d[n]))
        image(vis, pri).save(os.path.join(out, 'pic%03d.png' % n))
