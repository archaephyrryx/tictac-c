#include <stdio.h>
#include "engine.h"
#include "heuristics.h"
#include "uniform.h"
#define MAXLLEN 100

static int scale[3];
static int initialized;

static void inituniform(void)
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
   * Uniform conffiles should contain parameters for strategy functions, which should
   * be consist of three scalar weights, a threshold, and three relative
   * weights, all on one line
   */
  if (fgets(buf, sizeof(buf), conffile) == NULL)
    exit(EXIT_FAILURE);

  n = sscanf(buf, "%d %d %d %d %d %d %d",
	      &scale[0], &scale[1], &scale[2],
	      &threshold,
	      &weight[0], &weight[1], &weight[2]);
  if (n != 7)
    exit(EXIT_FAILURE);

  (void) fclose(conffile);
}

int cdsScalar(bNode *root, int depth)
{
  int dweight;
  int cweight;
  int sweight;
  int heuristic;

  if (!initialized) {
    inituniform();
    initialized = 1;
  }

  dweight = scale[0];
  cweight = scale[1];
  sweight = scale[2];

  heuristic = rand() % (dweight + cweight + sweight); 

  return (heuristic < dweight) ? dominating(root, depth) :
	 (heuristic < dweight + sweight) ? selfish(root, depth) :
					   calculating(root, depth); 
}
