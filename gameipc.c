#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "gameipc.h"
#include "err.h"

static int conn = -1;
static pid_t child;

int ipcrole(void)
{
    int pair[2];

    if (socketpair(PF_UNIX, SOCK_STREAM, 0, pair) < 0)
    	efatal("socketpair");

    child = fork();
    if (child < 0)
    	efatal("fork");

    if (child == 0) {
	conn = pair[1];
	(void) close(pair[0]);
	return -1;
    } else {
	conn = pair[0];
    	(void) close(pair[1]);
	return 1;
    }
}

int ipcdone(void)
{
    int w;
    pid_t pid;

    if (child == 0)
    	return EXIT_SUCCESS;

    while ((pid = waitpid(child, &w, 0)) < 0 && errno == EINTR)
    	;

    if (pid != child)
    	efatal("waitpid");

    if (WIFSIGNALED(w))
    	kill(0, WTERMSIG(w));

    if (WIFEXITED(w))
    	return (WEXITSTATUS(w));

    fatal("waitpid: child neither exited nor terminated");
}

int receiveMove(bNode *root)
{
    int move = 0;
    int len = sizeof(move);
    int n;

    if (conn < 0) {
    	fprintf(stderr, "%s: receiveMove: not connected\n", progname);
    	exit(EXIT_FAILURE);
    }
    
    if ((n = read(conn, &move, len)) != len) {
    	fprintf(stderr, "%s: receiveMove: short read: %d\n", progname, n);
    	exit(EXIT_FAILURE);
    }

    return move;
}

void sendMove(int move)
{
    int len = sizeof(move);
    int n;

    if (conn < 0) {
    	fprintf(stderr, "%s: sendMove: not connected\n", progname);
    	exit(EXIT_FAILURE);
    }
    
    if ((n = write(conn, (char *)&move, len)) != len) {
    	fprintf(stderr, "%s: sendMove: short write: %d\n", progname, n);
    	exit(EXIT_FAILURE);
    }
}
