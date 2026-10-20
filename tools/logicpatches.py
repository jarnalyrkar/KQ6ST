# Game-script fixes for the ST version. logicpatches.py <original game dir> <target dir>
# Reads the logics from the original files (work/est), applies every patch below and writes
# LOGDIR + VOL.* to the target (e.g. build/KQ6ST-HD). Run it again after adding a patch.
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import agires
from agipatch import replace, split, write_logics, add_messages

def p015(code):
    # Cliffs of Logic: stepping onto the first stone (after the first etching raised the stones)
    # goes straight to the top (room 25) instead of the climb (rooms 239/238 and puzzles 235-237, 100).
    # Sets what the climb would: +4 points (one per etching) and the etching flags 143, 145, 146, 147
    # (only read by the cliff rooms). Compared with the user's saves "cliffs-1" -> "top of cliffs".
    return replace(code, 0x0258,
                   '6503' '12ef',                         # print(m3)  new.room(239)
                   '6503' '050304'                        # print(m3)  addn(v3,4)
                   '0c8f' '0c91' '0c92' '0c93'            # set(143) set(145) set(146) set(147)
                   '1219')                                # new.room(25)

def said(*words):
    return '0e%02x' % len(words) + ''.join('%02x%02x' % (w & 255, w >> 8) for w in words)

def ifblock(conds, body):
    # if (conds) { body }: conds/body as hex, conds already joined (use 'fc...fc' for or-brackets)
    b = bytes.fromhex(body)
    return 'ff' + conds + 'ff' + '%02x%02x' % (len(b) & 255, len(b) >> 8) + body

def either(*conds):
    return 'fc' + ''.join(conds) + 'fc'

def p008(code, msgs):
    # Pawn shop counter (M8, user request 2026-10-19): typed verbs instead of the TALK / TRADE / TAKE MINT /
    # EXIT icon menu. The menu is never opened; typed commands at the counter jump straight into the
    # bodies of the menu branches, so conversations, trades and points are unchanged. They jump instead
    # of setting the menu variable v102: with v102 > 0 the menu block runs set.cel on the icon objects.
    # Applied from the highest offset down; the new gotos (inserted first) are fixed up by later replaces.
    COUNTER = '0b' '00' '5c' '71' '64' '81'               # posn(0,92,113,100,129), the menu's own test
    TALK, TRADE, MINT = 0x0653, 0x0538, 0x06c6            # menu branch bodies (TALK after its v102 test,
                                                          # TRADE at print(m45), MINT after v102=3)
    # 1. Replace the talk handler at 0x033f (said talk / talk old man|beggar|man -> m42 "not close
    #    enough", anywhere in the room). Words: talk 25, trade 196, give/offer/show 1012, take/get 33,
    #    mint 103, candy dish 68, man 76, old man 77, beggar 46, merchant/owner 78, pawn owner 79;
    #    9999 = rest of line. Away from the counter: m44 "...he should step up to the counter."
    old = 'fffc0e0119000e0219004d000e0219002e000e0219004c00fcff0200652a'
    # Mouse menu (user request 2026-10-20): clicking the owner types "use counter" (use 22, counter 66; not
    # handled in this room before), which walks ego to the counter and opens the ferryman's kind of menu
    # (logic 12): a blank print.at box, numbered lines, get.num. The interpreter makes the lines
    # clickable (slots/getnum.c). Answer in v249 (used by no logic): 1 Talk, 2 Trade, 3 Take mint, 4 Leave.
    texts = ['                    ' + '\n' * 10, 'Pawn shop', '1. Talk', '2. Trade', '3. Take mint', '4. Leave',
             'Please Select: ']
    msgs, m = add_messages(msgs, texts)
    ask = ('0c0f' '97%02x020a15' % m +                  # set(15) print.at(box, row 2, col 10, width 21)
           '6d000f' '670310%02x' % (m + 1) +             # set.text.attribute(0,15) display(3,16,"Pawn shop")
           ''.join('67%02x0c%02x' % (5 + 2 * i, m + 2 + i) for i in range(4)) +   # rows 5/7/9/11, col 12
           '6d0f00' '76%02xf9' % (m + 6) + 'a9')        # set.text.attribute(15,0) get.num(v249) close.window
    pre = ifblock(COUNTER + said(22, 66), ask)
    parts = [                                            # (conds, body, target): if (conds) { body goto target }
        ('ff' + COUNTER + either(said(25), said(25, 78), said(25, 79)) + 'ff', '', TALK),
        ('ff' + COUNTER + 'fd0772' + said(25, 76) + 'ff', '', TALK),    # "man" is the owner unless the
                                                                        # old man (flag 114) is there
        ('ff' + COUNTER + either(said(196, 9999), said(1012, 9999)) + 'ff', '', TRADE),
        ('ff' + COUNTER + either(said(33, 103), said(33, 68)) + 'ff', '', MINT),
        ('ff01f901ff', '03f900', TALK),                  # menu answers (v249 = 0 again)
        ('ff01f902ff', '03f900', TRADE),
        ('ff01f903ff', '03f900', MINT),
    ]
    away = either(said(25), said(25, 78), said(25, 79), said(196, 9999), said(1012, 9999), said(22, 66))
    tail = (ifblock(away, '652c')                                                                          # m44
            + ifblock(either(said(25, 77), said(25, 46), said(25, 76), said(33, 103), said(33, 68)), '652a'))  # m42
    blen = lambda h: len(bytes.fromhex(h))
    size = blen(pre) + sum(blen(c) + 2 + blen(b) + 3 for c, b, _ in parts) + blen(tail)
    delta = size - blen(old)
    out, pos = pre, 0x033f + blen(pre)
    for conds, body, target in parts:
        pos += blen(conds) + 2 + blen(body) + 3
        off = (target + delta - pos) & 0xffff
        skip = blen(body) + 3
        out += conds + '%02x%02x' % (skip & 255, skip >> 8) + body + 'fe%02x%02x' % (off & 255, off >> 8)
    code = replace(code, 0x033f, old, out + tail, drop_inner=True)
    at = lambda o: o + delta                             # original offset -> offset now
    # 4. (highest offset first) TRADE item path: no icons to erase. Was erase(5..8) discard.view(6)
    #    new.room(234). Clearing v100 is the G7 fix: v100 is ego's direction here, logic 234 reads it as
    #    the talk topic (giving the bird after walking down gave the map talk and lost the bird).
    code = replace(code, at(0x05fb), '2405240624072408200612ea', '036400' '12ea')   # assignn(v100,0) new.room(234)
    # 3. TRADE refusals and cancel set v102 = 2 (back to the menu at TRADE): set 0 (no menu).
    for o in (0x054e, 0x0565, 0x0577, 0x0589, 0x059b, 0x05ad, 0x05bf, 0x05d1, 0x05e3, 0x05f5):
        code = replace(code, at(o), '036602', '036600')
    # 2. Never open the menu: drop the block that draws the icons at the counter (0x0396-0x0437).
    menu = code[at(0x0396):at(0x0437)]
    assert menu.startswith(bytes.fromhex('ff' + COUNTER + 'ff9700'))
    code = replace(code, at(0x0396), menu.hex(), '', drop_inner=True)
    return code, msgs

def p025(code):
    # Top of the cliffs: with the minotaur dead (flag 117) ego is only drawn when arriving from
    # 238/239/233/2, so after the cliffs skip (arrival from 15, see p015) ego stayed invisible, the
    # walk-in never ended and input stayed off; Night Mare just grazed (long path, found by the user
    # 2026-10-19). Add room 15 to that list: if || v1==238 v1==239 v1==233 v1==2 || draw(0).
    return replace(code, 0x027d,
                   '010102',                              # equaln(v1,2)
                   '010102' '01010f')                     # equaln(v1,2) equaln(v1,15)

def p097(code):
    # Disclaimer screen (user request 2026-10-19): Esc skips it too. Any key already did (have.key),
    # but Esc is mapped to controller 19 (logic 91, the menu key), and when the poll maps it first
    # have.key does not see it (see p255).
    return replace(code, 0x00b7,
                   'ff0dff',                              # if have.key()
                   'ff' + either('0d', '0c13') + 'ff')    # if || have.key() controller(19) ||

def p255(code):
    # Title room (user request 2026-10-19): Esc skips the second disclaimer and the title texts and goes
    # straight to the Opening / Play / Restore choice, keeping the title picture, sparkles and music.
    # Before the choice is shown, flag 41 (picture drawn) is clear or one of the text timers v106-v109
    # is running (they hand over without a gap). Inserted before 0x01b0 (if v106 == 1), so it runs after
    # the drawing block in the same cycle. v104 (unused here, cleared by logic 101) = Esc pending.
    # Esc arrives two ways: as controller 19 (mapped by the per-cycle poll), or as the raw key 27 in v19
    # when have.key() (text 0x1b0c) pulls it from the event queue first; the right button is 27 too.
    # So a pending key is pulled into v19 here (any other key stays there for the have.key at 0x02ec).
    pull = ifblock('0d', '')                                            # if have.key() {}
    esc = ifblock(either('0c13', '01131b') + either('fd0729', '056a00', '056b00', '056c00', '056d00'),
                  '036801' '036400' '036500' '036600' '0c25' '0c26')   # v104=1 v100..v102=0 set(37) set(38)
    # flags 37/38 = disclaimer texts done; 0x008b then starts the music and v103 = 30 before drawing:
    # cut that wait short
    wait = ifblock('016801' '0727' 'fd0729' '056701', '036701')         # v104==1 39 !41 v103>1: v103=1
    # picture drawn: stop the text timers and let 0x01f9 (v109 == 1) show the choice now
    menu = ifblock('016801' '0729', '036800' '036a00' '036b00' '036c00' '036d01')
    # the choice takes any key: not Esc (either way it arrives)
    code = replace(code, 0x02ec, 'ff0dff', 'ff0d' 'fd0c13' 'fd01131b' 'ff')
    # Mouse on the choice (user request 2026-10-20): moving the pointer onto Opening / Play / Restore moves
    # the marker there, a left click on one picks it. Runs every cycle before get.dir (0x0224; the "choice
    # drawn" block jumps there). The interpreter keeps the pointer in v240/v241 (menu.c; logic 0 arms
    # get.mse.posn every cycle), flag 19 = clicked this cycle. Hover acts only when the pointer moved
    # (v246/v247 = last position, free in this room), so the arrow keys still work with the mouse at rest.
    # A click walks ego (= the marker) toward it: stop.motion/start.motion cancel that. The click then sets
    # v19 = 13 (a key), so the have.key() test at 0x02ec starts the marked entry.
    shown = either('072b', '072c', '072d')                              # isset(43/44/45): choice is up
    out = ifblock(shown + '0713', '4d00' '4e00')                        # click: stop.motion start.motion
    show = '6714040b' '6714120c' '67141f0d'                             # display Opening / Play / Restore
    for flag, mx, x0, x1 in ((0x2b, 13, 10, 45), (0x2c, 63, 60, 90), (0x2d, 120, 116, 154)):
        on = '05f195' '03f1a4' '05f0%02x' % (x0 - 1) + '03f0%02x' % (x1 + 1)   # v241 150..163, v240 x0..x1
        pick = ('0d2b' '0d2c' '0d2d' '0c%02x' % flag +                  # reset 43-45, set this one
                '2400' '2500%02xa2' % mx + '2300' '3a00' + show)        # erase(0) position(0,mx,162) draw stop.update
        out += ifblock(shown + either('fd02f0f6', 'fd02f1f7') + on + 'fd07%02x' % flag, pick)
        out += ifblock(shown + '0713' + on, pick + '03130d')            # click: also v19 = 13
    code = replace(code, 0x0224, '57006e', out + '04f6f0' '04f7f1' '57006e')   # v246/v247 = now; get.dir
    # Play (flag 44, pointer at x 63) is preselected instead of Opening (flag 43, x 13), user request
    code = replace(code, 0x0221, '0c2b', '0c2c')                       # set(43) -> set(44)
    code = replace(code, 0x020d, '25000da2', '25003fa2')               # position(0,13,162) -> (0,63,162)
    return replace(code, 0x01b0, 'ff016a01ff', pull + esc + wait + menu + 'ff016a01ff')

# M5: black text on a blue box. The game asks for set.text.attribute(0,1); DOS AGI draws any non-black
# background as white in graphics mode, the ST interpreter uses the colour as given (1 = blue). Give the
# text the colour of what it is printed on (DOS still shows white, so nothing changes there).
YELLOW, BROWN, WHITE = '6d000e', '6d0006', '6d000f'      # set.text.attribute(0,14 / 6 / 15)

def p012(code):
    # ferryman's "Ask about" list, on the white print.at window
    return replace(code, 0x027d, '6d0001', WHITE)

def p014(code):
    # magic map: island names on the yellow parchment, numbers on the brown islands
    code = replace(code, 0x012f, '670b1a0d', BROWN + '670b1a0d')          # "3" (Isle of the Beast redrawn)
    code = replace(code, 0x0128, '6d0001', YELLOW)                        # "the Beast"
    code = replace(code, 0x006e, '6707120b', BROWN + '6707120b')          # "1" .. "4"
    code = replace(code, 0x0046, '67101f05', BROWN + '67101f05' + YELLOW) # "5" (Isle of the Mists)
    return replace(code, 0x0029, '6d0001', YELLOW)                        # names

def p072(code):
    # the vizier's letter, on white paper
    return replace(code, 0x0026, '6d0001', WHITE)

# Mouse-driven game cursors (user request 2026-10-20). Rooms 210 (vizier's ebony box), 211 (trunk of
# letters), 212 (beach box under the board) and the sword fight (111) make ego a cursor, moved with the
# arrow keys; any key runs posn() tests on its position. Now the mouse drives it: the pointer is hidden
# while a cursor is up and the cursor's tip follows the mouse; a left click counts as the key.
#   - ST command 177 get.mse.posn(va,vb) (text 0x67fc) clears flag 19 and names two vars; a left click in
#     play (0x6624) stores x/4 in va and y - 8 (play area) in vb, sets flag 19 (until re-armed), and walks
#     ego to the click (motion 4). 174 hide.mouse / 175 show.mouse hide and show the pointer.
#   - While the pointer is hidden, the interpreter's per-cycle hook (slots/menu.c) writes the pointer
#     position into the get.mse.posn vars every cycle.
#   - At the end of each cycle (CURSOR): if the mouse moved (v240/v241 differ from last cycle's v246/v247)
#     or was clicked, the cursor's tip goes to it (ego = (x - sx, y + sy), clamped to the cursor's range),
#     stop/start.motion cancel the click walk; a click sets v242 = 1 and re-arms. The next cycle (cursor
#     drawn there) the room's test runs as if a key was pressed (|| have.key() equaln(v242,1) ||).
#   - Capture is re-armed at the end of logic 0 every cycle, so flag 19 means "clicked this cycle" for the
#     room logic and for logic 88 (portrait dialogues, which now also close on a click).
#     The keys still move the cursor; it only jumps to the mouse when the mouse moves. v248 = cycles to
#     wait after hiding (the first written position is only there one cycle later), so the cursor does not
#     jump to wherever the mouse was left. A click still moves the cursor if the pointer is visible (e.g.
#     after restoring a game made during a step: logic 0 shows the pointer on a room change or restore).
#   v240-v248 are used by no logic (v243-v245 only in 111).
ARM = 'b1f0f1'                                            # get.mse.posn(v240,v241)
HIDE = 'ae' '03f802'                                      # hide.mouse v248 = 2
SHOW = 'af'                                               # show.mouse

def cursor(steps):
    # steps: [(cond, sx, sy, xmin, xmax, ymin, ymax)]: cond (hex) = this cursor is up ('' = always),
    # (sx, sy) = from the tip to ego's position (left, bottom), range = allowed ego position
    out = '03f200'                                                       # v242 = 0
    for cond, sx, sy, x0, x1, y0, y1 in steps:
        pos = '04f4f0'                                                   # v244 = v240 (- sx, at least 0)
        if sx: pos += ifblock('03f4%02x' % sx, '03f4%02x' % sx) + '07f4%02x' % sx
        pos += '04f5f1' '05f5%02x' % sy                                  # v245 = v241 + sy
        for v, lo, hi in (('f4', x0, x1), ('f5', y0, y1)):
            pos += ifblock('03%s%02x' % (v, lo), '03%s%02x' % (v, lo)) + ifblock('05%s%02x' % (v, hi), '03%s%02x' % (v, hi))
        moved = '01f800' + either('fd02f0f6', 'fd02f1f7', '0713')        # v248 == 0, moved or clicked
        body = (ifblock(moved, pos + '4d00' '9400f4f5' '4e00')           # stop.motion reposition start.motion
                + ifblock('0713', '03f201'))                             # click: v242 = 1
        out += ifblock(cond, body) if cond else body
    out += ifblock('05f800', '02f8')                                     # if v248 > 0: v248--
    return out + '04f6f0' '04f7f1'                                      # v246/v247 = now (logic 0 re-arms)

KEY = ('ff0dff', 'ff' + either('0d', '01f201') + 'ff')                   # || have.key() equaln(v242,1) ||
ARROW = (0, 6)                                                           # view 154 (11x12, points left): tip column 0, row 5
SWORD = (16, 30)                                                         # view 194 cel 1 (18x33): tip column 16, row 2

def cursor_room(code, cond, have_key, ret):
    # have_key: if have.key() (inside if !isset(5)), ret: the final return(). Range: the rooms' own
    # limits (they nudge the cursor back from x < 40, x > 90, y > 130); y >= 12 keeps the arrow on screen.
    code = replace(code, ret, '00', cursor([(cond,) + ARROW + (40, 90, 12, 130)]) + '00')
    code = replace(code, have_key, *KEY)
    return replace(code, 0x0006, '1665', '1665' + ARM + HIDE)            # call(101): arm and hide on entry

def p210(code): return cursor_room(code, '', 0x0091, 0x00c8)
def p211(code): return cursor_room(code, 'fd0938', 0x00a6, 0x00eb)      # ! has(56): no cursor once read
def p212(code): return cursor_room(code, 'fd0732', 0x00d9, 0x0108)      # ! isset(50): coin already taken

def p111(code):
    # Sword fight with the vizier: three cursor steps.
    #   1. arrow cursor grabs the sword on the wall before the vizier arrives: flag 41,
    #      block(98,78,109,125), test 0x013e
    #   2. sword cursor strikes: v103 == 1, block(79,114,102,158), test 0x01c7 (100-cycle limit, v102)
    #   3. sword cursor, the WHAP: flag 139, block(19,106,39,165), test 0x03f7 (limit v153)
    # v105-v110 are this room's, so the position is built in v244/v245. Ego is confined by block(), so the
    # cursor is clamped strictly inside the step's block (moving ego out would trap it outside, keys and
    # all). A step's cursor is up only while drawn: v243 = step (1-3), set where the cursor is drawn (pointer
    # hidden there), 0 at every erase(0) (pointer shown). The tests alone are not enough: after a time-out
    # the cursor is erased but flag 41 / v103 stay set, and a click could move it onto the target.
    steps = [('01f3%02x' % n,) + tip + (x1 + 1, x2 - 1, y1 + 1, y2 - 1) for n, tip, x1, y1, x2, y2 in
             ((1, ARROW, 98, 78, 109, 125), (2, SWORD, 79, 114, 102, 158), (3, SWORD, 19, 106, 39, 165))]
    nocursor = ('2400', '2400' '03f300' + SHOW)                          # erase(0) v243 = 0
    for o, old, new in sorted([
            (0x0626, '00', cursor(steps) + '00'),
            (0x05e0, '2300', '2300' '03f302' + HIDE),                    # draw(0) sword, step 2
            (0x051d, '2300', '2300' '03f303' + HIDE),                    # draw(0) sword, step 3
            (0x049b,) + nocursor, (0x0409,) + nocursor, (0x01dd,) + nocursor, (0x014f,) + nocursor,
            (0x00d4,) + nocursor,
            (0x03f7,) + KEY, (0x01c7,) + KEY, (0x013e,) + KEY,
            (0x0006, '1665', '1665' + ARM + '03f200' '03f301' + HIDE),   # hand drawn at init: step 1
            ], reverse=True):                                            # highest offset first
        code = replace(code, o, old, new)
    return code

def p000(code):
    # main loop: re-arm click capture at the end of every cycle (after the room logic and logic 88 saw this
    # cycle's click; also keeps a click from ever writing v0/v1, the interpreter's default target). The
    # pointer comes back on every room change (flag 5) and after a restore (flag 12), in case a cursor room
    # hid it (see cursor()).
    code = replace(code, 0x0503, '00', ARM + '00')
    return replace(code, 0x0000, 'ff051100ff', ifblock(either('0705', '070c'), SHOW) + 'ff051100ff')

def p088(code):
    # portrait dialogues (print.at with flag 15: the game keeps running, logic 88 closes the window on a
    # key): a left click closes them too. While the window is up the interpreter is in text mode, where a
    # left click is posted as key event 0x201 (0x6612); the per-cycle poll applies the set.key table first
    # (0x25c6) and logic 91's set.key(1,2,19) turns it into controller 19 (the menu / Esc controller), so
    # have.key() never saw it. Plain print() boxes wait in 0x2560, which skips that table: they already
    # closed on a click. So: || have.key() controller(19) isset(19) || (Esc closes them too now; flag 14
    # is off during these dialogues, so it never opened the menu there).
    return replace(code, 0x022f, 'ff0dff', 'ff' + either('0d', '0c13', '0713') + 'ff')

PATCHES = {0: p000, 8: p008, 88: p088, 111: p111, 12: p012, 14: p014, 15: p015, 25: p025, 72: p072, 97: p097, 210: p210, 211: p211,
           212: p212, 255: p255}

if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    d = agires.dirent(src, 'LOGDIR'); out = {}
    for num, fn in PATCHES.items():
        code, msgs = split(agires.res(src, *d[num]))
        out[num] = fn(code, msgs) if fn.__code__.co_argcount == 2 else (fn(code), msgs)   # (code, msgs) to add texts
        print('logic %d: %d -> %d bytes of code' % (num, len(code), len(out[num][0])))
    write_logics(src, dst, out)
