CFLAGS = -g

all: game

game: assert.o except.o mem.o table.o state.o move.o node.o heuristics.o subgame.o canon.o super.o
	$(CC) $(CFLAGS) -o game assert.o except.o mem.o table.o state.o move.o node.o heuristics.o subgame.o canon.o super.o

clean:
	$(RM) -f *.o game
