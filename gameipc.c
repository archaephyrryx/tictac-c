#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define SPATH ".gamesock"

const static char *progname;
static int conn = -1;

void invite(const char *name)
{
    struct sockaddr_un s_un;
    int s;
    int len;

    if (!progname)
    	progname = name;

    if (unlink(SPATH) < 0 && errno != ENOENT) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("unlink");
    	exit(EXIT_FAILURE);
    }

    if ((s = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("socket");
    	exit(EXIT_FAILURE);
    }
    memset(&s_un, 0, sizeof(s_un));

    len = strlen(SPATH);
    s_un.sun_family = AF_UNIX;
    strncpy(s_un.sun_path, SPATH, sizeof(s_un.sun_path));
    s_un.sun_path[len] = '\0';

    if (bind(s, (struct sockaddr *)&s_un, sizeof(s_un)) < 0) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("bind");
    	exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("listen");
    	exit(EXIT_FAILURE);
    }

    if ((conn = accept(s, 0, 0)) < 0) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("accept");
    	exit(EXIT_FAILURE);
    }
}

void join(const char *name)
{
    struct sockaddr_un s_un;
    int s;
    int len;

    if (!progname)
    	progname = name;

    if ((s = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
    	fprintf(stderr, "%s: invite: ", progname);
	perror("socket");
    	exit(EXIT_FAILURE);
    }
    memset(&s_un, 0, sizeof(s_un));

    len = strlen(SPATH);
    s_un.sun_family = AF_UNIX;
    strncpy(s_un.sun_path, SPATH, sizeof(s_un.sun_path));
    s_un.sun_path[len] = '\0';

    while (connect(s, (struct sockaddr *)&s_un, sizeof(s_un)) < 0){ continue; }
    	/*fprintf(stderr, "%s: join: ", progname);
	perror("connect");
    	exit(EXIT_FAILURE);
    }*/
    conn = s;
}

int receiveMove(void)
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
