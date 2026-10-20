/* Pathfinding for click to interact (act.c): when the straight walk to a hotspot's stand-here spot is
   blocked, a breadth-first search over the positions ego may stand on finds a way around.
   Where ego may stand is the interpreter's own position check (text 0x1076, run on every step): along his
   baseline (row y, x to x + width - 1) the priority screen (*0x13f08, 160 bytes a row, priority in the high
   nibble) has no 0 (barrier) and no 1 (conditional barrier, unless he ignores blocks); all water (3) or not,
   as object.on.water / object.on.land ask (object flags 0x100 / 0x800); no pixel test at all with a fixed
   priority of 15. Also y must be below the horizon (0xfa4e, unless ignore.horizon, flag 8), he stays on his
   side of a block (0xfa50, text 0x93a, unless ignore.blocks, flag 2), and the screen edges are left out with a
   pixel to spare, as touching them changes the room.
   The search runs backwards from the stand-here spot until it reaches ego, so every searched position knows
   its next step toward the spot (pf_grid: 1-8 = the step OFF[n - 1] backwards, GOAL, BLK = not walkable,
   0 = not searched). Ego then walks the path in pieces, each to the farthest point along it that the
   interpreter's move.obj reaches through walkable positions only (text 0x61b6: a step of the object's step
   size on each axis whose distance is at least that, so diagonal until one axis is lined up, then straight).
   Other objects are not obstacles here; if one is in the way, act.c's "no movement" check gives up. */
/* gcc 15 -mshort: internal compiler error in pf_plan with ivopts; the BLK fill loop becomes a memset call */
#pragma GCC optimize ("no-ivopts", "no-tree-loop-distribute-patterns")
typedef short W;
typedef unsigned char UB;

#define GW 160
#define GH 168
#define QN 4096                           /* ring queue; the search front is far smaller */
#define BLK 0x80
#define GOAL 9

extern UB pf_grid[];                      /* GW * GH bytes and the queue: past the interpreter's BSS */
extern W pf_queue[];
extern UB *prio_buf, *obj_tab;
extern W horizon, blk_on, blk_x1, blk_y1, blk_x2, blk_y2;

static const W OFF[8] = { -GW - 1, -GW, -GW + 1, -1, 1, GW - 1, GW, GW + 1 };
static W ew, ef, any, ymin, blk, bin, step, head, tail;

#define E(off) (*(W *)(obj_tab + (off)))  /* ego = object 0 */

static W inblock(W x, W y) { return x > blk_x1 && x < blk_x2 && y > blk_y1 && y < blk_y2; }

void pf_ego(void)                         /* ego's walking rules, before pf_line / pf_plan */
{
    ew = E(0x2c); ef = E(0x3e); step = E(0x30);
    if (step < 1) step = 1;
    any = (ef & 4) && E(0x3c) == 15;
    ymin = ef & 8 ? 1 : horizon + 1;
    if (ymin < E(0x2e) - 1) ymin = E(0x2e) - 1;
    if (ymin < 1) ymin = 1;
    blk = blk_on && !(ef & 2);
    bin = blk && inblock(E(6), E(8));
}

static W edge_ok(W x, W y)
{
    return x > 0 && x + ew < GW && y >= ymin && y < GH - 1 && (!blk || inblock(x, y) == bin);
}

static W water_ok(W wet) { return wet == ew ? !(ef & 0x800) : !(ef & 0x100); }

static W cell_ok(W x, W y)
{
    const UB *p;
    W i, wet = 0;
    UB n;
    if (!edge_ok(x, y)) return 0;
    if (any) return 1;
    p = prio_buf + y * GW + x;
    for (i = 0; i < ew; i++) {
        n = p[i] & 0xf0;
        if (n == 0 || (n == 0x10 && !(ef & 2))) return 0;
        if (n == 0x30) wet++;
    }
    return water_ok(wet);
}

static void pf_map(void)                  /* cell_ok for every position, a sliding window along each row */
{
    W x, y, bad, wet, i;
    UB *g = pf_grid, n;
    const UB *p;
#define COUNT(v, d) n = (v) & 0xf0; \
    if (n == 0 || (n == 0x10 && !(ef & 2))) bad += d; else if (n == 0x30) wet += d
    for (y = 0; y < GH; y++, g += GW) {
        p = prio_buf + y * GW;
        bad = wet = 0;
        for (i = 0; i < ew - 1; i++) { COUNT(p[i], 1); }
        for (x = 0; x + ew <= GW; x++) {
            COUNT(p[x + ew - 1], 1);
            g[x] = edge_ok(x, y) && (any || (!bad && water_ok(wet))) ? 0 : BLK;
            COUNT(p[x], -1);
        }
        for (; x < GW; x++) g[x] = BLK;
    }
#undef COUNT
}

static W walk_ok(W x, W y, W tx, W ty, W map)  /* move.obj from x, y to tx, ty stays on walkable positions */
{
    W dx, dy;
    for (;;) {
        dx = tx - x; dy = ty - y;
        dx = dx <= -step ? -step : dx >= step ? step : 0;
        dy = dy <= -step ? -step : dy >= step ? step : 0;
        if (!dx && !dy) return 1;
        x += dx; y += dy;
        if (!map) { if (!cell_ok(x, y)) return 0; }
        else if (x < 0 || x >= GW || y < 0 || y >= GH || (pf_grid[y * GW + x] & BLK)) return 0;
    }
}

W pf_line(W x, W y, W tx, W ty) { return walk_ok(x, y, tx, ty, 0); }

static W push(W c)
{
    pf_queue[tail] = c;
    tail = (tail + 1) & (QN - 1);
    return tail != head;
}

static W goal(W c)
{
    if (pf_grid[c]) return 1;             /* not walkable or already a goal */
    pf_grid[c] = GOAL;
    return push(c);
}

/* Search from the stand-here spot to ego: st = the hotspot's NEAR box, or DIST with o = its object
   (st[2] = the distance to get under). 1 = ego can get there. */
W pf_plan(const UB *st, const UB *o)
{
    W x, y, x1, y1, x2, y2, c, n, d, start, ox, oy, ow, dy;
    if (E(6) < 0 || E(6) >= GW || E(8) < 0 || E(8) >= GH) return 0;
    pf_map();
    start = E(8) * GW + E(6);
    pf_grid[start] = 0;
    head = tail = 0;
    if (st[0] == 0xff) {                  /* every position closer than d, as act.c's dist() */
        if (!o) return 0;
        ox = *(W *)(o + 6); oy = *(W *)(o + 8); ow = *(W *)(o + 0x2c);
        d = st[2];
        for (y = oy - d + 1; y < oy + d; y++) {
            if (y < 1 || y >= GH - 1) continue;
            dy = y < oy ? oy - y : y - oy;
            for (x = ox + ow / 2 - ew / 2 - (d - dy) + 1; x < ox + ow / 2 - ew / 2 + (d - dy); x++) {
                if (x < 1 || x >= GW - 1) continue;
                if (y == oy && x < ox + ow && x + ew > ox) continue;  /* would stand in the object */
                if (!goal(y * GW + x)) return 0;
            }
        }
    } else {                              /* the box's rim: every way in crosses it */
        x1 = st[0] < 1 ? 1 : st[0]; x2 = st[2] > GW - 2 ? GW - 2 : st[2];
        y1 = st[1] < 1 ? 1 : st[1]; y2 = st[3] > GH - 2 ? GH - 2 : st[3];
        for (y = y1; y <= y2; y++)
            for (x = x1; x <= x2; x += (y == y1 || y == y2 || x == x2) ? 1 : x2 - x1)
                if (!goal(y * GW + x)) return 0;
    }
    if (pf_grid[start] == GOAL) return 1;
    while (head != tail) {
        c = pf_queue[head];
        head = (head + 1) & (QN - 1);
        for (d = 0; d < 8; d++) {
            n = c + OFF[d];
            if (pf_grid[n]) continue;
            pf_grid[n] = d + 1;
            if (n == start) return 1;
            if (!push(n)) return 0;
        }
    }
    return 0;
}

/* A plain click (act.c, user request 2026-10-20): ego is to walk to tx, ty (the click walk's target), or out
   of the room when the click is past an edge. 0 = leave the interpreter's straight walk alone (it gets
   there, or there is no better way); 1 = a path is in the grid for pf_next, and *ox, *oy is where to go from
   its end: the end itself, or a point past the edge (touching the edge is what changes the room).
   The search goes forward from ego (pf_grid: the step that reached a position, START for ego's) until it
   finds a goal: the target, or for an exit the walkable positions along that edge near the click. If no goal
   is reachable, he goes to the reachable position nearest the click (a click on a house walks to its front).
   The path is then turned around into next steps, as pf_plan leaves them. */
#define START 10
#define NEAR 16                           /* an exit: goal positions within this of the click along the edge */
#define EDGE 4                            /* clicks this close to the left, right or bottom edge mean "out" */

static W ingoal(W x, W y, W x1, W y1, W x2, W y2) { return x >= x1 && x <= x2 && y >= y1 && y <= y2; }

W pf_click(W tx, W ty, W *ox, W *oy)
{
    W x1, y1, x2, y2, px, py, ex, ey, out = 1, c, n, d, start, end, x, y, p, q = 0;
    long bd, dd;
    if (E(6) < 0 || E(6) >= GW || E(8) < 0 || E(8) >= GH) return 0;
    px = tx < 1 ? 1 : tx > GW - 1 - ew ? GW - 1 - ew : tx;
    py = ty < ymin ? ymin : ty > GH - 2 ? GH - 2 : ty;
    if (ty < ymin && !(ef & 8)) {         /* above the horizon: out at the top (touching it is edge 1) */
        y1 = y2 = ymin; x1 = px - NEAR; x2 = px + NEAR; ex = px; ey = ymin - 1;
    } else if (ty >= GH - 1 - EDGE) {     /* out at the bottom: past row 167 */
        y1 = y2 = GH - 2; x1 = px - NEAR; x2 = px + NEAR; ex = px; ey = GH;
    } else if (tx + ew / 2 <= EDGE) {     /* out to the left: x below 0 */
        x1 = x2 = 1; y1 = py - NEAR; y2 = py + NEAR; ex = -1; ey = py;
    } else if (tx + ew / 2 >= GW - 1 - EDGE) {  /* out to the right: x + width past 160 */
        x1 = x2 = GW - 1 - ew; y1 = py - NEAR; y2 = py + NEAR; ex = GW - ew + 1; ey = py;
    } else {
        out = 0; x1 = x2 = ex = px; y1 = y2 = ey = py;
    }
    if (!out && pf_line(E(6), E(8), tx, ty)) return 0;
    pf_map();
    start = E(8) * GW + E(6);
    pf_grid[start] = START;
    head = tail = 0;
    push(start);
    end = -1; bd = 0x7fffffffL;
    if (ingoal(E(6), E(8), x1, y1, x2, y2)) end = start;
    while (end < 0 && head != tail) {
        c = pf_queue[head];
        head = (head + 1) & (QN - 1);
        x = c % GW; y = c / GW;
        dd = (long)(x - px) * (x - px) + (long)(y - py) * (y - py);
        if (dd < bd) { bd = dd; q = c; }
        for (d = 0; d < 8; d++) {
            n = c + OFF[d];
            if (pf_grid[n]) continue;
            pf_grid[n] = d + 1;
            if (ingoal(n % GW, n / GW, x1, y1, x2, y2)) { end = n; break; }
            if (!push(n)) return 0;
        }
    }
    if (end < 0) {                        /* no goal: as near the click as he can get; out if that is on the edge */
        if (bd == 0x7fffffffL || q == start) return 0;
        end = q;
        if (out && (x1 == x2 ? q % GW == x1 : q / GW == y1)) out = 2;
        else { out = 0; ex = q % GW; ey = q / GW; }
    }
    if (!out) { ex = end % GW; ey = end / GW; }
    else if (x1 == x2) ey = end / GW;     /* left / right: out at the row he got to */
    else ex = end % GW;                   /* top / bottom: at the column */
    /* turn the path around: each position on it gets the step to the next (9 - d: OFF is symmetric) */
    c = end; p = pf_grid[c]; pf_grid[c] = GOAL;
    while (c != start) {
        n = c - OFF[p - 1];
        q = pf_grid[n];
        pf_grid[n] = 9 - p;
        c = n; p = q;
    }
    *ox = ex; *oy = ey;
    return 1;
}

/* The next piece of the path from where ego stands: the farthest point along it (up to 120 steps) that
   move.obj reaches from here. 0 = he is off the searched path or at its end. */
W pf_next(W *tx, W *ty)
{
    W x = E(6), y = E(8), c, n, best = -1;
    UB g;
    if (x < 0 || x >= GW || y < 0 || y >= GH) return 0;
    c = y * GW + x;
    for (n = 0; n < 120; n++) {
        g = pf_grid[c];
        if (g < 1 || g >= GOAL) break;
        c -= OFF[g - 1];
        if (walk_ok(x, y, c % GW, c / GW, 1)) best = c;
    }
    if (best < 0) return 0;
    *tx = best % GW; *ty = best / GW;
    return 1;
}
