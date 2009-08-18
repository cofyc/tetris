#ifndef CTETRIS_H
#define CTETRIS_H

/* ISO C99 */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* Ncurses */
#include <ncurses.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#define CTETRIS_DEBUG 1

typedef unsigned int uint;
typedef unsigned char uchar;

/* Helper Functions */
extern void die(const char *err, ...);

extern void usage(const char *err);

extern int error(const char *err, ...);

extern void warning(const char *err, ...);

/* Alloc Functions */
extern void *xmalloc(size_t size);

extern void *xrealloc(void *ptr, size_t size);

extern void *xcalloc(size_t nmemb, size_t size);

extern char *xstrndup(const char *str, size_t len);

#endif
