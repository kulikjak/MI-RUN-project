CFLAGS=-O2 -Wall -Wextra -pedantic -g
HEADERS=frontend.h lang.h prototypes.h

OBJS=ast.o reader.o lexan.o utils.o memory.o parser.o runtime.o io.o functions.o main.o

all: ../Lang

$(OBJS): $(HEADERS)

../Lang: $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o
