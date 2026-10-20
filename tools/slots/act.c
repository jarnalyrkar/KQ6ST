/* Click to interact (user request 2026-10-20, see CLICK-TO-INTERACT.md). A left double click or a right
   click in play on something in the room types the obvious command for it ("get ring", "open box",
   "look ocean"), as if the player had typed it: the room logic then does the rest (the "not close enough"
   check, score, item). Nothing happens on a click that hits no hotspot.
   The interpreter detects double clicks itself (KQ6 sets flag 8; window 15 ticks), so the first click of a
   double click never walks. In play a double click posts a stop (event type 2, code 0) at text 0x66ea and
   a right click posts 0x101 at 0x673c (controller 30, unused by KQ6); both calls go through here. The
   click only notes the request; act_poll (from menu_poll, after the per-cycle event poll, before logic 0,
   flags 2 and 4 already reset for the cycle) finds the hotspot and runs the interpreter's own parser
   (0x7022) on its command, which fills the word list said() reads and sets flag 2 and v9.
   Hotspots (one file per island under hotspots/, listed in hotspots.h): an animated object (live position
   and cel pixels, so it is gone once erased; the view number must match) or a box in play-area
   coordinates; the first hit in the room's list wins. A
   click on ego himself does nothing. Input must be accepted (accept.input) and nothing typed that cycle.
   Walking there: a hotspot can carry the room logic's stand-here box (its posn(0, ...) test). If ego is
   outside it and under player control, he walks in a straight line to the nearest point of the box (the
   click walk's motion 4: target +0x40/+0x42, step +0x44, see text 0x6236; on arrival 0x616e clears the
   motion) and the command is typed when he is inside. Or DIST(obj, d), for logics that test
   distance(0, obj, v) < d instead (text 0x1400: |dy| + |d(x + width / 2)|): he walks toward that object
   until the distance is below d. He aims a little in front of (below) it, re-aimed every cycle as it
   may move: objects on the same baseline block each other, which would keep him too far away. If he stops moving short of it (an obstacle), it is
   typed anyway, so the room answers "not close enough". The walk is dropped when the player takes over
   (another click or arrow key changes the motion or target), on a room change and when ego is erased.
   While the game holds player control or input (a scripted walk such as room 7's way in past the stalls when
   ego touches its signal line, logic 88's portrait talk), the walk waits; when it gives both back, the
   command is typed if ego is now in the spot, else he sets off again from where he is (at most 5 times).
   If the straight line is blocked (barriers, water, horizon), pf.c searches a way around and he walks it
   in pieces; for DIST he goes straight again once that works from the end of a piece (the object may walk).
   Plain clicks (user request 2026-10-20): the interpreter's click walk (text 0x6236, called at 0x668c) is a
   straight line to the click. It now goes through click_walk: if the line is blocked, or the click is past an
   edge (above the horizon, or close to the left, right or bottom edge), pf_click finds a way and he walks it in
   the same pieces, then on through the edge, which changes the room. A click he cannot reach takes him as
   near to it as he can get. The game taking ego over pauses this walk as above. */
typedef short W;
typedef unsigned char UB;

extern long ev_post(W type, W code);
extern void parse_str(char *s);
extern W is_flag(W n);
extern W text_mode, menu_state, dlg_busy, ff0a, input_on, play_top, player_ctl;
extern volatile W mouse_x2, mouse_y2;
extern UB agi_vars[];
extern UB *obj_tab, *obj_last;
extern void pf_ego(void);
extern W pf_line(W x, W y, W tx, W ty), pf_plan(const UB *st, const UB *o), pf_next(W *tx, W *ty);
extern W pf_click(W tx, W ty, W *ox, W *oy);
extern void walk_orig(W mx, W my);

#define BOX 0xff
struct hs { UB room, obj, view, x1, y1, x2, y2, st[4]; const char *cmd; };
#define OBJ(r, o, v, st, c) { r, o, v, 0, 0, 0, 0, st, c }
#define AREA(r, x1, y1, x2, y2, st, c) { r, BOX, 0, x1, y1, x2, y2, st, c }
#define NEAR(x1, y1, x2, y2) { x1, y1, x2, y2 }
#define ANY { 0, 0, 0, 0 }
#define DIST(o, d) { 0xff, o, d, 0 }
static const struct hs HS[] = {
#include "hotspots.h"
};

static W req, rx, ry;
static const struct hs *walk;             /* walking to this hotspot's stand-here box */
static W wroom, wx, wy, lastx, lasty, still;
static W path, replans;                   /* walking a path from pf.c in pieces; searches so far */
static W held, resumes;                   /* the game took ego over; times the walk went on after that */
static W creq, ctx, cty;                  /* a plain click walk to ctx, cty (ego's target, as 0x6236 sets it) */
static W cw, cox, coy;                    /* walking a click path; then on to cox, coy (past an edge to leave) */

#define E(off) (*(W *)(obj_tab + (off)))  /* ego = object 0 */

static W in_play(void) { return !text_mode && ff0a != 1 && !menu_state && !dlg_busy; }

static void note(void)
{
    req = 1;
    rx = mouse_x2 >> 2;
    ry = mouse_y2 - play_top;
}

long act_dbl(W type, W code)              /* in play only (0x669e); type 2, code 0 = stop ego */
{
    long r = ev_post(type, code);
    if (in_play()) note();
    return r;
}

long act_right(W type, W code)            /* also in text mode when the menu may open from there */
{
    long r = ev_post(type, code);
    if (in_play()) { ev_post(2, 0); note(); }
    return r;
}

/* Is pixel (px, py) of the cel opaque? AGI cel: width, height, mirror/loop/transparent colour, then
   rows of (colour << 4 | count) runs, each row ended by 0. Mirrored when bit 7 is set and the loop
   stored in bits 4-6 is not the one drawn (the data is flipped in place on first use). */
static W cel_pix(const UB *c, W loop, W px, W py)
{
    W w = c[0], h = c[1], t = c[2] & 15, x, row;
    UB b;
    if (px < 0 || py < 0 || px >= w || py >= h) return 0;
    if ((c[2] & 0x80) && ((c[2] >> 4) & 7) != loop) px = w - 1 - px;
    c += 3;
    for (row = 0; row < py; row++) {      /* not "while (*c++);": gcc -mshort indexes that with 0x0000ffff */
        while (*c) c++;
        c++;
    }
    for (x = 0; (b = *c++) != 0; x += b & 15)
        if (px < x + (b & 15)) return (b >> 4) != t;
    return 0;
}

static W obj_hit(W n, W view, W x, W y, W tol)
{
    UB *o = obj_tab + n * 0x48;
    W ox, top, dx, dy, loop;
    const UB *cel;
    if (!obj_tab || o > obj_last || !(*(W *)(o + 0x3e) & 1)) return 0;
    if (view >= 0 && *(W *)(o + 0xa) != view) return 0;
    cel = *(UB **)(o + 0x1c);
    if (!cel) return 0;
    ox = *(W *)(o + 6); top = *(W *)(o + 8) - *(W *)(o + 0x2e) + 1; loop = *(W *)(o + 0x10);
    for (dy = -tol; dy <= tol; dy++)
        for (dx = -tol; dx <= tol; dx++)
            if (cel_pix(cel, loop, x + dx - ox, y + dy - top)) return 1;
    return 0;
}

static const struct hs *find(W x, W y)
{
    const struct hs *h;
    W room = agi_vars[0];
    if (obj_hit(0, -1, x, y, 0)) return 0;
    for (h = HS; h < HS + sizeof HS / sizeof HS[0]; h++) {
        if (h->room != room) continue;
        if (h->obj == BOX ? x >= h->x1 && x <= h->x2 && y >= h->y1 && y <= h->y2
                          : obj_hit(h->obj, h->view, x, y, 1)) return h;
    }
    return 0;
}

static UB *obj(W n)
{
    UB *o = obj_tab + n * 0x48;
    return o <= obj_last && (*(W *)(o + 0x3e) & 1) ? o : 0;
}

static W dist(UB *o)                      /* as the distance command; 255 = not drawn */
{
    W dx, dy;
    if (!o || !(E(0x3e) & 1)) return 255;
    dx = (E(6) + E(0x2c) / 2) - (*(W *)(o + 6) + *(W *)(o + 0x2c) / 2);
    dy = E(8) - *(W *)(o + 8);
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}

static W at_spot(const struct hs *h)
{
    W x = E(6), y = E(8);
    if (h->st[0] == 0xff) return dist(obj(h->st[1])) < h->st[2];
    return x >= h->st[0] && x <= h->st[2] && y >= h->st[1] && y <= h->st[3];
}

static W aim(const struct hs *h)          /* DIST: just below the object's bottom centre */
{
    UB *o = obj(h->st[1]);
    if (!o) return 0;
    wx = *(W *)(o + 6) + *(W *)(o + 0x2c) / 2 - E(0x2c) / 2;
    wy = *(W *)(o + 8) + 3;
    if (wx < 0) wx = 0;
    if (wy > 167) wy = 167;
    return 1;
}

static W toward(W v, W lo, W hi)          /* nearest value in lo..hi, one pixel inside when it fits */
{
    if (hi - lo >= 2) { lo++; hi--; }
    return v < lo ? lo : v > hi ? hi : v;
}

static void act(const struct hs *h)
{
    walk = 0;
    if (input_on && !is_flag(2)) parse_str((char *)h->cmd);
}

static void stop_ego(void)
{
    E(0x38) = 0; E(0x36) = 0; agi_vars[6] = 0;
}

static void go(void)                      /* the click walk's motion to wx, wy (text 0x6236) */
{
    E(0x38) = 4; E(0x40) = wx; E(0x42) = wy; E(0x44) = E(0x30);
    lastx = E(6); lasty = E(8); still = 0;
}

static W route(void)                      /* search a path from here; wx, wy = its first piece */
{
    return replans++ < 8 && pf_plan(walk->st, walk->st[0] == 0xff ? obj(walk->st[1]) : 0) && pf_next(&wx, &wy);
}

static W next_piece(void)                 /* at the end of a piece: straight on (DIST), the next piece or 0 */
{
    W px = wx, py = wy;
    pf_ego();
    if (walk->st[0] == 0xff && aim(walk) && pf_line(E(6), E(8), wx, wy)) { path = 0; return 1; }
    wx = px; wy = py;
    return pf_next(&wx, &wy) || route();
}

static void start(void)                   /* set off toward walk's spot from where ego stands */
{
    if (walk->st[0] == 0xff) {
        if (!aim(walk)) { act(walk); return; }
    } else {
        wx = toward(E(6), walk->st[0], walk->st[2]);
        wy = toward(E(8), walk->st[1], walk->st[3]);
    }
    pf_ego();
    path = !pf_line(E(6), E(8), wx, wy) && route();
    go();
}

void click_walk(W mx, W my)               /* the click walk (0x6236): mouse x in 640ths, y on screen */
{
    walk_orig(mx, my);
    if (obj_tab && E(0x38) == 4) { creq = 1; ctx = E(0x40); cty = E(0x42); }
}

static void click_start(void)             /* plan from where ego stands; 0 = the straight walk stays */
{
    W r;
    pf_ego();
    r = pf_click(ctx, cty, &cox, &coy);
    if (!r) { cw = 0; return; }
    if (!pf_next(&wx, &wy)) { wx = cox; wy = coy; cw = 0; }  /* already at the path's end: just go on */
    else cw = 1;
    go();
}

static void click_poll(void)
{
    if (agi_vars[0] != wroom || !(E(0x3e) & 1)) { cw = 0; return; }
    if (!player_ctl || !input_on) { held = 1; return; }
    if (!in_play()) return;
    if (held) {
        held = 0;
        if (++resumes > 5) cw = 0; else click_start();
        return;
    }
    if (E(6) == wx && E(8) == wy && (!E(0x38) || (E(0x38) == 4 && E(0x40) == wx && E(0x42) == wy))) {
        if (!pf_next(&wx, &wy)) { wx = cox; wy = coy; cw = 0; }
        go();
        return;
    }
    if (E(0x38) != 4 || E(0x40) != wx || E(0x42) != wy) { cw = 0; return; }  /* the player took over */
    if (E(6) != lastx || E(8) != lasty) { lastx = E(6); lasty = E(8); still = 0; }
    else if (++still >= 10) { cw = 0; stop_ego(); }
}

void act_poll(void)
{
    const struct hs *h;
    if (creq) {
        creq = 0; walk = 0; cw = 0;
        if (!req && in_play() && player_ctl && (E(0x3e) & 1) && E(0x38) == 4) {
            wroom = agi_vars[0]; held = resumes = 0;
            click_start();
        }
    }
    if (req) cw = 0;                      /* a double or right click replaces a click walk */
    if (cw) { click_poll(); return; }
    if (req) {
        req = 0;
        walk = 0;
        if (!obj_tab || !input_on || is_flag(2) || !in_play()) return;
        h = find(rx, ry);
        if (!h) return;
        if (!h->st[2] || at_spot(h) || !player_ctl || !(E(0x3e) & 1)) { act(h); return; }
        walk = h; wroom = agi_vars[0]; replans = 0; held = resumes = 0;
        start();
        return;
    }
    if (!walk) return;
    if (agi_vars[0] != wroom || !(E(0x3e) & 1)) { walk = 0; return; }
    if (!player_ctl || !input_on) { held = 1; return; }  /* the game has ego (scripted walk, talk): wait */
    if (!in_play()) return;               /* a message is up: wait */
    if (at_spot(walk)) { stop_ego(); act(walk); return; }
    if (held) {                           /* the game gave him back: on from wherever he is now */
        held = 0;
        if (++resumes > 5) walk = 0; else start();
        return;
    }
    if (path && E(6) == wx && E(8) == wy && (!E(0x38) || (E(0x38) == 4 && E(0x40) == wx && E(0x42) == wy))) {
        if (next_piece()) go();
        else { stop_ego(); act(walk); }
        return;
    }
    if (!path && walk->st[0] == 0xff && E(0x38) == 4 && E(0x40) == wx && E(0x42) == wy && aim(walk)) {
        E(0x40) = wx; E(0x42) = wy;       /* follow a moving object */
    }
    if (E(0x38) != 4 || E(0x40) != wx || E(0x42) != wy) {
        if (E(0x38) == 0 && E(6) == wx && E(8) == wy) act(walk);
        else walk = 0;                    /* the player took over */
        return;
    }
    if (E(6) != lastx || E(8) != lasty) { lastx = E(6); lasty = E(8); still = 0; }
    else if (++still >= 10) { stop_ego(); act(walk); }
}
