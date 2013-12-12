#ifndef STATE_H
#define STATE_H
board boardalloc(board);
subBoard subboardalloc(subBoard);
unsigned boardhash(const void *);
int boardcmp(const void *, const void *);
#endif
