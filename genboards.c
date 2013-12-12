#include <stdlib.h>
#include <stdio.h>

#include "err.h"
#include "node.h"

static int seen[27 * 27 * 27];

typedef int subBoard[9];
static subBoard root;

static int stakes(int a, int b, int c, int p) {

    /*
     * For three positions along a win-vector, the sum of the pair-wise
     * products of their values is negative if two positions have opposite
     * values (+1 and -1). If all values are 0 or a single player value,
     * then this evaluation is greater than or equal to 0.
     */
    if (((a * b + b * c + c * a) >= 0) && ((p * (a + b + c)) >= 0))
      return p * (a + b + c);
    return -1;
}
 
void winWays(subBoard state, int player, int *ways)
{
#define docount(i, j, k) do { \
	int _s = stakes(state[i], state[j], state[k], player); \
	if (_s >= 0) \
	    ++ways[_s]; \
    } while (0)

    /*
     * Counts the number of win vectors that a player owns a certain number of
     * positions along
     */
    docount(0, 1, 2);
    docount(0, 3, 6);
    docount(0, 4, 8);
    docount(1, 4, 7);
    docount(2, 4, 6);
    docount(2, 5, 8);
    docount(3, 4, 5);
    docount(6, 7, 8);
}

int whowon(subBoard state)
{
    int i;

    /*
     * We cycle through all winning lines on the board. As the values at
     * those positions are either 1, 0, or -1, any potential winning triplet
     * must hold the same values. a*(a+b+c) == 3 iff a, b, c are all equal
     * and non-zero.
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

static void genboards(subBoard root, int hash, int depth, int adv)
{
    int w;
    int p;
    int i;

    if (seen[hash])
	return;
    seen[hash] = 1;
    subinfo[hash].adv = adv;

    if ((w = subinfo[hash].win = whowon(root)) != 0) {
	subinfo[hash].done = 1;
	subinfo[hash].max = (w == 1);
	subinfo[hash].min = (w == -1);
    } else if (depth == 9) {
	subinfo[hash].done = 1;
	subinfo[hash].max = subinfo[hash].min = 0;
    } else {
	for (i = 0; i < 9; ++i) {
	    if (root[i] != 0)
		continue;
	    for (p = 0; p < 2; ++p) {
		int chash = hash + (p + 1) * pow3[i];
		root[i] = (p << 1) - 1;
		genboards(root, chash, depth + 1, adv + root[i]);
		subinfo[hash].max += subinfo[chash].max;
		subinfo[hash].min += subinfo[chash].min;
	    }
	    root[i] = 0;
	}
    }
    printf("%d %d %d %d %d", hash,
	   subinfo[hash].done, subinfo[hash].win,
	   subinfo[hash].max, subinfo[hash].min);
    for (p = 0; p < 2; ++p) {
	winWays(root, (p << 1) - 1, subinfo[hash].ways[p]);
    	for (i = 0; i < 4; ++i)
	    printf(" %d", subinfo[hash].ways[p][i]);
    }
    putchar('\n');
}

int main(int argc, const char *argv[])
{
    if (argc > 0)
	progname = argv[0];
    genboards(root, 0, 0, 0);
    return EXIT_SUCCESS;
}
