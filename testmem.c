#include <stdlib.h>
#include <stdio.h>

#include "mem.h"
#include "err.h"

int main(int argc, const char *argv[])
{
    void *p;

    if (argc > 0)
	progname = argv[0];

    p = xmalloc(0);
    p = xrealloc(p, 10);
    xfree(p);

    p = xcalloc(1, 0);
    p = xrealloc(p, 10);
    xfree(p);

    p = xcalloc(0, 1);
    p = xrealloc(p, 10);
    xfree(p);

    return EXIT_SUCCESS;
}
