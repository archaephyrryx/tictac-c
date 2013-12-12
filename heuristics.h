#ifndef HEURISITCS_H
#define HEURISTICS_H

#include <limits.h>

#include "node.h"

/* Minimaxing */
#define POSINFTY INT_MAX
#define NEGINFTY INT_MIN
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
#define MIN(x,y) (((x) <= (y)) ? (x) : (y))

#define SIGN(x) ((x < 0) ? -1 : (x > 0))
#define ABS(x) ((x < 0) ? -1 * x : x)
#define PI (3.141592653589793)

extern int threshold;
extern int weight[3];

extern int selfish(bNode *, int);
extern int calculating(bNode *, int);
extern int dominating(bNode *, int);

#endif
