#ifndef CT_DISPLAY_H
#define CT_DISPLAY_H

#include <ncurses.h>

#include "ct.h"
#include "ct_block.h"

#define CT_SCREEN_Y 24
#define CT_SCREEN_X 12
#define CT_SIDEBAR_Y 24
#define CT_SIDEBAR_X 12

extern int ct_display_init();
extern int ct_display_set_block(int y, int x, struct block *b);
extern int ct_display_check_shape(struct block *b, int y, int x);
extern int ct_display_move_block(int y, int x, struct block *b);
extern int ct_display_end();
extern int ct_display_update_sidebar();

#endif
