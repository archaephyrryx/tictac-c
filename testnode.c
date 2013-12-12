#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "err.h"

int main(int argc, const char *argv[])
{
    bNode *root;
    int infocount;

    if (argc > 0)
	progname = argv[0];

    infocount = loadinfo(argv[1]);

    root = bNodeAlloc(0, 0);
    addMissing(root);

    printf("%d\n", infocount);
    return EXIT_SUCCESS;
}
