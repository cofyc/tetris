#include "ct_game.h"
#include "ct_block.h"
#include "ct_display.h"
#include "ct_debug.h"

static void ct_game_end();

int
ct_game_init()
{
    ct_display_block_new();
    return 0;
}

static int
ct_game_block_down()
{
    int status = ct_display_block_check(cur_b, cur_y + 1, cur_x);
    if (!status) {
        cur_y++;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_left()
{
    int status = ct_display_block_check(cur_b, cur_y, cur_x - 1);
    if (!status) {
        cur_x--;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_right()
{
    int status = ct_display_block_check(cur_b, cur_y, cur_x + 1);
    if (!status) {
        cur_x++;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_change()
{
    int status = ct_display_block_check(ct_block_get(cur_type, (cur_shape + 1) % 4), cur_y, cur_x);
    if (!status) {
        cur_shape++;
        cur_shape = cur_shape % 4;
        cur_b = ct_block_get(cur_type, cur_shape);
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

void
ct_game_daemon()
{
    int status = ct_game_block_down();
    if (status) {
        if (!ct_display_block_set(cur_y, cur_x, cur_b)) {
            ct_display_block_new();
        } else {
            ct_game_end();
        }
    }
}

int
ct_game_run()
{
    struct itimerval tout_val, ovalue;
    tout_val.it_interval.tv_usec = 400 * 1000;
    tout_val.it_interval.tv_sec = 0;
    tout_val.it_value.tv_usec = 400 * 1000;
    tout_val.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    if (signal(SIGALRM, ct_game_daemon) == SIG_ERR)
        error("signal error");

    for (;;) {
        int c = getch();
        switch (c) {
            case 'h':
                ct_game_block_left();
                break;
            case 'j':
                ct_game_block_down();
                ct_game_block_down();
                break;
            case 'k':
                ct_game_block_change();
                break;
            case 'l':
                ct_game_block_right();
                break;
            case '\x20':
                while (!ct_game_block_down());
                break;
            default:
                ct_debug_log("keystroke: %d", c);
                break;
        }
    }

    return 0;
}

static void
ct_game_end()
{
    ct_display_end();
    printf("You lose.\n");
    exit(0);
}
