# Draw the click-to-interact AREA boxes (and OBJ start positions from the logic) over each room's picture.
# hotspotmap.py <gamedir> <logicdir> <outdir> [room ...] -> outdir/roomNNN.png
# Boxes are drawn in list order with their command; earlier entries win where boxes overlap, so the label
# of the box that wins is the one drawn last (on top). Objects are marked at their position(...) values
# from the logic (bottom-left corner), as they are only known live in the game.
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from agires import dirent, res
from agipic import render, image
from PIL import ImageDraw
gd, logdir, out = sys.argv[1], sys.argv[2], sys.argv[3]
os.makedirs(out, exist_ok=True)
HS = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'slots', 'hotspots')
rooms = {}
for fn in sorted(os.listdir(HS)):
    for l in open(os.path.join(HS, fn)):
        m = re.match(r'\s*(OBJ|AREA)\((\d+), (.*)\),\s*$', l)
        if m: rooms.setdefault(int(m.group(2)), []).append((m.group(1), m.group(3)))
pics = dirent(gd, 'PICDIR')
COL = [(255, 0, 255), (0, 255, 255), (255, 255, 0), (255, 128, 0), (128, 255, 0), (255, 255, 255)]
for room in [int(a) for a in sys.argv[4:]] or sorted(rooms):
    code = open(os.path.join(logdir, 'logic%03d.txt' % room)).read()
    m = re.search(r'load\.pic\((\d+)\)', code)
    pic = int(m.group(1)) if m else room
    if pic not in pics: pic = room
    if pic not in pics: continue
    vis, pri = render(res(gd, *pics[pic]))
    im = image(vis, pri); d = ImageDraw.Draw(im)
    entries = rooms.get(room, [])
    for i, (kind, rest) in reversed(list(enumerate(entries))):
        c = COL[i % len(COL)]; cmd = re.search(r'"([^"]*)"', rest).group(1)
        n = [int(a) for a in re.findall(r'\d+', re.split('NEAR|ANY|DIST', rest)[0])]
        if kind == 'AREA':
            x1, y1, x2, y2 = n
            d.rectangle([x1 * 8, y1 * 4, x2 * 8 + 7, y2 * 4 + 3], outline=c, width=2)
            d.text((x1 * 8 + 3, y1 * 4 + 3), '%d %s' % (i, cmd), fill=c)
        else:
            for x, y in re.findall(r'position\(%d,(\d+),(\d+)\)' % n[0], code):
                x, y = int(x), int(y)
                d.ellipse([x * 8 - 5, y * 4 - 5, x * 8 + 5, y * 4 + 5], outline=c, width=2)
                d.text((x * 8 + 7, y * 4 - 12), '%d o%d %s' % (i, n[0], cmd), fill=c)
        st = re.search(r'NEAR\((\d+), (\d+), (\d+), (\d+)\)', rest)
        if st:
            x1, y1, x2, y2 = map(int, st.groups()); oy = 168 * 4 + 4
            d.rectangle([x1 * 8, oy + y1 * 4, x2 * 8 + 7, oy + y2 * 4 + 3], outline=c, width=2)
            d.text((x1 * 8 + 3, oy + y1 * 4 + 3), 'stand %d' % i, fill=c)
    im.save(os.path.join(out, 'room%03d.png' % room))
