#ifndef CT_BLOCKS_H
#define CT_BLOCKS_H

#include "ct.h"

struct block {
    char show[4][4];
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

extern struct block *cur_b;
extern int cur_type, cur_shape;
extern int cur_y, cur_x;
extern struct block *next_b;
extern int next_type, next_shape;
extern int ct_block_init();
extern int ct_rand();
extern struct block *ct_block_get(int type, int shape);

#endif
