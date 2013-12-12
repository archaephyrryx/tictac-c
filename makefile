CFLAGS = -g -Wall -Werror
LDFLAGS = -lm 

COMPONENTS := assert.o except.o mem.o table.o \
              state.o move.o node.o heuristics.o \
	      subgame.o canon.o super.o duel.o

all: stratagy.o engine.a 
	$(CC) $(CFLAGS) stratagy.o engine.a -o game $(LDFLAGS)

clean:
	$(RM) -f *.o game

engine.a: $(COMPONENTS)
	ar cr engine.a $(COMPONENTS)

assert.o: assert.c assert.h except.h 
canon.o: canon.c engine.h
except.o: except.c
heuristics.o: heuristics.c engine.h super.h subgame.h
mem.o: mem.c
move.o: move.c engine.h
node.o: node.c engine.h
state.o: state.c engine.h
duel.o: duel.c
stratagy.o: stratagy.c heuristics.h
subgame.o: subgame.c
super.o: super.c engine.h node.h duel.h stratagy.h
table.o: table.c mem.h except.h assert.h
