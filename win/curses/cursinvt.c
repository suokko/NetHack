/* vim:set cin ft=c sw=4 sts=4 ts=8 et ai cino=Ls\:0t0(0 : -*- mode:c;fill-column:80;tab-width:8;c-basic-offset:4;indent-tabs-mode:nil;c-file-style:"k&r" -*-*/

#include "curses.h"
#include "hack.h"
#include "wincurs.h"
#include "cursinvt.h"

/* Permanent inventory for curses interface */

/* Runs when the game indicates that the inventory has been updated */
void
curses_update_inv(void)
{
    WINDOW *win = curses_get_nhwin(INV_WIN);
    boolean border;
    int x = 0;
    int y = 0;
    attr_t attr = A_UNDERLINE;

    /* Check if the inventory window is enabled in first place */
    if (!win) {
        /* It's not. Re-initialize the main windows if the
           option was enabled. */
        if (iflags.perm_invent) {
            curses_create_main_windows();
            curses_last_messages();
            doredraw();
        }
        return;
    }

    border = curses_window_has_border(INV_WIN);

    /* Figure out drawing area */
    if (border) {
        x++;
        y++;
    }

    /* Clear the window as it is at the moment. */
    werase(win);

    wmove(win, y, x);
    wattron(win, attr);
    wprintw(win, "Inventory:");
    wattroff(win, attr);

    /* The actual inventory will override this if we do carry stuff */
    wmove(win, y + 1, x);
    wprintw(win, "Not carrying anything");

    display_inventory(NULL, FALSE);

    if (border)
        box(win, 0, 0);

    wnoutrefresh(win);
}

/* Adds an inventory item. */
void
curses_add_inv(int y, int glyph, CHAR_P accelerator, attr_t attr,
               const char *str)
{
    WINDOW *win = curses_get_nhwin(INV_WIN);
    int color = NO_COLOR;

    /* Figure out where to draw the line */
    int x = 0;
    if (curses_window_has_border(INV_WIN)) {
        x++;
        y++;
    }

    wmove(win, y, x);
    if (accelerator) {
        attr_t bold = A_BOLD;
        wattron(win, bold);
        waddch(win, accelerator);
        wattroff(win, bold);
        wprintw(win, ") ");
    }
#if 0 /* FIXME: MENU GLYPHS */
    if (accelerator && glyph != NO_GLYPH && iflags.use_menu_glyphs) {
        unsigned dummy = 0; /* Not used */
        int color = 0;
        int symbol = 0;
        mapglyph(glyph, &symbol, &color, &dummy,
                     u.ux, u.uy);
        attr_t glyphclr = curses_color_attr(color, 0);
        wattron(win, glyphclr);
        wprintw(win, "%c ", symbol);
        wattroff(win, glyphclr);
    }
#endif
    if (accelerator && /* Don't colorize categories */
        iflags.use_menu_color) {
        char str_mutable[BUFSZ];
        Strcpy(str_mutable, str);
        attr = 0;
        get_menu_coloring(str_mutable, &color, (int *) &attr);
        attr = curses_convert_attr(attr);
    }
    if (color == NO_COLOR) color = NONE;
    curses_toggle_color_attr(win, color, attr, ON);
    /* wattron(win, attr); */
    wprintw(win, "%s", str);
    /* wattroff(win, attr); */
    curses_toggle_color_attr(win, color, attr, OFF);
    wclrtoeol(win);
}
