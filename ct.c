#include "ct.h"
#include "ct_blocks.h"
#include "ct_display.h"



static void
sig_handler(int sig)
{
    endwin();
    /* do your non-curses wrapup here */
    exit(0);
}


int
main(int argc, const char **argv)
{
    ct_blocks_init();

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

    ct_display_init();
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
}
