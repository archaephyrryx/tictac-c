#include "engine.h"
#include "heuristics.h"
#include "stratagy.h"

int stratagizer(bNode *root, int depth)
{
    int mean[3] = {27, 0, 54};
    int stddev[3] = {13, 13, 13};
    int dweight = gauss(root->depth, mean[0], stddev[0]);
    int cweight = gauss(root->depth, mean[1], stddev[1]);
    int sweight = gauss(root->depth, mean[2], stddev[2]);
    int heuristic = rand() % (dweight + cweight + sweight); 

    return (heuristic < dweight) ? dominating(root, depth) :
           (heuristic < dweight + sweight) ? selfish(root, depth) :
	                                     calculating(root, depth); 
}
