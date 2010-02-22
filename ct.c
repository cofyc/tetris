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

void
loop_main()
{
    if (cur_step >= step) {
        if (cur_b) {
            if (ct_display_check_shape(cur_b, cur_y + 1, cur_x)) {
                cur_y++;
                ct_display_move_block(cur_y, cur_x, cur_b);
            } else {
                ct_display_set_block(cur_y, cur_x, cur_b);
                // new 
                cur_b = rand_block();
                cur_x = CT_SCREEN_X / 2 - 2;
                cur_y = 0 - cur_b->y_min;
                ct_display_move_block(cur_y, cur_x, cur_b);
            }
        } else {
            cur_b = rand_block();
            cur_x = CT_SCREEN_X / 2 - 2;
            cur_y = 0 - cur_b->y_min;
            ct_display_move_block(cur_y, cur_x, cur_b);
        }
        cur_step = 0;
    } else {
        cur_step++;
    }
}

int
main(int argc, const char **argv)
{
    ct_blocks_init();

    setlocale(LC_ALL, "C");

    /* initialize your non-curses data structures here */

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        error("signal error");

    if (signal(SIGALRM, loop_main) == SIG_ERR)
        error("signal error");

    /* 0.01s */
    struct itimerval tout_val, ovalue;
    tout_val.it_interval.tv_usec = 10 * 1000;
    tout_val.it_interval.tv_sec = 0;
    tout_val.it_value.tv_usec = 10 * 1000;
    tout_val.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    ct_display_init();

    // init
    cur_b = NULL;
    cur_y = cur_x = 0;

    for (;;) {
        int c = getch();

        /* process the command keystroke */
        switch (c) {
            case 'h':
                if (ct_display_check_shape(cur_b, cur_y, cur_x - 1)) {
                    cur_x--;
                    cur_step = step;
                }
                break;
            case 'j':
                if (ct_display_check_shape(cur_b, cur_y + 2, cur_x)) {
                    cur_y += 2;
                    cur_step = step;
                }
                break;
            case 'k':
                cur_shape++;
                if (ct_display_check_shape(get_block(cur_type, cur_shape), cur_y, cur_x)) {
                    cur_b = get_block(cur_type, cur_shape);
                    cur_step = step;
                } else {
                    cur_shape--;
                }
                break;
            case 'l':
                if (ct_display_check_shape(cur_b, cur_y, cur_x + 1)) {
                    cur_x++;
                    cur_step = step;
                }
                break;
            default:
                break;
        }
    }
}
