CFLAGS=-O2 -Wall -Wextra -pedantic

all: ../LexTest ../SyntaxTest

../LexTest: ../tests/lextest.c reader.c lexan.c utils.c
	gcc $(CFLAGS) -o $@ ../tests/lextest.c reader.c lexan.c utils.c

../SyntaxTest: ../tests/syntaxtest.c reader.c lexan.c parser.c ast.c utils.c
	gcc $(CFLAGS) -o $@ ../tests/syntaxtest.c reader.c lexan.c parser.c ast.c utils.c

clean:
	rm -f *.o
