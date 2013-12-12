#include "engine.h"
#include "heuristics.h"
#include "stratagy.h"

int gauss(int x, int mean, int stddev)
{
  return (int) ((1000000 / (stddev * sqrtf(2 * PI))) * expf(-1 * (powf((float)
  MODDIST(x, mean), (float) 2))/(2 * powf((float) stddev, (float) 2))));
}

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

int toetactician(bNode *root, int depth)
{
    int mean[3] = {27, 0, 54};
    int stddev[3] = {26, 26, 26};
    int dweight = gauss(root->depth, mean[0], stddev[0]);
    int cweight = gauss(root->depth, mean[1], stddev[1]);
    int sweight = gauss(root->depth, mean[2], stddev[2]);
    int heuristic = rand() % (dweight + cweight + sweight); 

    return (heuristic < dweight) ? dominating(root, depth) :
           (heuristic < dweight + sweight) ? selfish(root, depth) :
	                                     calculating(root, depth); 
}
