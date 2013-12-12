#include "engine.h"
#include "canon.h"

void rotate(subBoard state)
{
  /* Rotates the entire board anti-clockwise about the center point */
  int i;
  int j;
  int image[9] = {0,0,0,0,0,0,0,0,0};
  
  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      image[3 * i + j] = state[3 * j + 2 - i];
    }
  }
  memcpy(state, image, 9 * sizeof(int));
}

void reflect(subBoard state)
{
  /* Rotates the entire board about the y-axis, swapping columns but not rows */
  int i;
  int j;
  int image[9];
 
  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      image[3 * i + j] = state[3 * i + 2 - j];
    }
  }
  memcpy(state, image, 9 * sizeof(int));
}

subBoard canonBoard(subBoard state)
{
  morphism m;
  subBoard canon = subboardalloc(state);
  
  /*
   * We cycle through all possible rotations and reflections of the board, and
   * at each stage evaluate whether the current board is "less than" the
   * smallest board we have seen. If it is, we overwrite the working version of
   * the canonical board. At the end, we overwrite state to be the final
   * evaluation of the canonical board.
   *
   * Note that the morphism contains the rotation and reflection that INVERT the
   * canonical morphism, i.e. revert to the current reference frame, not the
   * rotation and reflection that ARRIVES at the canonical morphism. Note also
   * that reflections are applied first, and then rotations
   */
  for (m.reflection = 0; m.reflection < 2; ++m.reflection) {
    for (m.rotation = 3; m.rotation >= 0; --m.rotation) {
      rotate(state);
      if (boardcmp(state, canon) < 0) { 
        subboardcpy(canon, state);
      }
    }
    reflect(state);
  }
  return canon;
}

unsigned boardhash(const void *x)
{
  int i;
  unsigned hash = 0;
  subBoard state = (subBoard) x;

  for (i = 0; i < 9; ++i) {
    hash = hash*3 + state[i];
  }
  return hash;
}

int boardcmp(const void *x, const void *y)
{
  return memcmp(x, y, 9 * sizeof(int));
}
