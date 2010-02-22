#ifndef CT_BLOCKS_H
#define CT_BLOCKS_H
#include "ct.h"

struct block {
    uchar show[4][4];
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

#define XCOLOR_OF(cell)     ((cell) & 0x0F)
#define XSTATUS_OF(cell)    ((cell) >> 7)

extern struct block *cur_b, *prev_b;

extern int cur_type, cur_shape;

extern int cur_y, cur_x;

extern int prev_type, prev_shape;

extern unsigned int step;

extern unsigned int cur_step;

extern void ct_blocks_init();

extern struct block *rand_block();

extern struct block *get_block(int type, int shape);
#endif
