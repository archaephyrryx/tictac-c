#ifndef NODE_H
#define NODE_H

#include <stdint.h>

typedef struct node {
    struct mNode *moves;
    uint64_t movemask[2];
    uint64_t usedmask[2];
    unsigned short boards[11];
    int depth;
    int force;
    int value;
} bNode;

typedef struct mNode {
    int loc;
    struct node *child;
    struct mNode *next;
} mNode;

typedef struct {
    int done;
    int win;
    int max;
    int min;
    int adv;
    int ways[2][4];
} subinfo_t;

extern subinfo_t subinfo[];
extern int pow3[];

extern void addMissing(bNode *);
extern bNode *bNodeAlloc(bNode *, int);
extern int loadinfo(const char *);
extern bNode *bNodeFree(bNode *, int);
extern void printNode(bNode *);
extern void sortMoves(bNode *);

#endif
