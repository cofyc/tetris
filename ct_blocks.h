#include "ctetris.h"

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
extern int prev_type, prev_shape;

extern void ct_init_blocks();

extern struct block *rand_block();

extern struct block *get_block(int type, int shape);
