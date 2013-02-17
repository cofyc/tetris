#ifndef CT_DEBUG_H
#define CT_DEBUG_H

#include "tetris.h"

extern void ct_debug_init();
extern void ct_debug_log(const char *fmt, ...);
extern void ct_debug_enable();
extern void ct_debug_disable();

#endif
