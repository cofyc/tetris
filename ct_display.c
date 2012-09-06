#include "ct_display.h"
#include "ct_debug.h"

/**
 *
 * (x0, y0) ------- (xn, y0)  x axis
 * |        columes
 * | lines
 * |
 * (x0, yn)
 *
 * y axis
 *
 */

#define CT_DISPLAY_CELL_X   2
#define CT_DISPLAY_CELL_Y   1
#define CT_DISPLAY_MAIN_Y 24
#define CT_DISPLAY_MAIN_X 12
#define CT_DISPLAY_SIDEBAR_Y 24
#define CT_DISPLAY_SIDEBAR_X 12

#define XCOLOR_OF(cell)     ((cell) & 0x0F)
#define XSTATUS_OF(cell)    ((cell) >> 7)

static int score = 0;

struct ct_window {
    WINDOW *win;
    int x;
    int y;
};

struct ct_window ct_win_main;
struct ct_window ct_win_sidebar;

static char ct_screen_buffer[CT_DISPLAY_MAIN_Y][CT_DISPLAY_MAIN_X];
static char ct_screen_bg[CT_DISPLAY_MAIN_Y][CT_DISPLAY_MAIN_X];

static int ct_display_update(int top_y, int btm_y, int lft_x, int rgt_x);
static int ct_display_update_sidebar();

int
ct_display_init()
{
    /* initialize the curses library, return pointer to stdscr */
    initscr();
    /* take input chars one at a time, no wait for \n */
    cbreak();
    noecho();
    /* if false, then reads will block */
    nodelay(stdscr, false);
    /* tell curses not to do NL->CR/NL on output */
    nonl();
    /* enable keyboard mapping (function key -> single value) */
    keypad(stdscr, true);
    /*
     * set cursor invisible
     * @Note: mac os x terminal don't support this currently
     */
    curs_set(0);

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
    // bg
    for (int i = 0; i < CT_DISPLAY_MAIN_Y; i++) {
        for (int j = 0; j < CT_DISPLAY_MAIN_X; j++) {
            ct_screen_buffer[i][j] = 0;
        }
    }

    // bg borders
    wattrset(stdscr, COLOR_PAIR(7 % 8));
    for (int i = CT_DISPLAY_MAIN_Y; i < CT_DISPLAY_MAIN_Y + 1; i++) {
        for (int j = 0; j < CT_DISPLAY_MAIN_X + 1; j++) {
            mvwaddch(stdscr, i * CT_DISPLAY_CELL_Y, j * CT_DISPLAY_CELL_X, '[');
            mvwaddch(stdscr, i * CT_DISPLAY_CELL_Y, j * CT_DISPLAY_CELL_X + 1, ']');
        }
    }
    for (int i = 0; i < CT_DISPLAY_MAIN_Y + 1; i++) {
        for (int j = CT_DISPLAY_MAIN_X; j < CT_DISPLAY_MAIN_X + 1; j++) {
            mvwaddch(stdscr, i * CT_DISPLAY_CELL_Y, j * CT_DISPLAY_CELL_X, '[');
            mvwaddch(stdscr, i * CT_DISPLAY_CELL_Y, j * CT_DISPLAY_CELL_X + 1, ']');
        }
    }
    wrefresh(stdscr);

    // sidebar info
    ct_win_main.win =
        newwin(CT_DISPLAY_MAIN_Y * CT_DISPLAY_CELL_Y, CT_DISPLAY_MAIN_X * CT_DISPLAY_CELL_X, 0, 0);
    ct_win_main.x = CT_DISPLAY_MAIN_X;
    ct_win_main.y = CT_DISPLAY_MAIN_Y;

    ct_win_sidebar.win =
        newwin(CT_DISPLAY_SIDEBAR_Y * CT_DISPLAY_CELL_Y, CT_DISPLAY_SIDEBAR_X * CT_DISPLAY_CELL_X,
               0, (CT_DISPLAY_MAIN_X + 1) * CT_DISPLAY_CELL_X);
    ct_win_sidebar.x = CT_DISPLAY_SIDEBAR_X;
    ct_win_sidebar.y = CT_DISPLAY_SIDEBAR_Y;

    ct_display_update_sidebar();

    return 0;
}

static int
ct_display_show_cell(struct ct_window *ct_win, int y, int x, char cell)
{
    // check
    if (y < 0 || y >= ct_win->y) {
        return 1;
    } else if (x < 0 || x >= ct_win->x) {
        return 2;
    }
    // color
    wattrset(ct_win->win, COLOR_PAIR(XCOLOR_OF(cell) % 8));

    // show
    if (XSTATUS_OF(cell)) {
        mvwaddch(ct_win->win, y * CT_DISPLAY_CELL_Y, x * CT_DISPLAY_CELL_X, '[');
        mvwaddch(ct_win->win, y * CT_DISPLAY_CELL_Y, x * CT_DISPLAY_CELL_X + 1, ']');
    } else {
        mvwaddch(ct_win->win, y * CT_DISPLAY_CELL_Y, x * CT_DISPLAY_CELL_X, ' ');
        mvwaddch(ct_win->win, y * CT_DISPLAY_CELL_Y, x * CT_DISPLAY_CELL_X + 1, ' ');
    }

    return 0;
}

static int
ct_display_show_block(struct ct_window *win, int x, int y, struct block *block)
{
    int i, j;

    if (!win) {
        return 1;
    }

    if (!block) {
        return 2;
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            ct_display_show_cell(win, i + 1, j + 1, block->show[i][j]);
        }
    }

    return 0;
}

static int
ct_display_update(int top_y, int btm_y, int lft_x, int rgt_x)
{
    int i, j;

    // cur bg
    for (i = top_y; i <= btm_y; i++) {
        for (j = lft_x; j <= rgt_x; j++) {
            ct_screen_buffer[i][j] = ct_screen_bg[i][j];
        }
    }

    // cur block
    for (i = cur_b->y_min; i <= cur_b->y_max; i++) {
        for (j = cur_b->x_min; j <= cur_b->x_max; j++) {
            if (XSTATUS_OF(cur_b->show[i][j])) {
                ct_screen_buffer[cur_y + i][cur_x + j] = cur_b->show[i][j];
            }
        }
    }

    // show
    for (i = top_y; i <= btm_y; i++) {
        for (j = lft_x; j <= rgt_x; j++) {
            ct_display_show_cell(&ct_win_main, i, j, ct_screen_buffer[i][j]);
        }
    }

    wrefresh(ct_win_main.win);

/*
 * Show cursor on sidebar, cuz we can not hide it in Mac OS X Terminal.
 */
#ifdef __APPLE__
    ct_display_update_sidebar();
#endif

    return 0;
}

static int
ct_display_update_sidebar()
{
    // show block
    ct_display_show_block(&ct_win_sidebar, 1, 1, next_b);

    // info
    mvwprintw(ct_win_sidebar.win, 6, 2, "score: %d", score);
    mvwprintw(ct_win_sidebar.win, 7, 2, "change: k");
    mvwprintw(ct_win_sidebar.win, 8, 2, "left: h right: l");
    mvwprintw(ct_win_sidebar.win, 9, 2, "down: j");
    mvwprintw(ct_win_sidebar.win, 10, 2, "fast down: [space]");

    // refresh
    wrefresh(ct_win_sidebar.win);

    return 0;
}

int
ct_display_block_set(int y, int x, struct block *b)
{
    int i, j;

    if (y <= 0) {
        // lose game
        return 1;
    }
    // set block into bg
    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(b->show[i][j])) {
                ct_screen_bg[y + i][x + j] = b->show[i][j];
            }
        }
    }
    ct_display_update(0, CT_DISPLAY_MAIN_Y - 1, 0, CT_DISPLAY_MAIN_X - 1);

    // 
    int erased_num = 0;
    int erased_lines[4] = { -1, -1, -1, -1 };
    for (i = y + b->y_min; i <= y + b->y_max; i++) {
        for (j = 0; j < CT_DISPLAY_MAIN_X; j++) {
            if (!XSTATUS_OF(ct_screen_bg[i][j])) {
                goto next;
            }
        }

        // erase
        for (j = 0; j < CT_DISPLAY_MAIN_X; j++) {
            ct_screen_bg[i][j] = 0;
        }

        ct_display_update(i, i, 0, CT_DISPLAY_MAIN_X - 1);
        erased_lines[erased_num++] = i;
    next:
        continue;
    }

    // erase lines
    ct_debug_log("erased num: %d",erased_num);
    int _i;
    for (i = 0; i < erased_num; i++) {
        for (_i = erased_lines[i]; _i >= 0 && _i < CT_DISPLAY_MAIN_Y; _i--) {
            for (j = 0; j < CT_DISPLAY_MAIN_X; j++) {
                if ((_i - 1) < 0) {
                    ct_debug_log("line %d to line %d, but use blank line as line %d ", _i - 1, _i,
                                 _i - 1);
                    // out of screen, use blank line
                    ct_screen_bg[_i][j] = 0;
                } else {
                    ct_debug_log("line %d to line %d", _i - 1, _i);
                    ct_screen_bg[_i][j] = ct_screen_bg[_i - 1][j];
                }

                ct_display_update(0, CT_DISPLAY_MAIN_Y - 1, 0, CT_DISPLAY_MAIN_X - 1);
            }
        }
    }

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
    ct_debug_log("score: %d", score);

    ct_display_update_sidebar();

    return 0;
}

int
ct_display_block_check(struct block *b, int y, int x)
{
    // bg block
    if (x + b->x_min < 0) {
        return 1;
    } else if (x + b->x_max >= CT_DISPLAY_MAIN_X) {
        return 2;
    } else if (y + b->y_min < 0) {
        return 3;
    } else if (y + b->y_max >= CT_DISPLAY_MAIN_Y) {
        return 4;
    } else {

        int i, j;
        for (i = b->y_min; i <= b->y_max; i++) {
            for (j = b->x_min; j <= b->x_max; j++) {
                if (XSTATUS_OF(b->show[i][j]) && XSTATUS_OF(ct_screen_bg[y + i][x + j])) {
                    return 5;
                }
            }
        }

        return 0;
    }
}

int
ct_display_block_move(int y, int x, struct block *b)
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
            return 0;
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

    return 0;
}

int
ct_display_block_new()
{
    if (!next_b) {
        next_type = ct_rand() % 7;
        next_shape = ct_rand() % 4;
        next_b = ct_block_get(next_type, next_shape);
    }

    cur_b = next_b;
    cur_type = next_type;
    cur_shape = next_shape;

    next_type = ct_rand() % 7;
    next_shape = ct_rand() % 4;
    next_b = ct_block_get(next_type, next_shape);

    cur_x = CT_DISPLAY_MAIN_X / 2 - 2;
    cur_y = 0;

    ct_display_block_move(cur_y, cur_x, cur_b);
    ct_display_update_sidebar();
    return 0;
}

int
ct_display_end()
{
    endwin();
    printf("You got score: %d\n", score);
    return 0;
}
