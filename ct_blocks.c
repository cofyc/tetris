#include "ct_blocks.h"

int cur_x = 0;
int cur_y = 0;

struct block *cur_b = NULL;
int cur_type = 0;
int cur_shape = 0;

struct block *next_b = NULL;
int next_type = 0;
int next_shape = 0;


struct block blocks[7][4] = {
    /*
     * ----------
     * |        |
     * |  [][]  |
     * |  [][]  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0, 0x81, 0x81, 0},
       {0, 0x81, 0x81, 0},
       {0, 0, 0, 0}
       },
      1,
      2,
      1,
      2},
     // 
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |  []    |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x82, 0x82, 0x82, 0},
       {0, 0x82, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     //
     },
    /*
     * ----------
     * |        |
     * |[][]    |
     * |  [][]  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x83, 0x83, 0, 0},
       {0, 0x83, 0x83, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |  [][]  |
     * |[][]    |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0, 0x84, 0x84, 0},
       {0x84, 0x84, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |    []  |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x85, 0x85, 0x85, 0},
       {0, 0, 0x85, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][]  |
     * |[]      |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x86, 0x86, 0x86, 0},
       {0x86, 0, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      2,
      1,
      2},
     },
    /*
     * ----------
     * |        |
     * |[][][][]|
     * |        |
     * |        |
     * ----------
     */
    {
     {
      {
       {0, 0, 0, 0},
       {0x87, 0x87, 0x87, 0x87},
       {0, 0, 0, 0},
       {0, 0, 0, 0},
       },
      0,
      3,
      1,
      1,
      }
     },
};

/**
 *
 * 90" clockwise rotation
 *
 * y axis -> x axis
 * x axis -> reverse of x axis
 */
static void
rotate_block(struct block *to, struct block *from)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            to->show[i][j] = from->show[4 - 1 - j][i];
        }
    }

    to->x_min = 4 - 1 - from->y_max;
    to->x_max = 4 - 1 - from->y_min;
    to->y_min = from->x_min;
    to->y_max = from->x_max;
}

void
ct_blocks_init()
{
    int i, j;

    for (i = 0; i < 7; i++) {
        for (j = 1; j < 4; j++) {
            rotate_block(&blocks[i][j], &blocks[i][j - 1]);
        }
    }

}

struct block *
get_block(int type, int shape)
{
    return &blocks[type % 7][shape % 4];
}

static int
ct_rand()
{
    static bool is_seeded = false;

    if (!is_seeded) {
        srand(((long)(time(0) * getpid())));
        is_seeded = true;
    }
    return rand();
}

struct block *
rand_block()
{
    cur_shape = ct_rand();
    cur_type = ct_rand();
    return get_block(cur_type, cur_shape);
}
