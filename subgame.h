#include "state.h"
#include "canon.h"
#ifndef SUBGAME_H
#define SUBGAME_H
void makeTermTable(Table_T);
terminal *terminalTable(Table_T, subBoard);
int winWays(subBoard, int, int);
int stakes(subBoard, int, int, int, int);
int win(subBoard);
subBoard metaState(board);
#endif
