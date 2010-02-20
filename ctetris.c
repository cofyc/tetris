#include "ctetris.h"
#include "ct_blocks.h"

/**
 * lines -> Y (i)
 * colums -> X (j)
 *
 * (x, y) - x
 * |
 * y
 *
 *
 */

WINDOW *mainwin, *leftwin, *rightwin;

static int cell_x = 2;

static int cell_y = 1;

static unsigned int step = 30;

static unsigned int cur_step = 0;

#define win_line 24
#define win_cols 12
uchar win_buffer[win_line][win_cols];

uchar win_bg[win_line][win_cols];

static void
sig_handler(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}

static void
showCell(int y, int x)
{
    uchar cell = win_buffer[y][x];

    wattrset(leftwin, COLOR_PAIR(XCOLOR_OF(cell) % 8));

    if (XSTATUS_OF(cell)) {
        mvwaddch(leftwin, y * cell_y, x * cell_x, '[');
        mvwaddch(leftwin, y * cell_y, x * cell_x + 1, ']');
    } else {
        mvwaddch(leftwin, y * cell_y, x * cell_x, ' ');
        mvwaddch(leftwin, y * cell_y, x * cell_x + 1, ' ');
    }
}


int score = 0;

int cur_y, cur_x;

static void
updateScore()
{
    mvwprintw(rightwin, 1, 1, "score: %d", score);
    mvwprintw(rightwin, 2, 1, "         ");
    mvwprintw(rightwin, 3, 1, "left: h");
    mvwprintw(rightwin, 4, 1, "right: l");
    mvwprintw(rightwin, 5, 1, "speed: j");
    mvwprintw(rightwin, 6, 1, "change: k");
    wrefresh(rightwin);
}

static void
refreshMain(int top_y, int btm_y, int lft_x, int rgt_x)
{
    int i, j;

    struct block *b = cur_b;

    // main block
    wattrset(leftwin, COLOR_PAIR(7 % 8));
    for (i = win_line; i < win_line + 1; i++) {
        for (j = 0; j < win_cols + 1; j++) {
            mvwaddch(leftwin, i * cell_y, j * cell_x, '[');
            mvwaddch(leftwin, i * cell_y, j * cell_x + 1, ']');
        }
    }
    for (i = 0; i < win_line + 1; i++) {
        for (j = win_cols; j < win_cols + 1; j++) {
            mvwaddch(leftwin, i * cell_y, j * cell_x, '[');
            mvwaddch(leftwin, i * cell_y, j * cell_x + 1, ']');
        }
    }
    wrefresh(leftwin);

    // cur bg
    for (i = 0; i < win_line; i++) {
        for (j = 0; j < win_cols; j++) {
            win_buffer[i][j] = win_bg[i][j];
        }
    }

    // cur block
    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(cur_b->show[i][j])) {
                win_buffer[cur_y + i][cur_x + j] = cur_b->show[i][j];
            }
        }
    }

    // show
    for (i = top_y; i <= btm_y; i++) {
        for (j = lft_x; j <= rgt_x; j++) {
            showCell(i, j);
        }
    }

    wrefresh(leftwin);

    //
//    updateScore();
}

static void
moveBlock(int y, int x, struct block *b)
{
    static struct block *_b = NULL;

    static int _y, _x;

    if (!_b) {
        _b = b;
        _y = y;
        _x = x;
    } else {
        // doesn't change
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

    refreshMain(top_y, btm_y, lft_x, rgt_x);
}

static void
setBlock(int y, int x, struct block *b)
{
    int i, j;

    if (y <= 0) {
        // lose game
        sig_handler(0);
    }

    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(b->show[i][j])) {
                win_bg[y + i][x + j] = b->show[i][j];
            }
        }
    }

    int erased_num = 0;
    int erased_lines[4] = { -1, -1, -1, -1 };
    for (i = y + b->y_min; i <= y + b->y_max; i++) {
        for (j = 0; j < win_cols; j++) {
            if (!XSTATUS_OF(win_bg[i][j])) {
                goto next;
            }
        }

        for (j = 0; j < win_cols; j++) {
            win_bg[i][j] = 0;
        }

        refreshMain(i, i, 0, win_cols - 1);
        erased_lines[erased_num++] = i;
      next:
        continue;
    }

    int _i;

    for (i = 0; i < erased_num; i++) {
        for (_i = erased_lines[i]; _i >= 0; _i--) {
            for (j = 0; j < win_cols; j++) {
                win_bg[_i][j] = win_bg[_i - 1][j];
                win_bg[_i - 1][j] = 0;
            }
        }
    }

    refreshMain(0, win_line, 0, win_cols - 1);

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

    updateScore();
}

static bool
check_shape(struct block *b, int y, int x)
{
    // bg blocks
    if (x + b->x_min < 0) {
        return false;
    } else if (x + b->x_max > win_cols - 1) {
        return false;
    } else if (y + b->y_min < 0) {
        return false;
    } else if (y + b->y_max > win_line - 1) {
        return false;
    } else {
        int i, j;

        for (i = b->y_min; i <= b->y_max; i++) {
            for (j = b->x_min; j <= b->x_max; j++) {
                if (XSTATUS_OF(b->show[i][j]) && XSTATUS_OF(win_bg[y + i][x + j])) {
                    return false;
                }
            }
        }

        return true;
    }
}


static void
loop_main()
{
    if (cur_step >= step) {
        if (cur_b) {
            if (check_shape(cur_b, cur_y + 1, cur_x)) {
                cur_y++;
                moveBlock(cur_y, cur_x, cur_b);
            } else {
                setBlock(cur_y, cur_x, cur_b);
                // new 
                cur_b = rand_block();
                cur_x = win_cols / 2 - 2;
                cur_y = 0 - cur_b->y_min;
                moveBlock(cur_y, cur_x, cur_b);
            }
        } else {
            cur_b = rand_block();
            cur_x = win_cols / 2 - 2;
            cur_y = 0 - cur_b->y_min;
            moveBlock(cur_y, cur_x, cur_b);
        }
        cur_step = 0;
    } else {
        cur_step++;
    }
}

int
main(int argc, const char **argv)
{
    ct_init_blocks();

    setlocale(LC_ALL, "C");

    /* initialize your non-curses data structures here */

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        error("signal error");

    struct itimerval tout_val, ovalue;

    /* 0.01s */
    tout_val.it_interval.tv_usec = 10 * 1000;
    tout_val.it_interval.tv_sec = 0;
    tout_val.it_value.tv_usec = 10 * 1000;
    tout_val.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    if (signal(SIGALRM, loop_main) == SIG_ERR)
        error("signal error");

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

    leftwin = newwin(win_line * cell_y + 1, win_cols * cell_x + 2, 0, 0);
    rightwin = newwin(win_line * cell_y, win_cols * cell_x, 0, (win_cols + 2) * cell_x);
    updateScore();

    // init
    cur_b = NULL;
    cur_y = cur_x = 0;

    for (;;) {

        int c = getch();

        /* process the command keystroke */
        switch (c) {
            case 'h':
                if (check_shape(cur_b, cur_y, cur_x - 1)) {
                    cur_x--;
                    cur_step = step;
                }
                break;
            case 'j':
                if (check_shape(cur_b, cur_y + 2, cur_x)) {
                    cur_y += 2;
                    cur_step = step;
                }
                break;
            case 'k':
                cur_shape++;
                if (check_shape(get_block(cur_type, cur_shape), cur_y, cur_x)) {
                    cur_b = get_block(cur_type, cur_shape);
                    cur_step = step;
                } else {
                    cur_shape--;
                }
                break;
            case 'l':
                if (check_shape(cur_b, cur_y, cur_x + 1)) {
                    cur_x++;
                    cur_step = step;
                }
                break;
            default:
                break;
        }
    }

    /* We're done */
    sig_handler(0);
}
