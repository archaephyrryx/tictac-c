#include <stdlib.h>
#include <stdio.h>

#include "err.h"

const char *progname;

void efatal (const char *why, ...)
{
    va_list ap;

    if (progname)
    	fprintf(stderr, "%s: ", progname);

    va_start(ap, why);
    vfprintf(stderr, why, ap);
    va_end(ap);

    perror("");
    exit(EXIT_FAILURE);
}

void fatal (const char *why, ...)
{
    va_list ap;

    if (progname)
    	fprintf(stderr, "%s: ", progname);

    va_start(ap, why);
    vfprintf(stderr, why, ap);
    va_end(ap);
    fputc('\n', stderr);

    exit(EXIT_FAILURE);
}

