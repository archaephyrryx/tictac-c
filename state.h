#ifndef STATE_H
#define STATE_H
board boardalloc(board);
subBoard subboardalloc(subBoard);
void boardcpy(board, board);
void subboardcpy(subBoard, subBoard);
unsigned boardhash(const void *);
int boardcmp(const void *, const void *);
#endif
