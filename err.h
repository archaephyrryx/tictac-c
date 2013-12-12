#ifndef ERR_H
#define ERR_H

#include <stdarg.h>

extern const char *progname;
extern void efatal (const char *, ...) __attribute__ ((noreturn));
extern void fatal (const char *, ...) __attribute__ ((noreturn));

#endif
