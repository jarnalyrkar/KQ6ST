/* Pointer in the inventory picker (user request 2026-10-20). With flag 13 set, status() (command 124, text
   0xba12) lets the player pick an item: the pawn shop's "trade" (logic 8), giving items, etc. The
   interpreter has a mouse mode for it (0xbb70), but it only runs when flag 18 is set (0x678c), which KQ6
   never does, and in that mode the arrow keys do nothing and an item is deselected whenever the pointer
   is off the names. So the keyboard loop stays (wait for an event at 0xbcb8, Return = pick, Esc = none,
   directions move the highlight with 0xbe82) and its event wait comes here instead (inv_stub, which passes
   the entry list and the highlighted entry and takes the new one back): the pointer is shown, moving the
   mouse onto a name highlights it (0xbf00, as the keys do) and a left click on a name picks it (returned
   as Return). Keys work as before. The pointer is hidden again when the picker ends (Return / Esc). The
   call of 0x678c at 0xba66 (start of the picker) marks the start: a pointer resting on a name does not
   move the keyboard's preselection until the mouse moves. No mouse ($f22e != 0): the original wait. */
/* gcc 15 -mshort: ivopts ICEs on the entry loop (as in pointer.c) */
#pragma GCC optimize("no-ivopts", "no-tree-scev-cprop")
typedef short W;
typedef unsigned char UB;

struct ev { W type, code; };
struct ent { W item; const UB *name; W row, col; };   /* 10 bytes, as built at 0xba8e */
extern long ev_wait_(void);
extern long ev_poll_(void);
extern void tick(void), mouse_show(void), mouse_hide(void), mouse_poll(void);
extern long inv_mark(struct ent *old, struct ent *new);  /* 0xbf00 */
extern void inv_start_orig(void);                       /* 0x678c */
extern W str_len(const UB *s);                          /* 0x205e */
extern W no_mouse_lim;
extern volatile W mouse_x2, mouse_y2;

struct ent *inv_cur;
static W fresh, shown, lastx, lasty;
static struct ev enter = { 1, 13 };

void inv_start(void)
{
    inv_start_orig();
    fresh = 1;
}

static struct ent *under(struct ent *e, struct ent *last)
{
    W row = mouse_y2 >> 3, col = mouse_x2 >> 4;
    for (; e <= last; e++)
        if (e->row == row && col >= e->col && col < e->col + str_len(e->name)) return e;
    return 0;
}

static long done(long e)
{
    if (shown) { mouse_hide(); shown = 0; }
    return e;
}

long inv_wait(struct ent *list, struct ent *last, struct ent *cur)
{
    struct ev *e;
    struct ent *h;
    inv_cur = cur;
    if (no_mouse_lim) return ev_wait_();
    if (fresh) { fresh = 0; lastx = mouse_x2; lasty = mouse_y2; }
    if (!shown) { mouse_show(); shown = 1; }
    for (;;) {
        mouse_poll();
        tick();
        if (mouse_x2 != lastx || mouse_y2 != lasty) {
            lastx = mouse_x2; lasty = mouse_y2;
            h = under(list, last);
            if (h && h != inv_cur) {
                mouse_hide();
                inv_cur = (struct ent *)inv_mark(inv_cur, h);
                mouse_show();
            }
        }
        e = (struct ev *)ev_poll_();
        if (!e) continue;
        if (e->type == 1 && e->code == 0x201) {          /* left click: pick the name under the pointer */
            h = under(list, last);
            if (!h) continue;
            if (h != inv_cur) { mouse_hide(); inv_cur = (struct ent *)inv_mark(inv_cur, h); }
            return done((long)&enter);
        }
        if (e->type == 1 && (e->code == 13 || e->code == 27)) return done((long)e);
        return (long)e;
    }
}

/* the keyboard loop's own registers: a5 = highlighted entry, -0x1f8(a6) = list, -0x200(a6) = last entry */
__asm__(".globl inv_stub\n"
        "inv_stub:\n"
        "  move.l %a5,-(%sp)\n"
        "  move.l -0x200(%a6),-(%sp)\n"
        "  pea -0x1f8(%a6)\n"
        "  jsr inv_wait\n"
        "  lea 12(%sp),%sp\n"
        "  move.l inv_cur,%a5\n"
        "  rts\n");
