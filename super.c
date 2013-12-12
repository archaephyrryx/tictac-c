#include "engine.h"
#include "heuristics.h"
#include "node.h"
#include "move.h"
#include "super.h"
#include "duel.h"
#include "gauss.h"

Table_T termtable;

typedef struct {
  const char *name;
  int (*func)(bNode *, int);
} sentry;

sentry stratable[] = {
  {"cdsSigma", cdsSigma},
  {0, 0}
};


int queryMove(bNode *root)
{
  static char buf[MAXLINE];
  char *line;
  int move;

  /*
   * Enters a loop, asking a human player for a move, given as two adjacent
   * digits from 0-8. If EOF is reached (which will not normally happen), the
   * program exits. If the line is longer than three characters (two digits and
   * a newline), we continue to the next cycle of the loop. We then assign the
   * integer value of the first character of the line to move[0], and the value
   * of the second to move[1]. If either are greater than 8, then the move is
   * automatically out of bounds and the loop continues. If, however, they are
   * both less than eight, and the position they reference on the board is free
   * (as indicated by a call to isValid), then the loop exits and the location
   * move is overwritten to the move the player selected.
   */
  while (1) {
    printf("Enter a move of the form '<board><position>' (0-8): ");
    line = fgets(buf, MAXLINE, stdin);
    if (line == 0) {
      printf("EOF\n");
      exit(1);
    }
    if (strlen(line) > 3) {
      printf("Entry too long.\n");
      continue;
    }
    if (strlen(line) < 2) {
      printf("Entry too short.\n");
      continue;
    }
    if ((line[0] - '0') > 8 || (line[1] - '0') > 8) {
      printf("Indicated position out of bounds.\n");
      continue;
    }

    move = 9 * (line[0] - '0') + (line[1] - '0');

    if (hasMove(root, move))
      return move;

    printf("Indicated position cannot be selected.\n");
  }
}

int randomOptimal(bNode *root)
{
  addMissing(root);
  mNode *mchoice;
  mNode *bestchoice = 0;
  int bestValue = root->hValue;
  int p = PLAYDEP(root->depth);
  int randomChoice;
  int bestCounter = 0;

  /* One-pass algorithm for randomly selecting a move that is optimal (best
   * heuristic value) as follows: the current move node is sub-optimal
   * heuristically, it is skipped; otherwise, we proceed to the evaluation.
   * Any time we see a node with an optimal heuristic value, we increment
   * bestCounter by 1. If the If it is the only one (being the first mchoice
   * evaluated), then the counter has a value of 1, so we choose it with
   * probability 1. This selection method has uniform distribution, which we
   * will prove by induction. Suppose we have a sequence, a-1 through a-n, of n
   * values, each of which has equal value of being chosen (P(a-i) = 1/n forall
   * 1 <= i <= n). If we add a-n+1 with P(a-n+1) = 1/n+1, then P(!(a-n+1)) = 1 -
   * 1/n+1 = n/n+1, so P(a-i) = P(a-i|!a-n+1) * P(!a-n+1) = 1/n * n/n+1 = 1/n+1
   * forall i < n+1. For our base case, we take n = 1. We thus have induction,
   * so the distribution is entirely uniform.
   */
  for (mchoice = root->choices; mchoice; mchoice = mchoice->next) {
    if (p * mchoice->result->hValue < p * bestValue)
      continue;
    if (p * mchoice->result->hValue > p * bestValue) {
      bestValue = mchoice->result->hValue;
      bestCounter = 0;
    }
    randomChoice = (int) (random() % (++bestCounter));
    bestchoice = (randomChoice == 0) ? mchoice : bestchoice;
  }
  return bestchoice->loc;
}

void printState(bNode *root)
{
  int i;
  int j;
  int k;
  int l;
  char markers[3] = {'O',' ','X'};

  /* Prints the board on the screen, in three lines of three digits */
  printf("------------------------------\n");
  for (i = 0; i < 3; ++i) {
    for (k = 0; k < 3; ++k) {
      for (j = 0; j < 3; ++j) {
	printf("|");
	for (l = 0; l < 3; ++l) {
	  if (hasMove(root, (27 * i + 9 * j + 3 * k + l))) {
	    printf(" . ");
	  } else {
	    printf(" %c ", markers[root->state[3*i + j][3*k + l] + 1]);
	  }
	}
      }
    printf("|\n");
    }
    printf("------------------------------\n");
  }
}

int alphabeta(int a, int b, int d, bNode *r, int h(bNode *))
{
  /* a: alpha; b: beta; d: max depth; r: root; h:
   * heuristic function; hv: heuristic value; p: player */
  int p = PLAYDEP(r->depth);
  mNode *mchoice;

  if (r->depth == 81 || win(r->meta))
    return (r->hValue = 100 * h(r));

  /* Max depth reached */
  if (d == -1)
    return (r->hValue = h(r));


  /* add any uncomputed moves, sort the moves by hValue */
  addMissing(r);
  sortMoves(r);

  /* implementation of minimaxing with alpha beta pruning */
  if (p == 1) {
    for (mchoice = r->choices; mchoice; mchoice = mchoice->next) {
      bNode *child = mchoice->result;
      int tmp = alphabeta(a, b, d-1, child, h);

      a = MAX(a, tmp);
      if (b <= a)
	break;
    }
    return (r->hValue = a);
  } if (p == -1) {
    for (mchoice = r->choices; mchoice; mchoice = mchoice->next) {
      bNode *child = mchoice->result;
      int tmp = alphabeta(a, b, d-1, child, h);

      b = MIN(b, tmp);
      if (b <= a)
        break;
    }
    return (r->hValue = b);
  } else {
    abort();
  }
}



int main(int argc, char *argv[])
{
  int gamestate;
  int initplayer;
  int primaryplayer;
  int player;
  int ended = 0;
  int i;
  int choice;
  char *strat;
  sentry *ent;
  int (*tactic)(bNode *, int) = 0;

  /* usage: game strat [player-number] [reporter] */
  if (argc < 2)
    exit(1);
  strat = argv[1];
  initplayer = (argc > 2) ? atoi(argv[2]) : 1;
  primaryplayer = (argc > 3) ? atoi(argv[3]) : 1;
  
  for (ent = stratable; ent->name; ++ent) {
    if (strcmp(ent->name, strat) == 0) {
      tactic = ent->func;
      break;
    }
  }

  if (tactic == 0)
    exit(EXIT_FAILURE);

  srandom(time(NULL) + (getpid() << 16));
  bNode *root;
  bNode *next;
  
  root = bNodealloc();
  root->state = boardalloc(0);
  root->meta = subboardalloc(0);

  termtable = Table_new(SUBBOARDS, boardcmp, boardhash);
  makeTermTable(termtable);

  addMissing(root);

  if (initplayer == 1)
    invite(argv[0]);
  else
    join(argv[0]);

  for (gamestate = 0, player = 1; !ended && !gamestate; player *= -1) {

    if (player == initplayer) {
      choice = tactic(root, 5);
      sendMove(choice);
    } else {
      choice = receiveMove();
    }
    
    next = locToNode(root, choice);

    addMissing(next);

#if 0
    if (initplayer == primaryplayer) {
      printState(next);
    }
#endif

    root = next;
    
    ended = 1;
    for (i = 0; i < 9 && ended; ++i)
      if (countOpen(root->state[i]) && !(win(root->state[i])))
        ended = 0;

    if (!ended) {
      gamestate = win(root->meta);
    }
  }
#if 0
  printState(root);
#endif

  if (initplayer == 1) {
    switch (gamestate) {
    case 1:
      if (primaryplayer == 1)
	printf("win\n");
      else 
	printf("loss\n");
      break;
    case 0:
      printf("draw\n");
      break;
    case -1:
      if (primaryplayer == -1)
	printf("win\n");
      else 
	printf("loss\n");
      break;
    }
  }
  exit(EXIT_SUCCESS);
}
