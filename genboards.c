#include "engine.h"
#include "subgame.h"
#include "node.h"
#include "move.h"
#include "state.h"
#include "canon.h"

int main(int argc, char *argv[])
{
  Table_T termtable = Table_new(SUBBOARDS, boardcmp, boardhash);
  subBoard root = subboardalloc(0);
  terminalTable(termtable, root);
  return 0;
}

terminal *terminalTable(Table_T termtable, subBoard state)
{
  int player;
  int i;
  int w;
  terminal *terms;

  if ((terms = Table_get(termtable, state)) != 0) {
    return terms;
  }
  state = subboardalloc(state);

  terms = (terminal *) calloc(1, sizeof(terminal));

  if ((w = win(state)) != 0) {
    terms->max = (w == 1);
    terms->min = (w == -1);
  } else if (countOpen(state) == 0) {
    terms->max = 0;
    terms->min = 0;
  } else {
    int childState[9];
    memcpy(childState, state, 9*sizeof(int));

    for (i = 0; i < 9; ++i) {
      if (childState[i] == 0) {
	for (player = 0; player < 2; ++player) {
	  childState[i] = (2*player) - 1;
	  terminal *childterm = terminalTable(termtable, childState);
	  terms->max += childterm->max;
	  terms->min += childterm->min;
	}
	childState[i] = 0;
      }
    }
  }
  Table_put(termtable, state, terms);
  printf("%d %d", terms->max, terms->min);
  for (i = 0; i < 9; ++i) {
    printf(" %d", state[i]);
  }
  putchar('\n');
  return terms;
} 
