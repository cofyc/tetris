#include "ct_debug.h"

#define CT_LOG_FILENAME "ct.log"

static bool ct_debug_is_on = false;
static FILE *ct_debug_logfile = NULL;

void
ct_debug_init()
{
    ct_debug_logfile = fopen(CT_LOG_FILENAME, "a+");

    if (!ct_debug_logfile) {
        die("ct_debug init failed.");
    }

    ct_debug_is_on = true;
}

void
ct_debug_log(const char *fmt, ...)
{
    if (!ct_debug_is_on) {
        return;
    }

    char msg[4096];

    va_list params;
    va_start(params, fmt);

    vsnprintf(msg, sizeof(msg), fmt, params);
    fprintf(ct_debug_logfile, "%s\n", msg);

    /* 
     * If program exits with unexpected error, 
     * string in buffer may not be flushed out.
     */
    fflush(ct_debug_logfile);

    va_end(params);
}
