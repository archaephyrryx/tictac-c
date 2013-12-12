#ifndef COMMS_H
#define COMMS_H
extern void invite(const char *name);
extern void join(const char *name);
extern int receiveMove(void);
extern void sendMove(int move);
#endif
