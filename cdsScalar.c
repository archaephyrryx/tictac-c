#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "err.h"
#include "heuristics.h"
#include "cdsScalar.h"

#define MAXLLEN 100

static int scale[3];

void cdsScalarInit(const char *confpath)
{
    FILE *conffile;
    char buf[MAXLLEN];
    int n;

    if (confpath == NULL)
    	fatal("cdsScalarInit: null confpath");
    if ((conffile = fopen(confpath, "r")) == NULL)
    	efatal("cdsScalarInit: fopen: %s", confpath);

    /*
     * Uniform conffiles should contain parameters for strategy functions,
     * which should be consist of three scalar weights, a threshold, and
     * three relative weights, all on one line
     */
    if (fgets(buf, sizeof(buf), conffile) == NULL)
    	fatal("cdsScalarInit: %s: fgets() EOF", confpath);

    n = sscanf(buf, "%d %d %d %d %d %d %d",
	       &scale[0], &scale[1], &scale[2],
	       &threshold,
	       &weight[0], &weight[1], &weight[2]);
    if (n != 7)
    	fatal("cdsSigmaInit: %s: expected 7 parameters: got %d", confpath, n);

    (void) fclose(conffile);
}

int cdsScalar(bNode *root, int depth)
{
    int dweight;
    int cweight;
    int sweight;
    int heuristic;

    dweight = scale[0];
    cweight = scale[1];
    sweight = scale[2];

    heuristic = random() % (dweight + cweight + sweight); 

    return (heuristic < dweight) ? dominating(root, depth) :
	   (heuristic < dweight + sweight) ? selfish(root, depth) :
					     calculating(root, depth); 
}
