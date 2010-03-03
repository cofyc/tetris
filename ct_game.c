#include "ct_game.h"
#include "ct_blocks.h"
#include "ct_display.h"
#include "ct_debug.h"

void
ct_game_init() {

}

void
ct_game_daemon()
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

void 
ct_game_main()
{
    for (;;) {
        int c = getch();
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
                ct_debug_log("here?");
                break;
        }
    }
}
