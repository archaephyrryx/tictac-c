CFLAGS = -g -Wall -Werror -lm
LDFLAGS = -lm 

COMPONENTS := assert.o except.o mem.o table.o \
              state.o move.o node.o heuristics.o \
	      subgame.o canon.o super.o

all: game

game: $(COMPONENTS)
	$(CC) $(CFLAGS) $(COMPONENTS) -o game $(LDFLAGS)


clean:
	$(RM) -f *.o game

assert.o: assert.c assert.h except.h 
canon.o: canon.c engine.h
except.o: except.c
heuristics.o: heuristics.c engine.h super.h subgame.h
mem.o: mem.c
move.o: move.c engine.h
node.o: node.c
state.o: state.c engine.h
subgame.o: subgame.c
super.o: super.c engine.h node.h
table.o: table.c mem.h except.h assert.h table.h
