/* Replacement for the save/restore slot list of Sierra ST AGI 1.422 (text 0x3118 in SIERRA.PRG).
   The original reads 12 slots into a 12-entry array on its stack and sizes the window to fit them.
   This version reads up to MAXSLOT slots and shows at most VIS rows at a time, scrolling the list:
   up/down move the highlight (wrapping at both ends), in the mouse loop too; with the mouse, the
   pointer selects the row under it, and holding it just above or below the list scrolls.
   Everything else follows the original: newest save preselected, the auto-name (f85c) shortcut,
   Return/left button picks, Esc/right button cancels.
   Built with gcc -mshort (16-bit int, like the interpreter's Alcyon C); the interpreter's functions
   and variables are linker symbols (slots.ld.in). The interpreter returns every value in d0, also
   pointers, so functions returning pointers are declared as returning long. */
typedef short W;
struct ent { W num; char desc[32]; };          /* 34 bytes, as the original */
struct ev { W type; W code; };

extern W read_slot(W n, struct ent *e, long *time);   /* 1 if sg.n exists; fills e */
extern void print_entry(char *desc, W fg, W bg);      /* "[ desc padded ]" */
extern void gotorc(W row, W col);
extern void open_win(char *msg, W height, W width, W flag);
extern void close_win(void);
extern void str_cpy(char *dst, char *src);
extern W str_cmp(char *a, char *b);
extern void s_printf(char *buf, char *fmt, ...);
extern void msgbox(char *text);
extern void before_msg(void);
extern void mouse_hide(void), mouse_show(void), mouse_poll(void), tick(void);
extern long ev_poll_(void), ev_wait_(void);           /* return pointers in d0 (gcc expects a0) */
#define ev_poll() ((struct ev *)ev_poll_())
#define ev_wait() ((struct ev *)ev_wait_())
extern void ev_take(struct ev *e);
extern char get_drv(void);

extern W mouse_on, win_top, win_left, auto_flag, screen_mode;
extern volatile W mouse_x, mouse_y;
extern W mouse_x2, mouse_y2;
extern volatile long ticks;
extern char auto_name[], last_desc[], save_dir[], str_1402e[], empty_str[], drv_save;
extern char m_nogames[], m_entry[], m_eeee[], m_save[], m_restore[];

#define MAXSLOT 99
#define VIS 12
#define SCROLL_TICKS 3                                /* ~20 ticks/s */

static struct ent e[MAXSLOT];
static char buf[160];
static W top, sel, cnt, vis, r0, col;

static void line(W i)
{
    gotorc(r0 + i - top, col);
    if (i == sel) print_entry(e[i].desc, 15, 0); else print_entry(e[i].desc, 0, 15);
}

static void redraw(void)
{
    W i;
    for (i = top; i < top + vis; i++) line(i);
}

static void select_row(W n)                            /* n may be -1 or cnt: wraps */
{
    W old = sel;
    if (n < 0) n = cnt - 1;
    if (n >= cnt) n = 0;
    sel = n;
    if (sel < top) { top = sel; redraw(); }
    else if (sel >= top + vis) { top = sel - vis + 1; redraw(); }
    else { if (old >= top && old < top + vis) line(old); line(sel); }
}

static W pick(W mode)                                  /* Return on the highlighted entry */
{
    if (!auto_name[0]) str_cpy(last_desc, e[sel].desc);
    else if ((char)mode == 'r') str_cpy(auto_name, e[sel].desc);
    return e[sel].num;
}

W select_slot(W mode)
{
    long newest = 0, t;
    W i, s = (char)mode == 's';
    struct ev *ev;
    struct ent *p;

    sel = 0;
    if (mouse_on) mouse_hide();
    cnt = 0;
    for (i = 0, p = e; i < MAXSLOT; i++) {
        if (read_slot(i + 1, p, &t)) {
            if (t > newest) { newest = t; sel = cnt; }
            cnt++; p++;
        } else if (s) { cnt++; p++; }                  /* save: every slot, empty ones too */
    }
    if (cnt == 0 && !auto_name[0]) {
        before_msg();
        s_printf(buf, m_nogames, save_dir, str_1402e);
        msgbox(buf);
        return 0;
    }
    if (auto_name[0] && !auto_flag) {
        str_cpy(last_desc, auto_name);
        for (i = 0; i < cnt; i++) if (!str_cmp(auto_name, e[i].desc)) return e[i].num;
        if (s) for (i = 0; i < cnt; i++) if (!str_cmp(empty_str, e[i].desc)) return e[i].num;
        if ((char)mode == 'r') return 0;
    }
    if (auto_name[0] && auto_flag) drv_save = get_drv();

    vis = cnt < VIS ? cnt : VIS;
    open_win(auto_name[0] ? (auto_flag ? m_entry : m_eeee) : (s ? m_save : m_restore), vis + 3, 0x22, 1);
    r0 = 3 + win_top; col = win_left;
    auto_flag = 0;
    top = sel >= vis ? sel - vis + 1 : 0;
    redraw();

    if (mouse_on) {
        W mr, mc, lastr = -1, lastc = -1, idx, old;
        long lastt = ticks, lasts = ticks;
        mouse_y = mouse_y2 = (r0 + sel - top) * 8 + 4;
        mouse_x = mouse_x2 = screen_mode == 2 ? (win_left + 20) * 8 + 4 : win_left * 16 + 8;
        mouse_show();
        for (;;) {
            ev = ev_poll();
            if (ev && ev->type == 1) {
                ev_take(ev);
                if (ev->code == 0x0d || ev->code == 0x1b) {
                    mouse_hide();
                    close_win();
                    if (ev->code == 0x0d && sel != -1) return pick(mode);
                    return 0;
                }
            } else if (ev && ev->type == 2 && (ev->code == 1 || ev->code == 5)) {
                mouse_hide();
                select_row(sel < 0 ? top : sel + (ev->code == 1 ? -1 : 1));
                mouse_show();
            }
            if (lastt != ticks) { tick(); lastt = ticks; }
            mouse_poll();
            mr = mouse_y >> 3;
            mc = screen_mode == 2 ? (mouse_x >> 3) - 20 : mouse_x >> 4;
            if (mc >= col && mc <= col + 0x21 && (mr < r0 || mr >= r0 + vis)) {
                /* above or below the list: scroll while it is held there */
                if (ticks - lasts >= SCROLL_TICKS) {
                    lasts = ticks;
                    if (mr < r0 && top > 0) { mouse_hide(); top--; sel = top; redraw(); mouse_show(); }
                    else if (mr >= r0 + vis && top + vis < cnt) {
                        mouse_hide(); top++; sel = top + vis - 1; redraw(); mouse_show();
                    }
                }
                lastr = mr; lastc = mc;
                continue;
            }
            if (mr == lastr && mc == lastc) continue;      /* only follow the mouse when it moves */
            lastr = mr; lastc = mc;
            if (mc < col || mc > col + 0x21) {
                if (sel != -1) { mouse_hide(); old = sel; sel = -1; if (old >= top && old < top + vis) line(old); mouse_show(); }
            } else if ((idx = top + mr - r0) != sel) {
                mouse_hide();
                old = sel; sel = idx;
                if (old >= top && old < top + vis) line(old);
                line(sel);
                mouse_show();
            }
        }
    }

    for (;;) {
        gotorc(r0 + sel - top, col);
        ev = ev_wait();
        if (ev->type == 1) {
            if (ev->code == 0x0d) { i = pick(mode); close_win(); return i; }
            if (ev->code == 0x1b) { close_win(); return 0; }
        } else if (ev->type == 2) {
            if (ev->code == 1) select_row(sel - 1);
            else if (ev->code == 5) select_row(sel + 1);
        }
    }
}
