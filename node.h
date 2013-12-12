#ifndef NODE_H
#define NODE_H
bNode *bNodealloc(void);
mNode *mNodealloc(void);
void sortMoves(bNode *);
void addMissingMoves(bNode *);
bNode *locToNode(bNode *, int);
#endif
