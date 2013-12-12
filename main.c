#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "game.h"
#include "err.h"
#include "node.h"
#include "gameipc.h"
#include "cdsSigma.h"
#include "cdsScalar.h"
#include "fullRandom.h"

typedef struct {
  const char *name;
  int (*func)(bNode *, int);
  void (*init)(const char *);
} tentry;

tentry tactics[] = {
  {"cdsSigma", cdsSigma, cdsSigmaInit},
  {"cdsScalar", cdsScalar, cdsScalarInit},
  {"fullRandom", fullRandom, fullRandomInit},
  {0, 0}
};

static void usage(void)
{
    fprintf(stderr, "usage: ");
    fprintf(stderr,
    	    "%s datfile depth tactic1 init1 tactic2 init2 [report] [repeat]\n",
	    progname ? progname : "a.out");
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
    static int stats[3];
    bNode *root;
    const char *tname;
    const char *confpath;
    tentry *t;
    int done;
    int win;
    int initplayer;
    int lookahead;
    int repeat;
    int report;
    int player;
    int round;

    if (argc > 0) {
	progname = *argv++; --argc;
    }

    if (argc < 6)
    	usage();

    (void) loadinfo(*argv++); --argc;

    lookahead = atoi(*argv++); --argc;

    initplayer = ipcrole();

    tname = *argv++; --argc;
    confpath = *argv++; --argc;
    if (initplayer == 1) {
	for (t = tactics; t->name; ++t)
	    if (strcmp(t->name, tname) == 0)
		break;
	if (t->name == 0)
	    fatal("invalid player 1 tactic: %s", tname);
    	t->init(confpath);
    }

    tname = *argv++; --argc;
    confpath = *argv++; --argc;
    if (initplayer == -1) {
	for (t = tactics; t->name; ++t)
	    if (strcmp(t->name, tname) == 0)
		break;
	if (t->name == 0)
	    fatal("invalid player 2 tactic: %s", tname);
    	t->init(confpath);
    }

    if (argc > 0) {
	report = atoi(*argv++); --argc;
    } else {
	report = initplayer == 1;
    }

    repeat = 1;
#ifdef FREEBNODES
    if (argc > 0) {
	repeat = atoi(*argv++); --argc;
    }
#endif

    srandom(time(NULL) + (getpid() << 16));

    for (round = 0; round < repeat; ++round) {
	root = bNodeAlloc(0, 0);
	addMissing(root);

	for (done = win = 0, player = 1; !done; player *= -1) {
	    int choice;
	    bNode *next;

	    if (player == initplayer) {
		choice = t->func(root, lookahead);
		sendMove(choice);
	    } else {
		choice = receiveMove();
	    }
	    next = bNodeFree(root, choice);

	    if (initplayer == 1 && report > 1) {
	    	printf("%d:%d\n", choice/9, choice % 9);
		printNode(next);
	    }

	    if ((done = next->boards[10] == 27 * 27 * 27 - 1) == 0)
	      done = win = subinfo[next->boards[9]].win;
	    root = next;
	}

	(void) bNodeFree(root, -1);
	if (initplayer == 1)
	    ++stats[win ? win + 1 : 1];
    }

    if (initplayer == 1 && report) {
    	if (repeat > 1) {
	    printf("l %d\n", stats[0]);
	    printf("d %d\n", stats[1]);
	    printf("w %d\n", stats[2]);
	} else if (stats[0]) {
	    printf("l\n");
	} else if (stats[1]) {
	    printf("d\n");
	} else if (stats[2]) {
	    printf("w\n");
	}
    }

    exit(ipcdone());
}
