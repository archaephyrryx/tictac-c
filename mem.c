#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "err.h"

void xfree(void *p)
{
    free(p);
}

void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (p)
    	return p;
    efatal("malloc");
}

void *xcalloc(size_t num, size_t size)
{
    void *p = calloc(num, size);
    if (p)
    	return p;
    efatal("calloc");
}

void *xrealloc(void *p, size_t size)
{
    p = realloc(p, size);
    if (p)
    	return p;
    efatal("realloc");
}

char *xstrdup(const char *s)
{
    char *p = strdup(s);
    if (p)
    	return p;
    efatal("strdup");
}

char *xstrndup(const char *s, size_t size)
{
    char *p = strndup(s, size);
    if (p)
    	return p;
    efatal("strndup");
}
