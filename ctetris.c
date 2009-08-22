#include "ctetris.h"

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

static unsigned int step = 1000;

static unsigned int cur_step = 0;

#define win_line 20
#define win_cols 12

uchar win_buffer[win_line][win_cols];

uchar win_bg[win_line][win_cols];

typedef struct _block block;

#define XCOLOR_OF(cell)     ((cell) & 0x0F)
#define XSTATUS_OF(cell)    ((cell) >> 7)

struct _block {
    uchar show[4][4];
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

block blocks[7][4] = {
    /*
     * ----------
     * |        |
     * |  [][]  |
     * |  [][]  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0, 0x81, 0x81, 0},
       {0, 0x81, 0x81, 0},
       {0, 0, 0, 0}
       },
      1,
      2,
      1,
      2},
     // 
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |  []    |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x82, 0x82, 0x82, 0},
       {0, 0x82, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     //
     },
    /*
     * ----------
     * |        |
     * |[][]    |
     * |  [][]  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x83, 0x83, 0, 0},
       {0, 0x83, 0x83, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |  [][]  |
     * |[][]    |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0, 0x84, 0x84, 0},
       {0x84, 0x84, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |    []  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x85, 0x85, 0x85, 0},
       {0, 0, 0x85, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |[]      |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x86, 0x86, 0x86, 0},
       {0x86, 0, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][][]|
     * |        |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x87, 0x87, 0x87, 0x87},
       {0, 0, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      3,
      1,
      1,
      }
     },
};

/** 
 *
 * standard screen (window): mainwin
 *
 * (0, 0) 
 *
 *
 *
 */

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

/**
 *
 * 90" clockwise rotation
 *
 * y axis -> x axis
 * x axis -> reverse of x axis
 */
static void
rotate_block(block *to, block *from)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            to->show[i][j] = from->show[4 - 1 - j][i];
        }
    }

    to->x_min = 4 - 1 - from->y_max;
    to->x_max = 4 - 1 - from->y_min;
    to->y_min = from->x_min;
    to->y_max = from->x_max;
}

block *cur_b = NULL, *prev_b = NULL;

int shape = 0;

int type = 0;

int score = 0;

int cur_y, cur_x;

static void
updateScore()
{
    mvwprintw(rightwin, 1, 1, "score: %d", score);
    mvwprintw(rightwin, 2, 1, "         ", score);
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

    block *b = cur_b;

    // main block
    wattrset(mainwin, COLOR_PAIR(7 % 8));
    for (i = win_line; i < win_line + 1; i++) {
        for (j = 0; j < win_cols + 1; j++) {
            mvwaddch(mainwin, i * cell_y, j * cell_x, '[');
            mvwaddch(mainwin, i * cell_y, j * cell_x + 1, ']');
        }
    }
    for (i = 0; i < win_line + 1; i++) {
        for (j = win_cols; j < win_cols + 1; j++) {
            mvwaddch(mainwin, i * cell_y, j * cell_x, '[');
            mvwaddch(mainwin, i * cell_y, j * cell_x + 1, ']');
        }
    }
    wrefresh(mainwin);

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

    // leftwin
    updateScore();
}

/*
 * init all blocks
 */
static void
init_blocks()
{
    int i, j;

    for (i = 0; i < 7; i++) {
        for (j = 1; j < 4; j++) {
            rotate_block(&blocks[i][j], &blocks[i][j - 1]);
        }
    }
}

static void
moveBlock(int y, int x, block *b)
{
    static block *_b = NULL;

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
setBlock(int y, int x, block *b)
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
check_shape(block *b, int y, int x)
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

static block *
get_block(int type, int shape)
{
    return &blocks[type % 7][shape % 4];
}

static block *
rand_block()
{
    static unsigned int seed = 0;

    struct timeval t;

    gettimeofday(&t, NULL);
    seed += t.tv_sec + t.tv_usec;
    srand(seed);
    shape = rand();
    srand(shape);
    type = rand();
    return get_block(type, shape);
}

static void
mylog(const char *fmt, ...)
{
    static FILE *logfile = NULL;

    char msg[1024];

    va_list params;

    va_start(params, fmt);

    if (!logfile) {
        logfile = fopen("log.txt", "a+");
    }

    vsnprintf(msg, sizeof(msg), fmt, params);
    fprintf(logfile, "%s\n", msg);

    fflush(logfile);            /* exit with some unexpected error, string in buffer may not be flushed out */
    va_end(params);
}

static void
loop()
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
msleep(unsigned long milliseconds)
{
    struct timespec ts;
    time_t sec = (int)(milliseconds / 1000);
    milliseconds = milliseconds - (sec * 1000);
    ts.tv_sec = sec;
    ts.tv_nsec = milliseconds * 1000;
    nanosleep(&ts, &ts);
    return 1;
}

static void *
loop_main(void *arg)
{
    while (true) {
        loop();
        msleep(10); // 0.01s
    }
    return (void *)0;
}

int
main(int argc, const char **argv)
{
    init_blocks();
    setlocale(LC_ALL, "C");

    /* initialize your non-curses data structures here */

    if (signal(SIGINT, sig_handler) == SIG_ERR)
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

    leftwin = newwin(win_line * cell_y, win_cols * cell_x, 0, 0);
    rightwin = newwin(win_line * cell_y, win_cols * cell_x, 0, (win_cols + 2) * cell_x);
    updateScore();

    int err;

    pthread_t loop_pid;

    err = pthread_create(&loop_pid, NULL, loop_main, NULL);
    if (err != 0) {
        die("pthread %s creating failed.", "info_printer");
    }
    // init
    cur_b = NULL;
    cur_y = cur_x = 0;

    for (;;) {

        int c = wgetch(rightwin);       /* refresh, accept single keystroke of input */

        /* process the command keystroke */
        switch (c) {
            case 'h':
                if (check_shape(cur_b, cur_y, cur_x - 1)) {
                    cur_x--;
                }
                break;
            case 'j':
                cur_step = step + 100;
                if (check_shape(cur_b, cur_y + 2, cur_x)) {
                    cur_y += 2;
                }
                break;
            case 'k':
                shape++;
                if (check_shape(get_block(type, shape), cur_y, cur_x)) {
                    cur_b = get_block(type, shape);
                } else {
                    shape--;
                }
                break;
            case 'l':
                if (check_shape(cur_b, cur_y, cur_x + 1)) {
                    cur_x++;
                }
                break;
            default:
                break;
        }

        moveBlock(cur_y, cur_x, cur_b);
    }

    /* We're done */
    sig_handler(0);
}
