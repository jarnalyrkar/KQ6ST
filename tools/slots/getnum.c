/* Clickable number menus (user request 2026-10-20). KQ6's ferryman (logic 12) and, since this change, the
   pawn shop (logic 8) show a menu as a blank print.at box with display()ed lines "1. ...", "2. ..." and then
   wait in get.num (command 118, text 0x1720). The box is a flag-15 window ($f46c = 1), and in such a
   window the interpreter does not draw the pointer (0x6408); a left click arrives in the line input as
   Return (event 0x201, mapped to 13 by 0x26ce), so it just entered an empty answer.
   Now display() (command 103, text 0x9144, through the command table) remembers, per text row, a line
   that starts with a number and a dot. While get.num reads its answer (its call of the line input 0xc22c
   at 0x1784 comes here) the pointer is shown, and the line input's key fetch (call of 0x265a at 0xc29e)
   turns a left click on such a line into the keys for that number and Return (after four backspaces for
   anything typed). Typing still works; other clicks are ignored. The remembered lines are forgotten when
   get.num returns. get.string and the input line use the same line input but are not affected. */
typedef short W;
typedef unsigned char UB;

struct ev { W type, code; };
extern long disp_orig(UB *p);            /* 0x9144: display(row, col, msg) */
extern long msg_text(W n);               /* 0x90ca: message n of the current logic */
extern W line_in(char *buf, W len);      /* 0xc22c */
extern W get_key(void);                  /* 0x265a */
extern void tick(void);
extern long ev_poll_(void);
extern void ev_take(struct ev *e);       /* 0x26ce: mouse buttons -> 13 / 27 */
extern void mouse_show(void), mouse_hide(void), mouse_poll(void);
extern W f46c;                           /* flag-15 window: no pointer */
extern unsigned char ptr_flags;          /* bit 6: pointer not hidden by the logic */
extern volatile W mouse_x2, mouse_y2;

static struct { UB num, col, len; } line[25];
static W active, queued, qpos;
static char keys[8];

/* -mshort: s[k] with a short k is computed as a 16-bit address, so the text is walked with s++ */
long disp_hook(UB *p)
{
    const UB *s = (const UB *)msg_text(p[2]);
    W n = 0, len = 0;
    if (p[0] < 25 && s) {
        while (*s == ' ') s++, len++;
        while (*s >= '0' && *s <= '9' && n < 100) n = n * 10 + *s++ - '0', len++;
        if (n && *s == '.') {
            while (*s) s++, len++;      /* not while (*s++): -mshort indexes that with 0x0000ffff */
            line[p[0]].num = n; line[p[0]].col = p[1]; line[p[0]].len = len;
        } else line[p[0]].num = 0;
    }
    return disp_orig(p);
}

static W hit(void)
{
    W row = mouse_y2 >> 3, col = mouse_x2 >> 4;
    if (row < 0 || row >= 25 || !line[row].num) return 0;
    return col >= line[row].col - 1 && col <= line[row].col + line[row].len ? line[row].num : 0;
}

W getnum_line(char *buf, W len)
{
    W r, saved = f46c, i, shown = ptr_flags & 0x40;
    active = 1; queued = qpos = 0;
    if (shown) { f46c = 0; mouse_show(); }
    r = line_in(buf, len);
    if (shown) { mouse_hide(); f46c = saved; }
    active = 0;
    for (i = 0; i < 25; i++) line[i].num = 0;
    return r;
}

W line_key(void)
{
    struct ev *e;
    W n;
    if (!active) return get_key();
    for (;;) {
        if (qpos < queued) return keys[qpos++];
        mouse_poll();                     /* 0x644a: redraw the pointer where the mouse is */
        tick();
        e = (struct ev *)ev_poll_();
        if (!e) continue;
        if (e->type == 1 && e->code == 0x201) {
            n = hit();
            if (n) {
                queued = qpos = 0;
                while (queued < 4) keys[queued++] = 8;
                if (n >= 10) keys[queued++] = '0' + n / 10;
                keys[queued++] = '0' + n % 10;
                keys[queued++] = 13;
            }
            continue;
        }
        ev_take(e);
        if (e->type == 1 && e->code) return e->code;
    }
}
