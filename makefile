CFLAGS = -g -Wall -Werror
LDFLAGS = -lm 

COMPONENTS := assert.o except.o mem.o table.o \
              state.o move.o node.o heuristics.o \
	      subgame.o canon.o duel.o gauss.o

all: game

game: super.o engine.a 
	$(CC) $(CFLAGS) super.o engine.a -o game $(LDFLAGS)

clean:
	$(RM) -f *.o game engine.a

engine.a: $(COMPONENTS)
	ar cr engine.a $(COMPONENTS)

assert.o: assert.c assert.h except.h 
canon.o: canon.c engine.h
except.o: except.c
heuristics.o: heuristics.c engine.h super.h subgame.h
mem.o: mem.c
move.o: move.c engine.h
node.o: node.c engine.h
state.o: state.c state.h engine.h
duel.o: duel.c duel.h
gauss.o: gauss.c gauss.h engine.h heuristics.h
subgame.o: subgame.c
super.o: super.c engine.h node.h duel.h gauss.h super.h
table.o: table.c mem.h except.h assert.h
