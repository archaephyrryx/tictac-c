#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "table.h"

#define MAXLINE 512

/* Minimaxing */
#define POSINFTY INT_MAX
#define NEGINFTY INT_MIN
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
#define MIN(x,y) (((x) <= (y)) ? (x) : (y))

/* Player Identification */
#define PLAYDEP(x) (1 + -2 * ((x) % 2))
#define SIGN(x) ((x < 0) ? -1 : (x > 0))

typedef int *subBoard;
typedef int (*board)[9];

typedef struct terminal {
  int min;
  int max;
} terminal;

typedef struct bNode {
  int depth;
  int hValue;
  board state;
  int lastMove;
  struct mNode *choices;
} bNode;

typedef struct mNode {
  int loc;
  struct bNode *result;
  struct mNode *next;
} mNode;

typedef struct morphism {
  int rotation;
  int reflection;
} morphism;
