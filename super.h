#ifndef SUPER_H
#define SUPER_H
int queryMove(bNode *);
int randomOptimal(bNode *);
bNode *build(bNode *, int);
int alphabeta(int, int, int, bNode *, int (*)(bNode *));
#endif
