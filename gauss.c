#include <stdio.h>
#include "engine.h"
#include "heuristics.h"
#include "gauss.h"
#define MAXLLEN 100

int threshold;
int weight[3];

static int mean[3];
static int stddev[3];
static int initialized;

static void initgauss(void)
{
  const char *confpath = getenv("CONFPATH");
  FILE *conffile;
  char buf[MAXLLEN];
  int n;

  if (confpath == NULL)
    exit(EXIT_FAILURE);
  conffile = fopen(confpath, "r");
  if (conffile == NULL)
    exit(EXIT_FAILURE);

  /*
   * Gauss conffiles should contain parameters for strategy functions, which should
   * be consist of three mean/stddev pairs, a threshold, and three relative
   * weights, all on one line
   */
  if (fgets(buf, sizeof(buf), conffile) == NULL)
    exit(EXIT_FAILURE);

  n = sscanf(buf, "%d %d %d %d %d %d %d %d %d %d",
	      &mean[0], &stddev[0],
	      &mean[1], &stddev[1],
	      &mean[2], &stddev[2],
	      &threshold,
	      &weight[0],
	      &weight[1],
	      &weight[2]);
  if (n != 10)
    exit(EXIT_FAILURE);

  (void) fclose(conffile);
}

static double gauss(int x, int mean, int stddev)
{
  return (1 / (stddev * sqrt(2 * PI)) *
 	  exp( -0.5 * pow(MODDIST(x, mean), 2) / pow(stddev, 2)) );
}

int cdsSigma(bNode *root, int depth)
{
  int dweight;
  int cweight;
  int sweight;
  int heuristic;

  if (!initialized) {
    initgauss();
    initialized = 1;
  }

  dweight = (int) (10000 * gauss(root->depth, mean[0], stddev[0]));
  cweight = (int) (10000 * gauss(root->depth, mean[1], stddev[1]));
  sweight = (int) (10000 * gauss(root->depth, mean[2], stddev[2]));

  heuristic = rand() % (dweight + cweight + sweight); 

  return (heuristic < dweight) ? dominating(root, depth) :
	 (heuristic < dweight + sweight) ? selfish(root, depth) :
					   calculating(root, depth); 
}
