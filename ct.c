#include "ct.h"
#include "ct_blocks.h"
#include "ct_display.h"
#include "ct_debug.h"
#include "ct_game.h"


int
main(int argc, const char **argv)
{
    setlocale(LC_ALL, "");
    ct_blocks_init();
    ct_display_init();
    ct_debug_init();
    ct_game_init();

    /* 0.01s */
    struct itimerval tout_val, ovalue;
    tout_val.it_interval.tv_usec = 10 * 1000;
    tout_val.it_interval.tv_sec = 0;
    tout_val.it_value.tv_usec = 10 * 1000;
    tout_val.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    if (signal(SIGALRM, ct_game_daemon) == SIG_ERR)
        error("signal error");

    ct_game_main();

}
