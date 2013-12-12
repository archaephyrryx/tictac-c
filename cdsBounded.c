#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "game.h"
#include "err.h"
#include "heuristics.h"
#include "cdsBounded.h"

#define MAXLLEN 100

static int start[3];

void cdsBoundedInit(const char *confpath)
{
    FILE *conffile;
    char buf[MAXLLEN];
    int n;

    if (confpath == NULL)
    	fatal("cdsBoundedInit: null confpath");
    if ((conffile = fopen(confpath, "r")) == NULL)
    	efatal("cdsBoundedInit: fopen: %s", confpath);

    /*
     * Uniform conffiles should contain parameters for strategy functions,
     * which should be consist of three scalar weights, a threshold, and
     * three relative weights, all on one line
     */
    if (fgets(buf, sizeof(buf), conffile) == NULL)
    	fatal("cdsBoundedInit: %s: fgets() EOF", confpath);

    n = sscanf(buf, "%d %d %d", &start[0], &start[1], &start[2]);
    if (n != 3)
    	fatal("cdsBoundedInit: %s: expected 3 parameters: got %d", confpath, n);

    (void) fclose(conffile);
}

int cdsBounded(bNode *root, int depth)
{
    int dstart;
    int cstart;
    int sstart;
    int d;
    int c;
    int s;

    dstart = start[0];
    cstart = start[1];
    sstart = start[2];
    d = (root->depth >= dstart);
    c = (root->depth >= cstart);
    s = (root->depth >= sstart);
    
    if (d && !(c || s))
        return dominating(root, depth);
    if (c && !(d || s))
        return calculating(root, depth);
    if (s && !(d || c))
        return selfish(root, depth);

    if (d && c && !(s)) {
        if (dstart < cstart)
            return calculating(root, depth);
        if (dstart > cstart)
            return dominating(root, depth);
    }
    if (d && s && !(c)) {
        if (dstart < sstart)
            return selfish(root, depth);
        if (dstart > sstart)
            return dominating(root, depth);
    }
    if (c && s && !(d)) {
        if (cstart < sstart)
            return selfish(root, depth);
        if (cstart > sstart)
            return calculating(root, depth);
    }
    if (c && s && d) {
        if (cstart < dstart && sstart < dstart)
	    return dominating(root, depth);
        if (cstart < sstart && dstart < sstart)
	    return selfish(root, depth);
        if (dstart < cstart && sstart < cstart)
	    return calculating(root, depth);
    }
    return 0;
}
