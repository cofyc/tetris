#include "ct.h"
#include "ct_blocks.h"
#include "ct_display.h"
#include "ct_debug.h"
#include "ct_game.h"

int
main(int argc, char **argv)
{

    static struct option long_options[] = {
        { "debug", no_argument, 0, 'd' },
        { 0, 0, 0, 0 },
    };

    int c;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                ct_debug_init();
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    ct_display_init();
    ct_blocks_init();
    ct_game_init();

    /* 0.01s */
    struct itimerval tout_val, ovalue;
    tout_val.it_interval.tv_usec = 400 * 1000;
    tout_val.it_interval.tv_sec = 0;
    tout_val.it_value.tv_usec = 400 * 1000;
    tout_val.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    if (signal(SIGALRM, ct_game_daemon) == SIG_ERR)
        error("signal error");

    ct_game_main();
}
