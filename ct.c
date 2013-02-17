#include "ct.h"
#include "ct_block.h"
#include "ct_display.h"
#include "ct_debug.h"
#include "ct_game.h"

const char ct_usage_string[] =
    "Usage: ct 1.0\n"
    "   -h  show this help info\n" "   -d  enable debug mode\n";

int
main(int argc, char **argv)
{
    int c;
    while ((c = getopt(argc, argv, "dh")) != -1) {
        switch (c) {
        case 'd':
            ct_debug_init();
            ct_debug_enable();
            break;
        case 'h':
            printf("%s", ct_usage_string);
            exit(0);
        default:
            break;
        }
    }

    ct_display_init();
    ct_block_init();
    ct_game_init();

    return ct_game_run();
}
