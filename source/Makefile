CFLAGS=-O2 -Wall -Wextra -pedantic
HEADERS=frontend.h lang.h prototypes.h

OBJS=ast.o reader.o lexan.o utils.o memory.o parser.o

all: ../LexTest ../SyntaxTest ../Lang

$(OBJS): $(HEADERS)

../LexTest: ../tests/lextest.o $(OBJS)
	gcc $(CFLAGS) -o $@ ../tests/lextest.o $(OBJS)

../SyntaxTest: ../tests/syntaxtest.c $(OBJS)
	gcc $(CFLAGS) -o $@ ../tests/syntaxtest.c $(OBJS)

../Lang: $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o
