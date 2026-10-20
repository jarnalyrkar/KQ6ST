# Vocabulary additions to WORDS.TOK: wordpatches.py <srcdir> <outdir>
# Reads <srcdir>/WORDS.TOK, adds the words below and writes <outdir>/WORDS.TOK.
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from words import load

ADD = {
    'load': 4,      # synonym of "restore" (group 4)
}

def save(groups, path):
    words = sorted((w, g) for g, ws in groups.items() for w in ws)
    hdr = [0]*26; body = bytearray(); prev = ''
    for w, g in words:
        i = ord(w[0]) - ord('a')
        if 0 <= i < 26 and not hdr[i]: hdr[i] = 52 + len(body)
        keep = 0
        while keep < min(len(w), len(prev)) and w[keep] == prev[keep]: keep += 1
        body.append(keep)
        rest = w[keep:].encode('latin1')
        body += bytes(c ^ 0x7f for c in rest[:-1]) + bytes([(rest[-1] ^ 0x7f) | 0x80])
        body += struct.pack('>H', g); prev = w
    open(path, 'wb').write(struct.pack('>26H', *hdr) + body + b'\0')

if __name__ == '__main__':
    g = load(os.path.join(sys.argv[1], 'WORDS.TOK'))
    for w, n in ADD.items():
        for ws in g.values():
            if w in ws: ws.remove(w)
        g.setdefault(n, []).append(w)
    save(g, os.path.join(sys.argv[2], 'WORDS.TOK'))
