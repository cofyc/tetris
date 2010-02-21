#include "ct_display.h"

/**
 * lines -> Y (i)
 * colums -> X (j)
 *
 * (x, y) - x
 * |
 * y
 *
 *
 */

WINDOW *mainwin, *leftwin, *rightwin;

int score = 0;

#define CT_CELL_X   2
#define CT_CELL_Y   1
#define win_line 24
#define win_cols 12

uchar win_buffer[win_line][win_cols];
uchar win_bg[win_line][win_cols];

int
ct_display_init()
{
    leftwin = newwin(win_line * CT_CELL_Y + 1, win_cols * CT_CELL_X + 2, 0, 0);
    rightwin = newwin(win_line * CT_CELL_Y, win_cols * CT_CELL_X, 0, (win_cols + 2) * CT_CELL_X);
    return 0;
}

void
showCell(int y, int x)
{
    uchar cell = win_buffer[y][x];

    wattrset(leftwin, COLOR_PAIR(XCOLOR_OF(cell) % 8));

    if (XSTATUS_OF(cell)) {
        mvwaddch(leftwin, y * CT_CELL_Y, x * CT_CELL_X, '[');
        mvwaddch(leftwin, y * CT_CELL_Y, x * CT_CELL_X + 1, ']');
    } else {
        mvwaddch(leftwin, y * CT_CELL_Y, x * CT_CELL_X, ' ');
        mvwaddch(leftwin, y * CT_CELL_Y, x * CT_CELL_X + 1, ' ');
    }
}

void
refreshMain(int top_y, int btm_y, int lft_x, int rgt_x)
{
    int i, j;

    struct block *b = cur_b;

    // main block
    wattrset(leftwin, COLOR_PAIR(7 % 8));
    for (i = win_line; i < win_line + 1; i++) {
        for (j = 0; j < win_cols + 1; j++) {
            mvwaddch(leftwin, i * CT_CELL_Y, j * CT_CELL_X, '[');
            mvwaddch(leftwin, i * CT_CELL_Y, j * CT_CELL_X + 1, ']');
        }
    }
    for (i = 0; i < win_line + 1; i++) {
        for (j = win_cols; j < win_cols + 1; j++) {
            mvwaddch(leftwin, i * CT_CELL_Y, j * CT_CELL_X, '[');
            mvwaddch(leftwin, i * CT_CELL_Y, j * CT_CELL_X + 1, ']');
        }
    }
    wrefresh(leftwin);

    // cur bg
    for (i = 0; i < win_line; i++) {
        for (j = 0; j < win_cols; j++) {
            win_buffer[i][j] = win_bg[i][j];
        }
    }

    // cur block
    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(cur_b->show[i][j])) {
                win_buffer[cur_y + i][cur_x + j] = cur_b->show[i][j];
            }
        }
    }

    // show
    for (i = top_y; i <= btm_y; i++) {
        for (j = lft_x; j <= rgt_x; j++) {
            showCell(i, j);
        }
    }

    wrefresh(leftwin);

    //
//    updateScore();
}

void
setBlock(int y, int x, struct block *b)
{
    int i, j;

    if (y <= 0) {
        // lose game
        exit(-1);
    }

    for (i = b->y_min; i <= b->y_max; i++) {
        for (j = b->x_min; j <= b->x_max; j++) {
            if (XSTATUS_OF(b->show[i][j])) {
                win_bg[y + i][x + j] = b->show[i][j];
            }
        }
    }

    int erased_num = 0;
    int erased_lines[4] = { -1, -1, -1, -1 };
    for (i = y + b->y_min; i <= y + b->y_max; i++) {
        for (j = 0; j < win_cols; j++) {
            if (!XSTATUS_OF(win_bg[i][j])) {
                goto next;
            }
        }

        for (j = 0; j < win_cols; j++) {
            win_bg[i][j] = 0;
        }

        refreshMain(i, i, 0, win_cols - 1);
        erased_lines[erased_num++] = i;
      next:
        continue;
    }

    int _i;

    for (i = 0; i < erased_num; i++) {
        for (_i = erased_lines[i]; _i >= 0; _i--) {
            for (j = 0; j < win_cols; j++) {
                win_bg[_i][j] = win_bg[_i - 1][j];
                win_bg[_i - 1][j] = 0;
            }
        }
    }

    refreshMain(0, win_line, 0, win_cols - 1);

    switch (erased_num) {
        case 1:
            score++;
            break;
        case 2:
            score += 3;
            break;
        case 3:
            score += 7;
            break;
        case 4:
            score += 13;
            break;
        default:
            break;
    }

    updateScore();
}

bool
check_shape(struct block *b, int y, int x)
{
    // bg blocks
    if (x + b->x_min < 0) {
        return false;
    } else if (x + b->x_max > win_cols - 1) {
        return false;
    } else if (y + b->y_min < 0) {
        return false;
    } else if (y + b->y_max > win_line - 1) {
        return false;
    } else {
        int i, j;

        for (i = b->y_min; i <= b->y_max; i++) {
            for (j = b->x_min; j <= b->x_max; j++) {
                if (XSTATUS_OF(b->show[i][j]) && XSTATUS_OF(win_bg[y + i][x + j])) {
                    return false;
                }
            }
        }

        return true;
    }
}


void
loop_main()
{
    if (cur_step >= step) {
        if (cur_b) {
            if (check_shape(cur_b, cur_y + 1, cur_x)) {
                cur_y++;
                moveBlock(cur_y, cur_x, cur_b);
            } else {
                setBlock(cur_y, cur_x, cur_b);
                // new 
                cur_b = rand_block();
                cur_x = win_cols / 2 - 2;
                cur_y = 0 - cur_b->y_min;
                moveBlock(cur_y, cur_x, cur_b);
            }
        } else {
            cur_b = rand_block();
            cur_x = win_cols / 2 - 2;
            cur_y = 0 - cur_b->y_min;
            moveBlock(cur_y, cur_x, cur_b);
        }
        cur_step = 0;
    } else {
        cur_step++;
    }
}

void
moveBlock(int y, int x, struct block *b)
{
    static struct block *_b = NULL;

    static int _y, _x;

    if (!_b) {
        _b = b;
        _y = y;
        _x = x;
    } else {
        // doesn't change
        if (_b == b && _y == y && _x == x) {
            return;
        }
    }

    int top_y = (int)fmin(_y + _b->y_min, y + b->y_min);

    int btm_y = (int)fmax(_y + _b->y_max, y + b->y_max);

    int lft_x = (int)fmin(_x + _b->x_min, x + b->y_min);

    int rgt_x = (int)fmax(_x + _b->x_max, x + b->x_max);

    _b = b;
    _y = y;
    _x = x;

    refreshMain(top_y, btm_y, lft_x, rgt_x);
}


void
updateScore()
{
    mvwprintw(rightwin, 1, 1, "score: %d", score);
    mvwprintw(rightwin, 2, 1, "         ");
    mvwprintw(rightwin, 3, 1, "left: h");
    mvwprintw(rightwin, 4, 1, "right: l");
    mvwprintw(rightwin, 5, 1, "speed: j");
    mvwprintw(rightwin, 6, 1, "change: k");
    wrefresh(rightwin);
}
