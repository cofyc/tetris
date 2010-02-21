#ifndef CT_DISPLAY_H
#define CT_DISPLAY_H
#include "ct.h"
#include "ct_blocks.h"

extern WINDOW *mainwin, *leftwin, *rightwin;
extern int ct_display_init();
extern void showCell(int y, int x);
extern void refreshMain(int top_y, int btm_y, int lft_x, int rgt_x);
extern void setBlock(int y, int x, struct block *b);
extern bool check_shape(struct block *b, int y, int x);
extern void loop_main();
extern void moveBlock(int y, int x, struct block *b);
extern void updateScore();

#endif
