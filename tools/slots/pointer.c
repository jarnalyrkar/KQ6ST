/* Mouse pointer that does not erase text (Sierra ST AGI 1.422, colour driver; M1). The interpreter
   erases the pointer by copying the area back from its off-screen picture buffer (text 0xe4c2), unless
   a save-under was made first (0xe48e, only in text mode and menus). Text drawn over the picture
   (display, the title buttons, the spell book) goes to the screen through the VT52 console only, so
   moving the pointer over it wiped the letters. Now the pointer always saves what is under it, and
   remembers what it drew: on erase, a byte that still holds the drawn pointer gets the saved byte back,
   a byte that changed since (a sprite blit, text, the dissolve) is left alone, as it is new content.
   A redraw over a still-visible pointer (the sprite loop does that after every blit) takes the changed
   bytes into the saved copy first. Replaces the colour vectors save (0x41d4), hide (0x41da) and show
   (0x41e0); the drawing itself is still 0xe510. The mono driver (0xddb4.., hi-res) is unchanged. */
/* gcc 15 -mshort: ivopts ICEs on these loops, and scev-cprop computes a pointer after a loop in 16 bits */
#pragma GCC optimize("no-ivopts", "no-tree-scev-cprop")
typedef short W;
typedef unsigned short UW;
typedef unsigned char B;

extern void ptr_draw(void);                 /* 0xe510: draw at mouse_x/mouse_y, sets bit 0 */
extern volatile B ptr_flags;                /* bit 0 pointer drawn, bit 1 save-under valid */
extern W mouse_x, mouse_y, mouse_xmax, mouse_ymax;
extern B *screen;
extern W line_off[];
extern B saved[256];                        /* the interpreter's save-under buffer ($139d2) */

#define LINE 160L
static B drawn[256];
static unsigned long pa;                    /* screen address of the drawn pointer */
static W bpr, rows;                         /* bytes per row (8 or 16), rows (1..16) */
/* -mshort: pointer + short offset is done in 16 bits, so rows are stepped with long integers */
#define ROW(y) ((long *)(pa + (y) * LINE))

/* the same area as 0xe572 */
static void area(void)
{
    UW x = (UW)mouse_x >> 1, r;
    pa = (unsigned long)screen + (UW)line_off[mouse_y] + ((x & 0xfff0) >> 1);
    bpr = ((x & 15) > 4 && (UW)((x & 0xfff0) + 15) < ((UW)mouse_xmax >> 1)) ? 16 : 8;
    r = mouse_ymax - mouse_y;
    rows = (r < 15 ? r : 15) + 1;
}

static void save(void)
{
    long *s, *d = (long *)saved;
    W y, i;
    area();
    for (y = 0; y < rows; y++)
        for (i = bpr / 4, s = ROW(y); i; i--) *d++ = *s++;
}

void ptr_save(void)
{
    if (ptr_flags & 1) return;              /* would save the pointer itself */
    save();
    ptr_flags |= 2;
}

void ptr_show(void)
{
    long *s, *k, *d;
    W y, i, j;
    if (!(ptr_flags & 1)) save();
    else {                                  /* redraw: keep what was drawn over the pointer */
        k = (long *)saved; d = (long *)drawn;
        for (y = 0; y < rows; y++)
            for (i = bpr / 4, s = ROW(y); i; i--, s++, k++, d++)
                if (*s != *d)
                    for (j = 0; j < 4; j++)
                        if (((B *)s)[j] != ((B *)d)[j]) ((B *)k)[j] = ((B *)s)[j];
    }
    ptr_draw();
    area();
    for (y = 0, d = (long *)drawn; y < rows; y++)
        for (i = bpr / 4, s = ROW(y); i; i--) *d++ = *s++;
    ptr_flags |= 2;
}

void ptr_hide(void)
{
    long *s, *k, *d;
    W y, i, j;
    if (!(ptr_flags & 1)) return;
    k = (long *)saved; d = (long *)drawn;
    for (y = 0; y < rows; y++)
        for (i = bpr / 4, s = ROW(y); i; i--, s++, k++, d++)
            if (*s == *d) *s = *k;
            else
                for (j = 0; j < 4; j++)
                    if (((B *)s)[j] == ((B *)d)[j]) ((B *)s)[j] = ((B *)k)[j];
    ptr_flags &= ~3;
}
