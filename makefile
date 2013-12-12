CFLAGS = -g -Wall -Werror

COMPONENTS := assert.o except.o mem.o table.o \
	      subgame.o canon.o super.o \
              state.o move.o node.o heuristics.o

all: game

game: $(COMPONENTS)
	$(CC) $(CFLAGS) $(COMPONENTS) -o game 


clean:
	$(RM) -f *.o game

assert.o: assert.c assert.h except.h 
canon.o: canon.c engine.h
except.o: except.c
heuristics.o: heuristics.c engine.h
mem.o: mem.c
move.o: move.c engine.h
node.o: node.c
state.o: state.c engine.h
subgame.o: subgame.c
super.o: super.c engine.h
table.o: table.c mem.h except.h assert.h table.h
