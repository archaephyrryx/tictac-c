#include <stdlib.h>
#include <stdio.h>

#include "err.h"
#include "fullRandom.h"

#define MAXLLEN 100

static int autochoice;

void fullRandomInit(const char *confpath)
{
    FILE *conffile;
    char buf[MAXLLEN];

    if (confpath == NULL)
    	fatal("initrand: null confpath");
    if ((conffile = fopen(confpath, "r")) == NULL)
    	efatal("initrand: fopen: %s", confpath);

    /*
     * Random conffiles contain a single integer,
     * a favored move to make whenever possible
     */
    if (fgets(buf, sizeof(buf), conffile) == NULL)
    	fatal("initrand: %s: fgets() EOF", confpath);

    if (sscanf(buf, "%d", &autochoice) != 1)
    	fatal("initrand: %s: autochoice expected", confpath);

    (void) fclose(conffile);
}

static int randomMove(bNode *root)
{
    mNode *move;
    int n = 0;
    int loc = -1;

    for (move = root->moves; move; move = move->next) {
	int which = random() % ++n;

	if (which == 0)
	    loc = move->loc;
    }

    if (loc < 0) 
    	abort();
    return loc;
}

int fullRandom(bNode *root, int depth)
{
    static uint64_t one = 1;
    register int i = autochoice >> 6;
    register int j = autochoice & 0x3f;
    uint64_t bit = one << j;

    addMissing(root);

    if (root->movemask[i] & bit)
	return autochoice;
    return randomMove(root);
}
