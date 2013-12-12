#include "engine.h"
#include "state.h"

void boardcpy(board target, board source) {
  memcpy(target, source, 81 * sizeof(int));
}

void subboardcpy(subBoard target, subBoard source) {
  memcpy(target, source, 9 * sizeof(int));
}

board boardalloc(board clone)
{
  board state = (board) calloc(81, sizeof(int));
  if (clone)
    boardcpy(state, clone);
  return state;
}

subBoard subboardalloc(subBoard clone)
{
  subBoard state = (subBoard) calloc(9, sizeof(int));
  if (clone)
    subboardcpy(state, clone);
  return state;
}
