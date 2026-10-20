# Per-room summary for placing click-to-interact hotspots, from the agidis listings (work/logic/logicNNN.txt).
# roomsaid.py <logicdir> <words.txt> [room ...] -> for each room: animated objects (number, views, positions),
# then every said() with its words, the posn(0, ...) "close enough" box that follows it (if any) and the
# first message printed in that branch.
import sys, re, os
logdir, wordsf = sys.argv[1], sys.argv[2]
words = {}
for l in open(wordsf):
    p = l.split(None, 1)
    if len(p) == 2 and p[0].isdigit(): words[int(p[0])] = p[1].split()
def wname(g):
    w = words.get(g, ['?'])
    return '%d:%s' % (g, '/'.join(w[:4]) + ('/…' if len(w) > 4 else ''))
def room(n):
    f = os.path.join(logdir, 'logic%03d.txt' % n)
    if not os.path.exists(f): return
    lines = open(f).read().split('\n')
    msgs = dict(re.findall(r"^m(\d+): (.*)$", '\n'.join(lines), re.M))
    code = [l for l in lines if re.match(r'^[0-9a-f]{4}: ', l)]
    objs = {}
    for l in code:
        m = re.search(r'set\.view\((\d+),(\d+)\)', l)
        if m: objs.setdefault(int(m.group(1)), [set(), set()])[0].add(int(m.group(2)))
        m = re.search(r'position\((\d+),(\d+),(\d+)\)', l)
        if m: objs.setdefault(int(m.group(1)), [set(), set()])[1].add((int(m.group(2)), int(m.group(3))))
    print('=== room %d' % n)
    for o in sorted(objs):
        if o: print('  obj %d views %s at %s' % (o, sorted(objs[o][0]), sorted(objs[o][1])[:6]))
    for i, l in enumerate(code):
        for m in re.finditer(r'said\(([\d,]+)\)', l):
            ws = [int(a) for a in m.group(1).split(',')]
            posn = msg = ''
            for k in code[i + 1:i + 6]:
                p = re.search(r'posn\(0,(\d+),(\d+),(\d+),(\d+)\)', k)
                if p and not posn: posn = 'NEAR(%s)' % ', '.join(p.groups())
                pm = re.search(r'print(?:\.at)?\(m(\d+)', k)
                if pm and not msg: msg = msgs.get(pm.group(1), '')[:70]
                if 'said(' in k: break
            print('  %s %-40s %-24s %s' % (l[:4], ' '.join(wname(w) for w in ws), posn, msg))
for a in (sys.argv[3:] or range(256)): room(int(a))
