# Check the click-to-interact hotspots (tools/slots/hotspots/*.h) against the game data.
# checkhotspots.py <gamedir with WORDS.TOK> <logicdir (agidis listings of the PATCHED game, e.g.
#   agidis.py build/KQ6ST-HD work/logic-patched, so script patches such as the pawn shop are seen)>
# For each entry: every word of the command is in the vocabulary (as the parser reads it: longest match,
# group 0 words dropped); the room's logic has a said() that accepts exactly those groups (1 = any word,
# 9999 = rest of line); NEAR(...) equals a posn(0, ...) box that logic checks in that said()'s branch
# (a said() with a posn check and ANY is reported too); DIST(obj, d) equals a distance(0, obj, v) +
# lessn(v, d) test in that branch; an OBJ's view is set on that object in the logic.
# Exit status 1 if anything is wrong.
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from words import load
gd, logdir = sys.argv[1], sys.argv[2]
HS = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'slots', 'hotspots')
vocab = {}
for g, ws in load(os.path.join(gd, 'WORDS.TOK')).items():
    for w in ws: vocab[w] = g

def parse(cmd):
    toks = cmd.lower().split(); out = []; i = 0
    while i < len(toks):
        for n in range(len(toks) - i, 0, -1):
            w = ' '.join(toks[i:i + n])
            if w in vocab: break
        else: return None, toks[i]
        if vocab[w]: out.append(vocab[w])
        i += n
    return out, None

def accepts(pat, groups):
    i = 0
    for k, p in enumerate(pat):
        if p == 9999: return True
        if i >= len(groups): return False
        if p != 1 and p != groups[i]: return False
        i += 1
    return i == len(groups)

_logic = {}
def logic(room):
    if room not in _logic:
        f = os.path.join(logdir, 'logic%03d.txt' % room)
        code = [l for l in open(f).read().split('\n') if re.match(r'^[0-9a-f]{4}: ', l)] if os.path.exists(f) else []
        saids = []
        for i, l in enumerate(code):
            for m in re.finditer(r'said\(([\d,]+)\)', l):
                dist = []                  # distance(0, obj, v) + lessn(v, d) in the branch -> (obj, d)
                posn = []                  # every posn(0, ...) box in the branch (e.g. one per door), also
                dvar = None
                for k in [l] + code[i + 1:i + 16]:  # one tested together with the said() on its own line
                    if k is not l and 'said(' in k: break
                    p = re.search(r'posn\(0,(\d+),(\d+),(\d+),(\d+)\)', k)
                    if p: posn.append(tuple(int(a) for a in p.groups()))
                    d = re.search(r'distance\(0,(\d+),v(\d+)\)', k)
                    if d: dvar = (int(d.group(1)), d.group(2))
                    d = re.search(r'lessn\(v(\d+),(\d+)\)', k)
                    if d and dvar and d.group(1) == dvar[1]: dist.append((dvar[0], int(d.group(2))))
                saids.append(([int(a) for a in m.group(1).split(',')], posn, dist))
        views = set((int(a), int(b)) for a, b in re.findall(r'set\.view\((\d+),(\d+)\)', '\n'.join(code)))
        _logic[room] = (code, saids, views)
    return _logic[room]

bad = 0; n = 0
for fn in sorted(os.listdir(HS)):
    for ln, l in enumerate(open(os.path.join(HS, fn)), 1):
        m = re.match(r'\s*(OBJ|AREA)\((.*)\),\s*$', l)
        if not m: continue
        n += 1
        args = m.group(2)
        cmd = re.search(r'"([^"]*)"', args).group(1)
        dst = re.search(r'DIST\((\d+), (\d+)\)', args)
        dst = tuple(int(a) for a in dst.groups()) if dst else None
        near = re.search(r'NEAR\((\d+), (\d+), (\d+), (\d+)\)', args)
        near = tuple(int(a) for a in near.groups()) if near else None
        nums = [int(a) for a in re.findall(r'\d+', re.split('NEAR|ANY|DIST', args)[0])]
        room = nums[0]
        where = '%s:%d room %d "%s"' % (fn, ln, room, cmd)
        code, saids, views = logic(room)
        if not code: print(where, ': no logic for this room'); bad += 1; continue
        if m.group(1) == 'OBJ' and (nums[1], nums[2]) not in views:
            print(where, ': object %d never gets view %d in this logic' % (nums[1], nums[2])); bad += 1
        if m.group(1) == 'AREA' and not (nums[1] <= nums[3] <= 159 and nums[2] <= nums[4] <= 167):
            print(where, ': bad box'); bad += 1
        if len(cmd) > 40: print(where, ': command too long'); bad += 1
        groups, unk = parse(cmd)
        if groups is None: print(where, ': unknown word "%s"' % unk); bad += 1; continue
        if any(accepts(s[0], groups) for s in logic(0)[1]):
            print(where, ': logic 0 (runs first) takes this command'); bad += 1; continue
        hit = [s for s in saids if accepts(s[0], groups)]
        if not hit: print(where, ': no said() for groups %s in this room' % groups); bad += 1; continue
        posns = hit[0][1]                  # the first matching said() is the one that fires
        if near and near not in posns:
            print(where, ': NEAR%s is not the posn box after that said (%s)' % (near, posns or 'none')); bad += 1
        dists = hit[0][2]
        if dst and dst not in dists:
            print(where, ': DIST%s is not the distance test of that said (%s)' % (dst, dists or 'none')); bad += 1
        if not near and not dst and dists:
            print(where, ': ANY but the logic checks distance to object %d < %d' % dists[0]); bad += 1
        if not near and posns:
            print(where, ': ANY but the logic checks posn%s' % (posns[0],)); bad += 1
print('%d hotspots, %d problems' % (n, bad))
sys.exit(1 if bad else 0)
