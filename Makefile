CFLAGS=-O2 -Wall -Wextra -pedantic

all: LexTest SyntaxTest

LexTest:
	$(MAKE) -C source ../LexTest

SyntaxTest:
	$(MAKE) -C source ../SyntaxTest

clean:
	rm -f LexTest SyntaxTest
	$(MAKE) -C source clean

.PHONY: all clean LexTest SyntaxTest
