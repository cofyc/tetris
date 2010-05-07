#ifndef USAGE_H
#define USAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern void die(const char *err, ...);
extern void usage(const char *err);
extern int error(const char *err, ...);
extern void warning(const char *err, ...);

#endif
