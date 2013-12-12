#ifndef GAMEIPC_H
#define GAMEIPC_H

extern int ipcrole(void);
extern int ipcdone(void);
extern int receiveMove(void);
extern void sendMove(int move);

#endif
