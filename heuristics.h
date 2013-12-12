#include "subgame.h"
#ifndef HEURISITCS_H
#define HEURISTICS_H
int terminalDifference(subBoard);
int advantage(board);
int terminus(board);
int ownership(board);
int selfish(bNode *, int);
int calculating(bNode *, int);
int dominating(bNode *, int);
int randomizer(bNode *, int);
int nullius(bNode *, int);
int gauss(int, int, int);
int stratagizer(bNode *, int);
#endif
