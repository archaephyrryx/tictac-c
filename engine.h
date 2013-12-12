#include <time.h>
#include <math.h>
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

#define ABS(x) ((x < 0) ? -1 * x : x)
#define PI (3.141592653589793)
#define MODDIST(x,y) (MIN(( ABS((x - y) % 81) ), ( ABS( 81 -((x - y) % 81)) )))

/* The Terminal Table */
extern Table_T termtable;

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
  subBoard meta;
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

