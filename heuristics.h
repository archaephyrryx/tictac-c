#include "subgame.h"
#ifndef HEURISITCS_H
#define HEURISTICS_H
int terminalDifference(subBoard);
int advantage(bNode *);
int terminus(bNode *);
int ownership(bNode *);
int null(bNode *);
int selfish(bNode *, int);
int calculating(bNode *, int);
int dominating(bNode *, int);
int randomizer(bNode *, int);
int nullius(bNode *, int);
#endif
