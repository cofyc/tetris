#include "ct_game.h"
#include "ct_blocks.h"
#include "ct_display.h"
#include "ct_debug.h"

void
ct_game_init()
{

}

void
ct_game_end()
{
    ct_display_end();
    printf("You lose.\n");
    exit(0);
}


void
ct_game_daemon()
{
    if (cur_b) {
        if (ct_display_check_shape(cur_b, cur_y + 1, cur_x)) {
            cur_y++;
            ct_display_move_block(cur_y, cur_x, cur_b);
        } else {
            if (ct_display_set_block(cur_y, cur_x, cur_b)) {
                ct_game_end();
            }
            // new 
            cur_b = rand_block();
            cur_x = CT_SCREEN_X / 2 - 2;
            cur_y = 0 - cur_b->y_min;
            ct_display_move_block(cur_y, cur_x, cur_b);
        }
    } else {
        // new
        cur_b = rand_block();
        cur_x = CT_SCREEN_X / 2 - 2;
        cur_y = 0 - cur_b->y_min;
        ct_display_move_block(cur_y, cur_x, cur_b);
    }
}

void
ct_game_main()
{
    for (;;) {
        int c = getch();
        switch (c) {
            case 'h':
                if (ct_display_check_shape(cur_b, cur_y, cur_x - 1)) {
                    cur_x--;
                    ct_game_daemon();
                }
                break;
            case 'j':
                if (ct_display_check_shape(cur_b, cur_y + 2, cur_x)) {
                    cur_y += 2;
                    ct_game_daemon();
                }
                break;
            case 'k':
                if (ct_display_check_shape(get_block(cur_type, cur_shape), cur_y, cur_x)) {
                    cur_shape++;
                    cur_b = get_block(cur_type, cur_shape);
                    ct_game_daemon();
                }
                break;
            case 'l':
                if (ct_display_check_shape(cur_b, cur_y, cur_x + 1)) {
                    cur_x++;
                    ct_game_daemon();
                }
                break;
            case '\x20': /* space */
                while (ct_display_check_shape(cur_b, cur_y + 1 , cur_x)) {
                    cur_y++;
                }
                break;
            default:
                ct_debug_log("keystroke: %d", c);
                break;
        }
    }
}
