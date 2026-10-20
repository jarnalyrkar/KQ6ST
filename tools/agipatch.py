# Patch AGI v2 logic bytecode and write it back as a new resource.
#   code = replace(code, start, old_hex, new_hex)  - replace bytes, fix every jump crossing the change
#   write_logics(srcdir, dstdir, {num: (code, msgdata)})  - copy LOGDIR/VOL.*, append the logics to the
#       last VOL file and point LOGDIR at them (original resources stay in place, unused)
# Jumps: "if" blocks end with a signed 16-bit skip offset, "goto" (0xfe) has one; both are relative
# to the end of the instruction. A jump whose target is at or after the end of the replaced range moves
# with it; a target inside the range (other than its start) is an error.
import os, sys, struct, json, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import agires
OPS = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ops.json')))
COND, ACT = OPS['cond'], OPS['act']
# Sierra ST AGI 1.422 differs from DOS from 174 on: 174 hides the pointer (0x67bc), 175 shows it (0x67dc),
# 177 takes two vars (click capture, text 0x67fc), not DOS set.pri.base(n) / discard.sound(n) / allow.menu(n)
ACT[174] = ['hide.mouse', '']
ACT[175] = ['show.mouse', '']
ACT[177] = ['get.mse.posn', 'vv']

def jumps(code):
    """[(pos_of_offset_word, end_of_instruction)] for every if/goto in the code."""
    out = []; pc = 0
    while pc < len(code):
        op = code[pc]; pc += 1
        if op == 0xff:
            while True:
                c = code[pc]; pc += 1
                if c == 0xff: break
                if c in (0xfc, 0xfd): continue
                if c == 0x0e: pc += 1 + 2 * code[pc]; continue
                pc += len(COND[c][1])
            out.append((pc, pc + 2)); pc += 2
        elif op == 0xfe:
            out.append((pc, pc + 2)); pc += 2
        else:
            pc += len(ACT[op][1])
    return out

def replace(code, start, old_hex, new_hex, drop_inner=False):
    # drop_inner: the range holds whole if/goto instructions that go away with it (their jumps are dropped)
    old, new = bytes.fromhex(old_hex), bytes.fromhex(new_hex)
    assert code[start:start + len(old)] == old, 'expected %s at %04x, found %s' % (old.hex(), start, code[start:start + len(old)].hex())
    end = start + len(old); delta = len(new) - len(old)
    out = bytearray(code[:start] + new + code[end:])
    for pos, e in jumps(code):
        if start <= pos < end:
            if drop_inner: continue
            raise ValueError('jump inside the replaced range at %04x' % pos)
        target = e + struct.unpack('<h', code[pos:pos + 2])[0]
        if start < target < end: raise ValueError('jump into the replaced range at %04x' % pos)
        npos = pos + (delta if pos >= end else 0); ne = e + (delta if e > start else 0)
        nt = target + (delta if target >= end else 0)
        struct.pack_into('<h', out, npos, nt - ne)
    return bytes(out)

def split(data):
    """logic resource -> (code, raw message section)"""
    cl = struct.unpack('<H', data[:2])[0]
    return data[2:2 + cl], data[2 + cl:]

def write_logics(src, dst, logics):
    os.makedirs(dst, exist_ok=True)
    vols = sorted(f for f in os.listdir(src) if f.upper().startswith('VOL.'))
    for f in ['LOGDIR'] + vols:
        if os.path.abspath(src) != os.path.abspath(dst): shutil.copyfile(os.path.join(src, f), os.path.join(dst, f))
    last = max(vols, key=lambda f: int(f.split('.')[1])); vn = int(last.split('.')[1])
    vol = bytearray(open(os.path.join(dst, last), 'rb').read())
    ld = bytearray(open(os.path.join(dst, 'LOGDIR'), 'rb').read())
    for num, (code, msgs) in sorted(logics.items()):
        res = struct.pack('<H', len(code)) + code + msgs
        off = len(vol); assert off < 1 << 20
        vol += bytes([0x12, 0x34, vn]) + struct.pack('<H', len(res)) + res
        ld[num * 3:num * 3 + 3] = bytes([(vn << 4) | (off >> 16), (off >> 8) & 255, off & 255])
    open(os.path.join(dst, last), 'wb').write(vol)
    open(os.path.join(dst, 'LOGDIR'), 'wb').write(ld)

KEY = b'Avis Durgan'

def messages(raw):
    """raw message section -> {number: bytes} (AGI v2: count, end offset, pointers, XOR-encrypted text)"""
    n = raw[0]; txt = bytes(b ^ KEY[i % 11] for i, b in enumerate(raw[3 + 2 * n:])); out = {}
    for i in range(n):
        p = struct.unpack('<H', raw[3 + 2 * i:5 + 2 * i])[0]
        if p: s = p - 2 - 2 * n; out[i + 1] = txt[s:txt.index(b'\0', s)]
    return out

def encode_messages(msgs):
    """{number: bytes} -> raw message section"""
    n = max(msgs); txt = b''; ptrs = b''
    for i in range(1, n + 1):
        if i in msgs: ptrs += struct.pack('<H', len(txt) + 2 + 2 * n); txt += msgs[i] + b'\0'
        else: ptrs += b'\0\0'
    return bytes([n]) + struct.pack('<H', 2 + 2 * n + len(txt)) + ptrs + bytes(b ^ KEY[i % 11] for i, b in enumerate(txt))

def add_messages(raw, texts):
    """append texts (str) as new messages -> (new raw section, number of the first one)"""
    msgs = messages(raw); first = max(msgs) + 1
    for i, t in enumerate(texts): msgs[first + i] = t.encode('latin1')
    return encode_messages(msgs), first
