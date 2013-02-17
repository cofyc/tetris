#include "tetris.h"
#include "ct_block.h"
#include "ct_display.h"
#include "ct_debug.h"
#include "ct_game.h"
#include "argparse/argparse.h"

static const char *const tetris_usage[] = { 
    "tetris [options]",
    NULL
};

int
main(int argc, const char **argv)
{
    int debug = false;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('d', "debug", &debug, "log debug info"),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, tetris_usage, 0);
    argc = argparse_parse(&argparse, argc, argv);
    if (debug) {
        ct_debug_init();
        ct_debug_enable();
    }
    ct_display_init();
    ct_block_init();
    ct_game_init();

    return ct_game_run();
}
