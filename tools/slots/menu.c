/* Open the menu with the mouse pointer (Sierra ST AGI 1.422; M2). Other Sierra ST games open the menu
   bar when the pointer reaches the bottom of the status line. This interpreter only opens it through
   menu.input (Esc, controller 19 in KQ6) or its right-button path, which KQ6's logic maps elsewhere.
   During play the pointer is clamped to the top of the play area (y >= play_top), so "reaching the
   status line" means sitting on that limit. Hooked in front of the per-cycle event poll (text 0xca86,
   called from 0x4c34): when the pointer gets there, set menu_mouse = 2 (the right-button marker, which
   makes the poll run the hover-driven mouse menu at 0x54d0) and call menu.input, which honours flag 14
   (menu allowed). KQ6's logic only calls menu.input while flag 33 is clear (title/intro block it), so
   that is checked too. The pointer has to leave the top line before it opens the menu again, so it
   does not reopen right after an item is picked. Same conditions as the right-button path otherwise.
   Game cursor (2026-10-20): while the logic has hidden the pointer (hide.mouse, command 174, clears bit 6
   of ptr_flags) and named two vars with get.mse.posn (command 177; var 0 = never named), the pointer's
   play-area position goes into those vars every cycle, as a click would store it (x/4, y - play_top).
   KQ6's arrow / sword cursors (logics 111, 210-212) follow it. No menu from the pointer meanwhile.
   Since 2026-10-20 the position is written while the pointer is shown too (title choice hover, logic 255;
   KQ6's logic 0 names v240/v241 every cycle, flag 19 still says "clicked this cycle").
   After the poll, act_poll (act.c) handles a pending click-to-interact. */
typedef short W;

extern W is_flag(W n);
extern long menu_input(long p);
extern void ev_cycle(void);
extern void act_poll(void);
extern W menu_req, menu_mouse, menu_state, text_mode, text_menu_ok, no_mouse_lim, play_top;
extern volatile W mouse_x2, mouse_y2;
extern W mse_vx, mse_vy;
extern unsigned char ptr_flags, agi_vars[];

static W armed;

void menu_poll(void)
{
    W cursor = 0;                         /* a game cursor is up: the pointer is hidden */
    if (mse_vx && !no_mouse_lim) {
        agi_vars[mse_vx] = mouse_x2 >> 2;
        agi_vars[mse_vy] = mouse_y2 - play_top;
        cursor = !(ptr_flags & 0x40);
    }
    if (!cursor) {
        if (mouse_y2 > play_top) armed = 1;
        else if (armed && play_top > 0 && !menu_req && !menu_state && !no_mouse_lim &&
                 (!text_mode || text_menu_ok) && !is_flag(33)) {
            armed = 0;
            menu_mouse = 2;
            menu_input(0);
        }
    }
    ev_cycle();
    act_poll();
}
