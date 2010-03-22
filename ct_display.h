#ifndef CT_DISPLAY_H
#define CT_DISPLAY_H

#include <ncurses.h>

#include "ct.h"
#include "ct_block.h"

extern int ct_display_init();
extern int ct_display_block_set(int y, int x, struct block *b);
extern int ct_display_block_move(int y, int x, struct block *b);
extern int ct_display_block_check(struct block *b, int y, int x);
extern int ct_display_end();
extern int ct_display_block_new();

#endif
