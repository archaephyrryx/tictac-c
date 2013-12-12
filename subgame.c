#include <stdio.h>
#include "engine.h"
#include "subgame.h"

void makeTermTable(Table_T termtable)
{
  subBoard root = subboardalloc(0);
  terminalTable(termtable, root); 
}

terminal *terminalTable(Table_T termtable, subBoard state)
{
  int player;
  int i;
  int w;
  int *canon = canonBoard(state);
  terminal *terms;

  if ((terms = Table_get(termtable, canon)) != 0) {
    free(canon);
    return terms;
  }

  terms = (terminal *) malloc(sizeof(terminal));

  if ((w = win(state)) != 0) {
    terms->max = (w == 1) ? 1 : 0;
    terms->min = (w == -1) ? 1 : 0;
  } else if (countOpen(state) == 0) {
    terms->max = 0;
    terms->min = 0;
  } else {
    int childState[9];
    memcpy(childState, state, 9*sizeof(int));

    for (i = 0; i < 9; ++i) {
      if (state[i] == 0) {
	for (player = 0; player < 2; ++player) {
	  childState[i] = (2*player) - 1;
	  terminal *childterm = terminalTable(termtable, childState);
	  terms->max += childterm->max;
	  terms->min += childterm->min;
	}
      }
    }
  }
  Table_put(termtable, canon, terms);
  return terms;
} 
 
int stakes(subBoard state, int i, int j, int k, int player) {
  int a = state[i];
  int b = state[j];
  int c = state[k];

  /* Test */ 
  if ((a * b + b * c + c * a >= 0) && (a + b + c) * player >= 0) {
    return (a + b + c);
  }
  return -1;
}
 
int winWays(subBoard state, int player, int owned)
{
  int count = 0;

  /* Counts the number of win vectors that a player owns a certain number of
   * positions along */
  count += (stakes(state, 0, 4, 8, player) == owned);
  count += (stakes(state, 2, 4, 6, player) == owned);
  count += (stakes(state, 0, 3, 6, player) == owned);
  count += (stakes(state, 1, 4, 7, player) == owned);
  count += (stakes(state, 2, 5, 8, player) == owned);
  count += (stakes(state, 0, 1, 2, player) == owned);
  count += (stakes(state, 3, 4, 5, player) == owned);
  count += (stakes(state, 6, 7, 8, player) == owned);

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

subBoard metaState(board state)
{
  int i;
  subBoard metastate = subboardalloc(0);

  /* produces a sub-board containing the win-states of the respective sub-boards
   * on the large board */
  for (i = 0; i < 9; ++i) {
    metastate[i] = win(state[i]);
  }
  return metastate;
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
