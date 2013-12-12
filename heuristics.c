#include "engine.h"
#include "heuristics.h"
#include "subgame.h"
#include "super.h"
#include "node.h"
#include "gauss.h"

int null(bNode *root) {
  return (root->depth % 2) ? POSINFTY : NEGINFTY;
}

int terminalDifference(subBoard state) {
  terminal *terms = Table_get(termtable, state);
  return (terms->max + terms->min) ? (100 * (terms->max - terms->min))/(terms->max + terms->min) : 0;
}

int advantage(bNode *root)
{
  int advantage = 0;
  int i;

  /* ====Metastate Advantage====
   * Returns the "metastate advantage" of a board: the metastate advantage is
   * evaluated as the difference between the number of boards owned by the
   * maximizer and the number of boards owned by the minimizer
   */

  for (i = 0; i < 9; ++i)
    advantage += root->meta[i];

  return advantage;
}

int terminus(bNode *root)
{
  int i;
  int advantage = 0;
  int meta[9];

  /* ====Terminal Differential Advantage====
   * Returns the "terminal differential advantage" of a board:
   * the terminal differential advantage is calculated by evaluating
   * each individual sub-board as a single game of regular tic-tac-toe, with
   * complete freedom to move (either player can play at any depth), at which
   * point the difference between the maxima and the minima is taken. The sign
   * of each terminal difference is taken as the state of the meta-board. The
   * terminal difference of the meta-board is then evaluated.
   */

  for (i = 0; i < 9; ++i) {
    meta[i] = terminalDifference(root->state[i]);
    if (ABS(meta[i]) >= threshold)
      meta[i] = SIGN(meta[i]);
    else
      meta[i] = 0;
  }

  for (i = 3; i >= 0; --i) {
    advantage += winWays(meta, 1, i);
    advantage -= winWays(meta, -1, i);
    advantage *= i ? weight[i-1] : 1;
  }
  return advantage;
}

int ownership(bNode *root)
{
  int advantage = 0;
  int i;
  int j;

  /* ==== Board Ownership Advantage ===
   * Returns the "board ownership advantage" of a descendent over an
   * ancestor; the terminal differential advantage is calculated as the sum of
   * the ownership advantages of each sub-board. The ownership advantage is the
   * difference between the weighted sums of n-filled win-vector ownership of a
   * board; the weight indicates how many vectors underfilled by one are equal
   * to one filled vector.
   */
  for (i = 3; i >= 0; --i) {
    for (j = 0; j < 9; ++j) {
      advantage += winWays(root->state[j], 1, i);
      advantage -= winWays(root->state[j], -1, i);
    }
    advantage *= i ? weight[i-1] : 1;
  }
  return advantage;
}

int selfish(bNode *root, int depth)
{
  /*
   * The Selfish Evaluator
   * ===FUNCTION===
   * Metastate advantage
   * ===DESCRIPTION===
   * Greedily steals wins that are advantageous in short-term, zealously blocks
   * opponent wins that aren't advantageous in thes short-term
   * ===BEHAVIOR===
   * When depth == n: Follow the path that has the highest advantage
   * at depth n. Branches are alpha-beta pruned with the metastate
   * advantage heuristic. Once depth n is reached, the function is no
   * longer evaluated. For the degenerate case of n=0, the heuristic
   * function is evaluated on the child states without any further
   * lookups or constructions.
   */
  root->hValue = alphabeta(NEGINFTY, POSINFTY, depth, root, advantage);
  return randomOptimal(root);
}

int calculating(bNode *root, int depth)
{
  /* The Calculating Evaluator
   * ===FUNCTION===
   * Terminal Differential Advantage
   * ===DESCRIPTION===
   * Greedily selects moves that shift the terminal difference favorably,
   * zealously prevents opponent from taking move that shift the terminal
   * difference unfavorably
   * ===BEHAVIOR===
   * When depth == n:
   * Follow the path that has the best terminal advantage at depth n, using
   * alpha-beta pruning of minimaxing.
   */
  root->hValue = alphabeta(NEGINFTY, POSINFTY, depth, root, terminus);
  return randomOptimal(root);
}

int dominating(bNode *root, int depth)
{
  /* The Dominating Evaluator
   * ===FUNCTION===
   * Board ownership advantage
   * ===DESCRIPTION===
   * Maximizes the weighted sum of the n-filled win vectors for each board.
   * ===BEHAVIOR===
   * When depth == n:
   * Follow the path that increases ability to win a board, calculated using
   * alpha-beta pruning and minimaxing.
   * For the degenerate case of n=0, the heuristic function is evaluated on the child states without any
   * recursive lookups or constructions.
   */
  root->hValue = alphabeta(NEGINFTY, POSINFTY, depth, root, ownership);
  return randomOptimal(root);
}

int nullius(bNode *root, int depth)
{
  addMissing(root);
  return randomMove(root);
}



int randomizer(bNode *root, int depth)
{
    int heuristic = rand() % 3;

    return (heuristic == 0) ? dominating(root, depth) :
           (heuristic == 1) ? selfish(root, depth) :
	                      calculating(root, depth); 
}

