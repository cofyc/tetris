#include "ct.h"
#include "ct_block.h"
#include "ct_display.h"
#include "ct_debug.h"
#include "ct_game.h"

int
main(int argc, char **argv)
{
    ct_debug_init();
    ct_display_init();
    ct_block_init();
    ct_game_init();

    static struct option long_options[] = {
        {"debug", no_argument, 0, 'd'},
        {0, 0, 0, 0},
    };

    int c;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                ct_debug_enable();
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    return ct_game_run();
}
