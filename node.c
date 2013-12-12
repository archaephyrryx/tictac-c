#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"
#include "node.h"
#include "mem.h"
#include "err.h"

#define MAXLINE 1024

subinfo_t subinfo[27 * 27 * 27];
static uint64_t one = 1;

int pow3[] = {
       1,     3,     9,
      27,  27*3,  27*9,
     729, 729*3, 729*9,
};

int loadinfo(const char *datfile)
{
    FILE *f = fopen(datfile, "r");
    char buf[MAXLINE];
    int line = 0;

    if (f == NULL)
    	efatal("loadinfo: fopen: %s", datfile);

    while (fgets(buf, sizeof(buf), f)) {
	int hash;
	int n;
	int i;
	int p;

    	++line;
        char *cp = buf;
    	if (sscanf(cp, "%d%n", &hash, &n) != 1)
	    fatal("loadinfo: %s: malformed line %d: %s",
	    	  datfile, line, "hash expected");
	if (sscanf(cp+=n, "%d %d %d %d%n",
		   &subinfo[hash].done, &subinfo[hash].win,
		   &subinfo[hash].max, &subinfo[hash].min, &n) != 4) 
	    fatal("loadinfo: %s: malformed line %d: %s",
	    	  datfile, line, "done win max min expected");
	for (p = 0; p < 2; ++p)
	    for (i = 0; i < 4; ++i)
		if (sscanf(cp+=n, "%d%n", &subinfo[hash].ways[p][i], &n) != 1)
		    fatal("loadinfo: %s: malformed line %d: %s",
			  datfile, line, "8 winways expected");
    }
    return line;
}

bNode *bNodeFree(bNode *root, int loc)
{
    bNode *next = 0;
    mNode *move;

#ifdef FREEBNODES
    mNode *mnext;

    for (move = root->moves; move; move = mnext) {
	mnext = move->next;
	if (move->loc == loc)
	    addMissing(next = move->child);
	else
	    (void) bNodeFree(move->child, -1);
	xfree(move);
    }
    xfree(root);
#else
    for (move = root->moves; move; move = move->next)
	if (move->loc == loc) {
	    addMissing(next = move->child);
	    break;
	}
#endif
    return next;
}

bNode *bNodeAlloc(bNode *parent, int loc)
{
    bNode *new;
    
    if (parent == 0) {
	new = xcalloc(1, sizeof(bNode));
	new->moves = 0;
	new->force = -1;
    } else {
	int b = loc / 9;
	int i = loc % 9;
	int p;

	new = xmalloc(sizeof(bNode));
	memcpy(new, parent, sizeof(*new));
	++new->depth;

	/*
	 * The factor for the first player (even parent depth, odd child
	 * depth) is 2, and for his opponent (odd parent depth, even child
	 * depth) is 1.
	 *
	 * A move by a player on any board can only lead to a win by that
	 * player.  We update the meta-board (index 9) when that happens.
	 */
	p = 1 + (new->depth % 2);
	new->boards[b] += p * pow3[i];
	new->force = subinfo[new->boards[i]].done ? -1 : i;
	if (subinfo[new->boards[b]].win)
	    new->boards[9] += p * pow3[b];
	if (subinfo[new->boards[b]].done)
	    new->boards[10] += 2 * pow3[b];

	/*
	 * The move list, move bitmask and heuristic value will be
	 * calculated as needed.
	 */
	new->moves = 0;
	new->value = 0;
	new->movemask[0] = new->movemask[1] = 0;
	new->usedmask[loc >> 6] |= one << (loc & 0x3f);
    }

    return new;
}

static mNode *mNodeAlloc(bNode *root, int loc)
{
    mNode *m = xmalloc(sizeof(mNode));

    m->loc = loc;
    m->next = root->moves;
    root->moves = m;
    m->child = bNodeAlloc(root, loc);

    return m;
}

void addMissing(bNode *root)
{
    int start;
    int end;
    int loc;

    if (root->force < 0) {
    	start = 0;
	end = 81;
    } else {
	start = 9 * root->force;
	end = start + 9;
    }

    for (loc = start; loc < end; ++loc) {
    	register int i = loc >> 6;
	register int j = loc & 0x3f;
	uint64_t bit = one << j;
	mNode *m;

	/*
	 * Dont't generate duplicate moves or go to occupied positions and
	 * completed boards.
	 */
    	if (((root->movemask[i] | root->usedmask[i]) & bit)
	    || subinfo[root->boards[loc/9]].done)
	    continue;

	root->movemask[i] |= bit;
	m = mNodeAlloc(root, loc);
    }
}

static int maxmovecmp(const void *a, const void *b)
{
    mNode *m1 = *(mNode **)a;
    mNode *m2 = *(mNode **)b;
    return (m2->child->value - m1->child->value);
}

static int minmovecmp(const void *a, const void *b)
{
    mNode *m1 = *(mNode **)a;
    mNode *m2 = *(mNode **)b;
    return (m1->child->value - m2->child->value);
}

static mNode **gather(bNode *root, int *nelem)
{
    mNode **mlist;
    mNode *move;
    int n = 0;
    int i;

    for (move = root->moves; move; move = move->next)
	++n;

    mlist = (mNode **) xmalloc(n * sizeof(mNode *));

    for (i = 0, move = root->moves; move; move = move->next)
	mlist[i++] = move; 

    *nelem = n;
    return mlist;
}

void sortMoves(bNode *root)
{
    int i;
    int nelem;
    mNode **moves = gather(root, &nelem);
    mNode *move;
    
    if (PLAYDEP(root->depth) == 1)
	qsort(moves, nelem, sizeof(mNode *), maxmovecmp); 
    else
	qsort(moves, nelem, sizeof(mNode *), minmovecmp); 

    root->moves = NULL;

    for (i = 0; i < nelem; ++i) {
	move = moves[i];
	move->next = root->moves; 
	root->moves = move;
    }
    xfree(moves);
}

void printNode(bNode *root)
{
    int i;
    int j;
    int k;
    int l;
    int loc = 0;
    char markers[3] = {' ','O','X'};

    /* Prints the board on the screen, in three lines of three digits */
    printf("------------------------------\n");
    for (i = 0; i < 3; ++i) {
	for (k = 0; k < 3; ++k) {
	    for (j = 0; j < 3; ++j) {
		printf("|");
		for (l = 0; l < 3; ++l) {
		    loc = i*27 + k*3 + j*9 + l;
		    int a = loc >> 6;
		    int b = loc & 0x3f;
		    if (!(root->usedmask[a] & (one << b))) {
			if (root->movemask[a] & (one << b)) {
			    printf(" . ");
			} else {
			    printf("   ");
			}
		    } else {
			int c = loc / 9;
			int d = loc % 9;
			int e = root->boards[c];

		    	while (d-- > 0)
			    e /= 3;
			e = e % 3;
			printf(" %c ", markers[e]);
		    }
		}
	    }
	    printf("|\n");
	}
	printf("------------------------------\n");
    }
}
