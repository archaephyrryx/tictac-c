#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "useripc.h"
#include "err.h"
#include "node.h"

#define MAXLINE 512

static uint64_t one = 1;

int ipcrole(void)
{
    return -1;
}

int ipcdone(void)
{
    return 0;
}

int receiveMove(bNode *root)
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
  addMissing(root);

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

    if (root->movemask[move>>6] & (one << (move & 0x3f)))
      return move;

    printf("Indicated position cannot be selected.\n");
  }
}

void sendMove(int move)
{
    printf("%d:%d\n", move/9, move%9);
}
