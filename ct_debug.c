#include "ct_debug.h"

static FILE *ct_logfile = NULL;

void
ct_log(const char *fmt, ...)
{
    char msg[1024];

    va_list params;
    va_start(params, fmt);

    if (!ct_logfile) {
        ct_logfile = fopen("log.txt", "a+");
    }

    vsnprintf(msg, sizeof(msg), fmt, params);
    fprintf(ct_logfile, "%s\n", msg);

    /* 
     * if program exits with unexpected error, 
     * string in buffer may not be flushed out
     */
    fflush(ct_logfile);

    va_end(params);
}
