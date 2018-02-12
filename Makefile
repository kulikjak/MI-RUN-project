
all: LexTest SyntaxTest

LexTest:
	$(MAKE) -C source ../LexTest

SyntaxTest:
	$(MAKE) -C source ../SyntaxTest

Lang:
	$(MAKE) -C source ../Lang

clean:
	rm -f LexTest SyntaxTest Lang
	$(MAKE) -C source clean

.PHONY: all clean LexTest SyntaxTest Lang
