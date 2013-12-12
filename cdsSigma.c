#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "err.h"
#include "heuristics.h"
#include "cdsSigma.h"

#define MAXLLEN 100

static int mean[3];
static int stddev[3];
static int scale[3];

static int moddist(int x, int y)
{
    register int d = (x - y);
    register int ad = d >= 0 ? d : -d;
    register int left = ad % 81;
    register int right = 81 - left;
    return MIN(left, right);
}

void cdsSigmaInit(const char *confpath)
{
    FILE *conffile;
    char buf[MAXLLEN];
    int n;

    if (confpath == NULL)
    	fatal("cdsSigmaInit: null confpath");
    if ((conffile = fopen(confpath, "r")) == NULL)
    	efatal("cdsSigmaInit: fopen: %s", confpath);

    /*
     * Gauss conffiles should contain parameters for strategy functions,
     * which should be consist of three mean/stddev pairs, a threshold, and
     * three relative weights, all on one line
     */
    if (fgets(buf, sizeof(buf), conffile) == NULL)
    	fatal("cdsSigmaInit: %s: fgets() EOF", confpath);

    n = sscanf(buf, "%d %d %d %d %d %d %d %d %d",
	       &mean[0], &stddev[0], &scale[0],
	       &mean[1], &stddev[1], &scale[1],
	       &mean[2], &stddev[2], &scale[2]);
    if (n != 9)
    	fatal("cdsSigmaInit: %s: expected 9 parameters: got %d", confpath, n);

    (void) fclose(conffile);
}

static double gauss(int x, int mean, int stddev)
{
    return (1 / (stddev * sqrt(2 * PI)) *
	    exp( -0.5 * pow(moddist(x, mean), 2) / pow(stddev, 2)) );
}

int cdsSigma(bNode *root, int depth)
{
    int dweight;
    int cweight;
    int sweight;
    int heuristic;

    dweight = (int) (scale[0] * gauss(root->depth, mean[0], stddev[0]));
    cweight = (int) (scale[1] * gauss(root->depth, mean[1], stddev[1]));
    sweight = (int) (scale[2] * gauss(root->depth, mean[2], stddev[2]));

    heuristic = random() % (dweight + cweight + sweight); 

    return (heuristic < dweight) ? dominating(root, depth) :
	   (heuristic < dweight + sweight) ? selfish(root, depth) :
					     calculating(root, depth); 
}
