#ifndef CT_DISPLAY_H
#define CT_DISPLAY_H
#include "ct.h"
#include "ct_blocks.h"

#define CT_SCREEN_Y 24
#define CT_SCREEN_X 12

extern WINDOW *mainwin, *leftwin;

extern int ct_display_init();
extern void ct_display_show_cell(int y, int x);
extern void ct_display_update(int top_y, int btm_y, int lft_x, int rgt_x);
extern void ct_display_set_block(int y, int x, struct block *b);
extern bool ct_display_check_shape(struct block *b, int y, int x);
extern void ct_display_move_block(int y, int x, struct block *b);
#endif
