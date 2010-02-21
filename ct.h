#ifndef CT_H
#define CT_H

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
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

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

static inline void
mylog(const char *fmt, ...)
{
    static FILE *logfile = NULL;

    char msg[1024];

    va_list params;

    va_start(params, fmt);

    if (!logfile) {
        logfile = fopen("log.txt", "a+");
    }

    vsnprintf(msg, sizeof(msg), fmt, params);
    fprintf(logfile, "%s\n", msg);

    fflush(logfile);            /* exit with some unexpected error, string in buffer may not be flushed out */
    va_end(params);
}

#endif
