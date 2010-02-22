#include "ct_display.h"

/**
 *
 * (x0, y0) ------- (xn, y0)  x
 * |        columes
 * | lines
 * |
 * (x0, yn)
 *
 * y
 *
 */

#define CT_SCREEN_CELL_X   2
#define CT_SCREEN_CELL_Y   1

WINDOW *mainwin, *leftwin;

static int score = 0;

static uchar ct_screen_buffer[CT_SCREEN_Y][CT_SCREEN_X];
static uchar ct_screen_bg[CT_SCREEN_Y][CT_SCREEN_X];

int
ct_display_init()
{
    /* default window called strscr */
    mainwin = initscr();        /* initialize the curses library */
    cbreak();                   /* take input chars one at a time, no wait for \n */
    noecho();
    nodelay(mainwin, TRUE);
    nonl();                     /* tell curses not to do NL->CR/NL on output */
    keypad(mainwin, TRUE);      /* enable keyboard mapping (function key -> single value) */
    curs_set(0);                /* set cursor invisible */

    if (has_colors()) {
        start_color();
        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }

    leftwin =
        newwin((CT_SCREEN_Y + 1) * CT_SCREEN_CELL_Y, (CT_SCREEN_X + 1) * CT_SCREEN_CELL_X, 0, 0);
    return 0;
}

void
ct_display_show_cell(int y, int x)
{
    // check confine
    if (y < 0 || y >= CT_SCREEN_Y) {
        ct_log("y reach out of confine");
    } else if (x < 0 || x >= CT_SCREEN_X) {
        ct_log("x reach out of confine");
    }

    uchar cell = ct_screen_buffer[y][x];
    wattrset(leftwin, COLOR_PAIR(XCOLOR_OF(cell) % 8));

    if (XSTATUS_OF(cell)) {
        mvwaddch(leftwin, y * CT_SCREEN_CELL_Y, x * CT_SCREEN_CELL_X, '[');
        mvwaddch(leftwin, y * CT_SCREEN_CELL_Y, x * CT_SCREEN_CELL_X + 1, ']');
    } else {
        mvwaddch(leftwin, y * CT_SCREEN_CELL_Y, x * CT_SCREEN_CELL_X, ' ');
        mvwaddch(leftwin, y * CT_SCREEN_CELL_Y, x * CT_SCREEN_CELL_X + 1, ' ');
    }
}

void
ct_display_update(int top_y, int btm_y, int lft_x, int rgt_x)
{
    int i, j;

    struct block *b = cur_b;

    static bool ok = false;
    if (!ok) {
        // main block
        wattrset(leftwin, COLOR_PAIR(7 % 8));
        for (i = CT_SCREEN_Y; i < CT_SCREEN_Y + 1; i++) {
            for (j = 0; j < CT_SCREEN_X + 1; j++) {
                mvwaddch(leftwin, i * CT_SCREEN_CELL_Y, j * CT_SCREEN_CELL_X, '[');
                mvwaddch(leftwin, i * CT_SCREEN_CELL_Y, j * CT_SCREEN_CELL_X + 1, ']');
            }
        }
        for (i = 0; i < CT_SCREEN_Y + 1; i++) {
            for (j = CT_SCREEN_X; j < CT_SCREEN_X + 1; j++) {
                mvwaddch(leftwin, i * CT_SCREEN_CELL_Y, j * CT_SCREEN_CELL_X, '[');
                mvwaddch(leftwin, i * CT_SCREEN_CELL_Y, j * CT_SCREEN_CELL_X + 1, ']');
            }
        }
        wrefresh(leftwin);
        ok = true;
    }

    // cur bg
    for (i = 0; i < CT_SCREEN_Y; i++) {
        for (j = 0; j < CT_SCREEN_X; j++) {
            ct_screen_buffer[i][j] = ct_screen_bg[i][j];
        }
    }

    // cur block
    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(cur_b->show[i][j])) {
                ct_screen_buffer[cur_y + i][cur_x + j] = cur_b->show[i][j];
            }
        }
    }

    // show
    for (i = top_y; i <= btm_y; i++) {
        for (j = lft_x; j <= rgt_x; j++) {
            ct_display_show_cell(i, j);
        }
    }

    wrefresh(leftwin);
}

void
ct_display_set_block(int y, int x, struct block *b)
{
    int i, j;

    if (y <= 0) {
        // lose game
        exit(-1);
    }

    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(b->show[i][j])) {
                ct_screen_bg[y + i][x + j] = b->show[i][j];
            }
        }
    }

    int erased_num = 0;
    int erased_lines[4] = { -1, -1, -1, -1 };
    for (i = y + b->y_min; i <= y + b->y_max; i++) {
        for (j = 0; j < CT_SCREEN_X; j++) {
            if (!XSTATUS_OF(ct_screen_bg[i][j])) {
                goto next;
            }
        }

        // erase
        for (j = 0; j < CT_SCREEN_X; j++) {
            ct_screen_bg[i][j] = 0;
        }

        ct_display_update(i, i, 0, CT_SCREEN_X - 1);
        erased_lines[erased_num++] = i;
      next:
        continue;
    }


    // erase lines
    int _i;
    for (i = 0; i < erased_num; i++) {
        for (_i = erased_lines[i]; _i >= 0 && _i < CT_SCREEN_Y; _i--) {
            for (j = 0; j < CT_SCREEN_X; j++) {
                if ((_i - 1) < 0) {
                    ct_log("line %d to line %d, but use blank line as line %d ", _i - 1, _i, _i - 1);
                    // out of screen, use blank line
                    ct_screen_bg[_i][j] = 0;
                } else {
                    ct_log("line %d to line %d", _i - 1, _i);
                    ct_screen_bg[_i][j] = ct_screen_bg[_i - 1][j];
                }
            }
        }
    }

    // redisplay all screen
    ct_display_update(0, CT_SCREEN_Y - 1, 0, CT_SCREEN_X - 1);

    switch (erased_num) {
        case 1:
            score++;
            break;
        case 2:
            score += 3;
            break;
        case 3:
            score += 7;
            break;
        case 4:
            score += 13;
            break;
        default:
            break;
    }

}

bool
ct_display_check_shape(struct block *b, int y, int x)
{
    // bg blocks
    if (x + b->x_min < 0) {
        return false;
    } else if (x + b->x_max >= CT_SCREEN_X) {
        return false;
    } else if (y + b->y_min < 0) {
        return false;
    } else if (y + b->y_max >= CT_SCREEN_Y) {
        return false;
    } else {

        int i, j;
        for (i = b->y_min; i <= b->y_max; i++) {
            for (j = b->x_min; j <= b->x_max; j++) {
                if (XSTATUS_OF(b->show[i][j]) && XSTATUS_OF(ct_screen_bg[y + i][x + j])) {
                    return false;
                }
            }
        }
        return true;
    }
}

void
ct_display_move_block(int y, int x, struct block *b)
{
    static struct block *_b = NULL;

    static int _y, _x;

    if (!_b) {
        _b = b;
        _y = y;
        _x = x;
    } else {
        // does not change
        if (_b == b && _y == y && _x == x) {
            return;
        }
    }

    int top_y = (int)fmin(_y + _b->y_min, y + b->y_min);
    int btm_y = (int)fmax(_y + _b->y_max, y + b->y_max);
    int lft_x = (int)fmin(_x + _b->x_min, x + b->y_min);
    int rgt_x = (int)fmax(_x + _b->x_max, x + b->x_max);

    _b = b;
    _y = y;
    _x = x;

    ct_display_update(top_y, btm_y, lft_x, rgt_x);
}
