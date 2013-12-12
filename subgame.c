#include <stdio.h>
#include "engine.h"
#include "subgame.h"

static int paths[54];
 
void initstakes(void) {
  int a;
  int b;
  int c;
  int p;

  for (p = -1; p < 2; p += 2) {
    for (a = -1; a < 2; ++a) {
      for (b = -1; b < 2; ++b) {
	for (c = -1; c < 2; ++c) {
	  int i = 27*(p+1)/2 + 9*(a+1) + 3*(b+1) + (c+1);
	  /*
	   * For three positions along a win-vector, the sum of the pair-wise
	   * products of their values is negative if two positions have opposite
	   * values (+1 and -1). If all values are 0 or a single player value,
	   * then this evaluation is greater than or equal to 0.
	   */
	  if ((a * b + b * c + c * a >= 0) && (p * (a + b + c) >= 0)) {
	    paths[i] = p * (a + b + c);
	  } else {
	    paths[i] = -1;
	  }
	}
      }
    }
  }
}
 
int countOpen(subBoard state) {
  int i;
  int count = 0;

  for (i = 0; i < 9; ++i) {
    if (state[i] == 0) {
      ++count;
    }
  }
  return count;
}


void makeTermTable(Table_T termtable)
{
  char buf[MAXLINE];
  FILE *allboards = fopen("allboards.dat", "r");
  if (allboards == NULL)
    exit(EXIT_FAILURE);

  while (fgets(buf, sizeof(buf), allboards)) {
    subBoard board = subboardalloc(0);
    terminal *terms = (terminal *) malloc(sizeof(terminal));

    if (sscanf(buf, "%d %d %d %d %d %d %d %d %d %d %d", &terms->max, &terms->min,
		     &board[0], &board[1], &board[2], &board[3], &board[4],
		     &board[5], &board[6], &board[7], &board[8]) != 11) {
      exit(EXIT_FAILURE);
    }
    Table_put(termtable, board, terms);
  }
}

#if 0
static int stakes(subBoard state, int i, int j, int k, int player) {
  int a = state[i];
  int b = state[j];
  int c = state[k];

  /* Test */ 
  if ((a * b + b * c + c * a >= 0) && (a + b + c) * player >= 0) {
    return (a + b + c);
  }
  return -1;
}
#endif
 
int winWays(subBoard state, int player, int owned)
{
  static int _pmap[] = {0, 0, 27};
  static int _amap[] = {0, 9, 18};
  static int _bmap[] = {0, 3, 6};
  register int _p = _pmap[player + 1];
  register int count = 0;

#define docount(i, j, k) \
      do { \
      	register int _i = _amap[state[i] + 1]; \
      	_i += _bmap[state[j] + 1]; \
      	_i += state[k] + 1; \
	count += (paths[_p + _i] == owned); \
      } while (0)

  /* Counts the number of win vectors that a player owns a certain number of
   * positions along */
  docount(0, 1, 2);
  docount(0, 3, 6);
  docount(0, 4, 8);
  docount(1, 4, 7);
  docount(2, 4, 6);
  docount(2, 5, 8);
  docount(3, 4, 5);
  docount(6, 7, 8);

  return count;
}

int win(subBoard state)
{
  int i;

  /*
   * We cycle through all winning lines on the board. As the values at those
   * positions are either 1, 0, or -1, any potential winning triplet must hold
   * the same values. a*(a+b+c) == 3 iff a, b, c are all equal and non-zero.
   */

  /* Diagonal winner */
  if (state[4] * (state[0] + state[4] + state[8]) == 3 ||
      state[4] * (state[2] + state[4] + state[6]) == 3)
    return state[4];

  for (i = 0; i < 3; ++i) { 
    int sum;
    int j;

    /* Vertical winner */
    for (sum = j = 0; j < 3; ++j)
      sum += state[3*j+i];
    if (state[i]*sum == 3)
      return state[i];

    /* Horizontal winner */
    for (sum = j = 0; j < 3; ++j)
      sum += state[3*i+j];
    if (state[3*i]*sum == 3)
      return state[3*i];
  }
  return 0;
}

subBoard metaState(board state, int fillValue)
{
  int i;
  subBoard metastate = subboardalloc(0);

  /* produces a sub-board containing the win-states of the respective sub-boards
   * on the large board. If a board is completely filled, then the value is the
   * argument fillValue. This is to distinguish between evaluations of whether
   * the game has been won, and evaluations of whether a board is still open for
   * moves. */
  for (i = 0; i < 9; ++i) {
    if ((metastate[i] = win(state[i])) == 0 &&
        countOpen(state[i]) == 0 && fillValue != 0) {
    metastate[i] = fillValue;
    }
  }
  return metastate;
}

