#include "ct.h"
/*
 * Various trival helper wrappers around standard functions
 */

void *
xmalloc(size_t size)
{
    void *ret = malloc(size);

    if (!ret) {
        die("Out of memory, malloc failed");
    }

    return ret;
}

void *
xrealloc(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);

    if (!ret) {
        die("Out of memory, realloc failed");
    }

    return ret;
}

void *
xcalloc(size_t nmemb, size_t size)
{
    void *ret = calloc(nmemb, size);

    if (!ret) {
        die("Out of memory, realloc failed");
    }

    return ret;
}

char *
xstrndup(const char *str, size_t len)
{
    char *p;

    p = (char *)xmalloc(len + 1);
    memcpy(p, str, len);
    p[len] = '\0';

    return p;
}
