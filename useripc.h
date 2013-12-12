#ifndef USERIPC_H
#define USERIPC_H
#include "node.h"

extern int ipcrole(void);
extern int ipcdone(void);
extern int receiveMove(bNode *);
extern void sendMove(int move);

#endif
