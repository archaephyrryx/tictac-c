#include <stdlib.h>

#include "game.h"
#include "node.h"
#include "heuristics.h"

int threshold;
int weight[3];

static int alphabeta(int a, int b, int d, bNode *r, int h(bNode *))
{
    /*
     * a: alpha; b: beta; d: max depth; r: root;
     * h: heuristic function; p: player
     */
    int p = PLAYDEP(r->depth);
    mNode *move;

    if (subinfo[r->boards[9]].win)
	return (r->value = 100 * h(r));

    if (r->depth == 81)
    	return r->value = 0;

    /* Max depth reached */
    if (d == -1)
      return (r->value = h(r));


    /* add any uncomputed moves, sort the moves by hValue */
    addMissing(r);
    sortMoves(r);

    /* implementation of minimaxing with alpha beta pruning */
    if (p == 1) {
	for (move = r->moves; move; move = move->next) {
	    int tmp = alphabeta(a, b, d-1, move->child, h);
	    a = MAX(a, tmp);
	    if (b <= a)
		break;
	}
	return (r->value = a);
    } if (p == -1) {
	for (move = r->moves; move; move = move->next) {
	    int tmp = alphabeta(a, b, d-1, move->child, h);

	    b = MIN(b, tmp);
	    if (b <= a)
		break;
	}
	return (r->value = b);
    } else {
	abort();
    }
}

static int terminalDifference(int hash)
{
    subinfo_t *info = &subinfo[hash];
    register int max = info->max;
    register int min = info->min;

    return (max + min) ? (100 * (max - min)) / (max + min) : 0;
}

static int advantage(bNode *root)
{
    /*
     * ====Metastate Advantage====
     * Returns the "metastate advantage" of a board: the metastate advantage
     * is evaluated as the difference between the number of boards owned by
     * the maximizer and the number of boards owned by the minimizer.
     */
    return subinfo[root->boards[9]].adv;
}

static int terminus(bNode *root)
{
    int i;
    int adv;
    int hash;

    /*
     * ====Terminal Differential Advantage====
     * Returns the "terminal differential advantage" of a board: the
     * terminal differential advantage is calculated by evaluating each
     * individual sub-board as a single game of regular tic-tac-toe, with
     * complete freedom to move (either player can play at any depth), at
     * which point the difference between the maxima and the minima is
     * taken. The sign of each terminal difference is taken as the state of
     * the meta-board. The terminal difference of the meta-board is then
     * evaluated.
     */

    for (hash = i = 0; i < 9; ++i) {
	int diff = terminalDifference(root->boards[i]);
	if (diff <= -threshold)
	    hash += pow3[i];
	else if (diff >= threshold)
	    hash += 2 * pow3[i];
    }

    for (adv = 0, i = 3; i >= 0; --i) {
	adv += subinfo[hash].ways[1][i];
	adv -= subinfo[hash].ways[0][i];
	adv *= i ? weight[i-1] : 1;
    }
    return adv;
}

static int ownership(bNode *root)
{
    int adv;
    int i;
    int j;

    /*
     * ==== Board Ownership Advantage ===
     * Returns the "board ownership advantage" of a descendent over an
     * ancestor; the terminal differential advantage is calculated as the
     * sum of the ownership advantages of each sub-board. The ownership
     * advantage is the difference between the weighted sums of n-filled
     * win-vector ownership of a board; the weight indicates how many
     * vectors underfilled by one are equal to one filled vector.
     */

    for (adv = 0, i = 3; i >= 0; --i) {
	for (j = 0; j < 9; ++j) {
	    register int hash = root->boards[j];
	    adv += subinfo[hash].ways[1][i];
	    adv -= subinfo[hash].ways[0][i];
	}
	adv *= i ? weight[i-1] : 1;
    }
    return adv;
}

static int randomOptimal(bNode *root)
{
    mNode *move;
    mNode *best = 0;
    int value = root->value;
    int p = PLAYDEP(root->depth);
    int n = 0;

    /*
     * One-pass algorithm for randomly selecting a move that is optimal
     * (best heuristic value) as follows: the current move node is
     * sub-optimal heuristically, it is skipped; otherwise, we proceed to
     * the evaluation.  Any time we see a node with an optimal heuristic
     * value, we increment bestCounter by 1. If the If it is the only one
     * (being the first mchoice evaluated), then the counter has a value of
     * 1, so we choose it with probability 1. This selection method has
     * uniform distribution, which we will prove by induction. Suppose we
     * have a sequence, a-1 through a-n, of n values, each of which has
     * equal value of being chosen (P(a-i) = 1/n forall 1 <= i <= n). If we
     * add a-n+1 with P(a-n+1) = 1/n+1, then P(!(a-n+1)) = 1 - 1/n+1 =
     * n/n+1, so P(a-i) = P(a-i|!a-n+1) * P(!a-n+1) = 1/n * n/n+1 = 1/n+1
     * forall i < n+1. For our base case, we take n = 1. We thus have
     * induction, so the distribution is entirely uniform.
     */
    for (move = root->moves; move; move = move->next) {
	if (p * move->child->value < p * value)
	    continue;
	if (p * move->child->value > p * value) {
	    value = move->child->value;
	    n = 0;
	}
	if ((random() % ++n) == 0)
	    best = move;
    }
    return best->loc;
}

int selfish(bNode *root, int depth)
{
    /*
     * The Selfish Evaluator
     *
     * ===FUNCTION===
     * Metastate advantage
     *
     * ===DESCRIPTION===
     * Greedily steals wins that are advantageous in short-term, zealously
     * blocks opponent wins that aren't advantageous in thes short-term
     *
     * ===BEHAVIOR===
     * When depth == n: Follow the path that has the highest advantage at
     * depth n. Branches are alpha-beta pruned with the metastate advantage
     * heuristic. Once depth n is reached, the function is no longer
     * evaluated. For the degenerate case of n=0, the heuristic function is
     * evaluated on the child states without any further lookups or
     * constructions.
     */

    root->value = alphabeta(NEGINFTY, POSINFTY, depth, root, advantage);
    return randomOptimal(root);
}

int calculating(bNode *root, int depth)
{
    /*
     * The Calculating Evaluator
     *
     * ===FUNCTION===
     * Terminal Differential Advantage
     *
     * ===DESCRIPTION===
     * Greedily selects moves that shift the terminal difference favorably,
     * zealously prevents opponent from taking move that shift the terminal
     * difference unfavorably
     *
     * ===BEHAVIOR===
     * When depth == n:
     * Follow the path that has the best terminal advantage at depth n, using
     * alpha-beta pruning of minimaxing.
     */
    root->value = alphabeta(NEGINFTY, POSINFTY, depth, root, terminus);
    return randomOptimal(root);
}

int dominating(bNode *root, int depth)
{
    /*
     * The Dominating Evaluator
     *
     * ===FUNCTION===
     * Board ownership advantage
     *
     * ===DESCRIPTION===
     * Maximizes the weighted sum of the n-filled win vectors for each board.
     *
     * ===BEHAVIOR===
     * When depth == n:
     * Follow the path that increases ability to win a board, calculated
     * using alpha-beta pruning and minimaxing.  For the degenerate case of
     * n=0, the heuristic function is evaluated on the child states without
     * any recursive lookups or constructions.
     */
    root->value = alphabeta(NEGINFTY, POSINFTY, depth, root, ownership);
    return randomOptimal(root);
}
