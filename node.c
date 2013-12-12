#include <stdlib.h>

#include "engine.h"
#include "state.h"
#include "subgame.h"
#include "node.h"

bNode *bNodealloc(void)
{
  bNode *node = (bNode *) malloc(sizeof(bNode));
  node->depth = 0;
  node->hValue = 0;
  node->state = 0;
  node->lastMove = 0;
  node->choices = 0;
  return node;
}

mNode *mNodealloc(void)
{
  mNode *node = (mNode *) malloc(sizeof(mNode));
  node->loc = 0;
  node->result = 0;
  return node;
}

static mNode **gather(bNode *root, size_t *nelem)
{
  mNode *mchoice;
  int moves = 0;
  int i;

  for (mchoice = root->choices; mchoice; mchoice = mchoice->next) {
    ++moves;
  }

  mNode **mlist = (mNode **) malloc(moves * sizeof(mNode *));

  for (i = 0, mchoice = root->choices; mchoice; mchoice = mchoice->next) {
    mlist[i++] = mchoice; 
  }
  *nelem = moves;
  return mlist;
}


int hasMove(bNode *root, int loc)
{ 
  mNode *mchoice;

  for (mchoice = root->choices; mchoice; mchoice = mchoice->next) {
    if (mchoice->loc == loc) {
      return 1;
    }
  }
  return 0;
}

void addMissing(bNode *root)
{
  int loc;
  int last = root->lastMove;
  int moves = 0;
  board mState = boardalloc(0);
  subBoard meta = metaState(root->state);
 
  for (loc = 0; loc < 81; ++loc) {
    if (root->depth > 0) {
      if ((isValid(meta, last%9) && loc/9 != last%9) || !(isValid(root->state, loc))) {
	continue;
      }
    }
    if (hasMove(root, loc)) {
      continue;
    } else {
      boardcpy(mState, root->state);
      mNode *mchoice = mNodealloc();
      mchoice->loc = loc;

      mchoice->next = root->choices;
      root->choices = mchoice;

      applyMove(loc, mState, PLAYDEP(root->depth));
      mchoice->result = bNodealloc();
      mchoice->result->lastMove = loc;
      mchoice->result->state = boardalloc(mState);
      mchoice->result->depth = root->depth + 1;

    }
  }
  free(mState);
}

static int movecmp(const void *a, const void *b)
{
  mNode *m1 = *(mNode **)a;
  mNode *m2 = *(mNode **)b;
  return (m1->result->hValue - m2->result->hValue);
}

void sortMoves(bNode *root)
{
  int i;
  size_t nelem;
  mNode **moves = gather(root, &nelem);

  qsort(moves, nelem, sizeof(mNode *), movecmp); 

  mNode *mchoice;
  root->choices = NULL;

  for (i = 0; (mchoice = moves[i]); ++i) {
    mchoice->next = root->choices; 
    root->choices = mchoice;
  }
  free(moves);
}


bNode *locToNode(bNode *root, int loc)
{
  mNode *mchoice;

  for (mchoice = root->choices; mchoice; mchoice = mchoice->next) {
    if (mchoice->loc == loc) {
      return mchoice->result;
    }
  }
  return NULL;
}