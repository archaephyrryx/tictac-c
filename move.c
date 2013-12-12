#include "engine.h"
#include "move.h"
int isValid(subBoard state, int index) {
  return (state[index] == 0);
}

void applyMove(int index, board state, int player) {
  state[(index/9)][index%9] = player;
}
