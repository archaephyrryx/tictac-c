#ifndef SUPER_H
#define SUPER_H
int queryMove(board);
int randomOptimal(bNode *);
bNode *build(bNode *, int);
int alphabeta(int, int, int, bNode *, int (*)(board));
#endif
