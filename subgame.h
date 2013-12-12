#include "state.h"
#include "canon.h"
#ifndef SUBGAME_H
#define SUBGAME_H

extern void initstakes(void);
int countOpen(subBoard);
void makeTermTable(Table_T);
terminal *terminalTable(Table_T, subBoard);
int winWays(subBoard, int, int);
int win(subBoard);
subBoard metaState(board, int);
#endif
