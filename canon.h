#define SUBBOARDS 20000
#ifndef CANON_H
#define CANON_H
void rotate(subBoard);
void reflect(subBoard);
subBoard canonBoard(subBoard);
unsigned boardhash(const void *);
int boardcmp(const void *, const void *);
#endif
