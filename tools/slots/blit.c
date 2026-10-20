/* Sprites no longer paint over an open message box (Sierra ST AGI 1.422, colour driver; M9). A print
   window with flag 15 set (or print.at) stays open while the game keeps cycling, and the sprite update
   (text 0xe224) copies each object's rectangle from the picture buffer (back_buf, screen format) straight
   to the screen with 0xdf40, over the box: the sleeping oyster and the gnomes on the Isle of Wonder, the
   flames at the Lord of the Dead, the minotaur room. The call at 0xe2a4 now comes here: while a window is
   open (text_mode), the copy leaves out the box's pixels; partly covered 16-pixel groups are merged per
   plane word with a mask. The picture buffer still gets the sprites, so closing the window (0xf4a, which
   copies the box area back with 0xdf40 directly) shows them as they are by then.
   Box (set by open_win 0x8b04, low-res screen_mode 1): win_pos = x << 8 | bottom row, win_size =
   height << 8 | width, in game pixels (160 wide) and play-area rows, border included.
   0xdf40 takes registers: d0 bottom row, d2 rows, d3 x (multiple of 8 = byte offset in the row),
   d4 width in bytes (multiple of 8); it copies upwards. */
#pragma GCC optimize("no-ivopts", "no-tree-scev-cprop")
typedef short W;
typedef unsigned short UW;

extern W text_mode, screen_mode, play_top;
extern UW win_pos, win_size;
extern W line_off[];
extern unsigned char *screen, *back_buf;

asm(".section .text.sprite_blit,\"ax\"\n"
    "  .globl sprite_blit\n"
    "sprite_blit:\n"
    "  tst.w text_mode\n"
    "  beq.s 1f\n"
    "  cmp.w #1, screen_mode\n"
    "  bne.s 1f\n"
    "  movem.l %d0-%d7/%a0-%a6, -(%sp)\n"
    "  move.w %d4, -(%sp)\n"
    "  move.w %d3, -(%sp)\n"
    "  move.w %d2, -(%sp)\n"
    "  move.w %d0, -(%sp)\n"
    "  jsr blit_clip\n"
    "  addq.l #8, %sp\n"
    "  movem.l (%sp)+, %d0-%d7/%a0-%a6\n"
    "  rts\n"
    "1: jmp blit_orig\n");

void blit_clip(W y, W rows, W x, W wbytes)
{
    W bx0 = (win_pos >> 8) * 2, bx1 = bx0 + (win_size & 0xff) * 2 - 1;     /* box, screen pixels */
    W by1 = win_pos & 0xff, by0 = by1 - (win_size >> 8) + 1;               /* box, play rows */
    W r, k, lo, hi, gx;
    UW m, *s, *d;
    for (r = y - rows + 1; r <= y; r++) {
        s = (UW *)((long)back_buf + (UW)line_off[r] + x);
        d = (UW *)((long)screen + (UW)line_off[r + play_top] + x);
        for (k = 0, gx = x * 2; k < wbytes; k += 8, gx += 16, s += 4, d += 4) {
            m = 0;
            if (r >= by0 && r <= by1) {
                lo = bx0 > gx ? bx0 - gx : 0;
                hi = bx1 < gx + 15 ? bx1 - gx : 15;
                if (lo <= hi) m = (UW)(0xffffu >> lo) & (UW)(0xffffu << (15 - hi));
            }
            if (m == 0) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3]; }
            else if (m != 0xffff) {
                d[0] = (s[0] & ~m) | (d[0] & m); d[1] = (s[1] & ~m) | (d[1] & m);
                d[2] = (s[2] & ~m) | (d[2] & m); d[3] = (s[3] & ~m) | (d[3] & m);
            }
        }
    }
}
