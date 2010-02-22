#include "ct.h"

void
usage(const char *err)
{
    fprintf(stderr, "usage: %s\n", err);
    exit(129);
}

static void
report(const char *prefix, const char *err, va_list params)
{
    char msg[1024];

    vsnprintf(msg, sizeof(msg), err, params);
    fprintf(stderr, "%s%s\n", prefix, msg);
}

void
die(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    report("fatal: ", err, params);
    va_end(params);
    exit(128);
}

int
error(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    report("error: ", err, params);
    va_end(params);
    return -1;
}

void
warning(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    report("warning: ", err, params);
    va_end(params);
}
