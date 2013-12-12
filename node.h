#ifndef NODE_H
#define NODE_H
bNode *bNodealloc(void);
mNode *mNodealloc(void);
void sortMoves(bNode *);
int hasMove(bNode *, int);
void addMissing(bNode *);
bNode *locToNode(bNode *, int);
#endif
