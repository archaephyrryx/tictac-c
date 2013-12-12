#include "engine.h"
#include "heuristics.h"
#include "node.h"
#include "move.h"
#include "super.h"

Table_T termtable;

int queryMove(board state)
{
  static char buf[MAXLINE];
  char *line;
  int move[2];

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
    } else if (strlen(line) > 3) {
      printf("Entry too long.\n");
      continue;
    } else if (strlen(line) < 2) {
      printf("Entry too short.\n");
      continue;
    }
    move[0] = line[0] - '0';
    move[1] = line[1] - '0';

    if (move[0] > 8 || move[1] > 8) {
      printf("Indicated position out of bounds.\n");
      continue;
    } else if (!(isValid(metaState(state), move[0]))) {
      printf("Indicated board cannot be selected.\n");
      continue;
    } else if (!(isValid(state[move[0]], move[1]))) {
      printf("Indicated position cannot be selected.\n");
      continue;
    } else {
      break;
    }
  }
  return (9 * move[0] + move[1]);
}

int randomOptimal(bNode *root)
{
  mNode *mchoice;
  mNode *bestchoice;
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
    if (mchoice->result->hValue == root->hValue) {
      randomChoice = rand() % (++bestCounter);
      bestchoice = (randomChoice == 0) ? mchoice : bestchoice;
    }
  }
  return bestchoice->loc;
}

void printState(board state)
{
  int i;
  int j;
  int k;
  int l;
  char markers[3] = {'O','.','X'};

  /* Prints the board on the screen, in three lines of three digits */
  printf("------------------------------\n");
  for (i = 0; i < 3; ++i) {
    for (k = 0; k < 3; ++k) {
      for (j = 0; j < 3; ++j) {
	for (l = 0; l < 3; ++l) {
	  printf(" %c ", markers[state[3*i + j][3*k + l] + 1]);
	}
	printf("|");
      }
    printf("\n");
    }
    printf("------------------------------\n");
  }
}

bNode *build(bNode *root, int depth)
{
  mNode *mchoice;

  if (depth == 0) {
    return root;
  } else {
    addMissing(root);

    for (mchoice = root->choices; mchoice; mchoice = mchoice->next) {
      addMissing(mchoice->result);
    }

    return root;
  }
}

int alphabeta(int a, int b, int d, bNode *r, int h(board))
{
  /* a: alpha; b: beta; d: max depth; r: root; ht: hashtable; h:
   * heuristic function; hv: heuristic value; p: player */
  int p = PLAYDEP(r->depth);
  mNode *mchoice;

  /* Max depth reached */
  if (d == -1 || win(metaState(r->state)))
    return (r->hValue = h(r->state));

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
  int player;
  int choice;
  srand((unsigned) time(NULL));
  bNode *root;
  bNode *next;
  
  root = bNodealloc();
  root->state = boardalloc(0);

  termtable = Table_new(SUBBOARDS, boardcmp, boardhash);
  makeTermTable(termtable);

  root = build(root, 2);

  for (gamestate = 0, player = 1; gamestate == 0 && root->depth < 81; player *= -1) {
    printState(root->state);
    if (player == 1) {
      choice = calculating(root, 6); 
      // choice = dominating(root, 4); 
      // nchoice = selfish(root, 4); 
      next = locToNode(root, choice);
    } else {
      addMissing(root);
      do {
	choice = queryMove(root->state);
	next = locToNode(root, choice);
      } while (next == NULL);
    }
    
    root = next;
    gamestate = win(metaState(root->state));
  }
  printState(root->state);

  switch (gamestate) {
  case -1:
    printf("You win!\n");
    break;
  case 0:
    printf("If you didn't want a draw, don't play tic tac toe!\n");
    break;
  case 1:
    printf("Your code is good!\n");
    break;
  }
  exit(EXIT_SUCCESS);
}
