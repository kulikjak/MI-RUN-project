
all: Lang

Lang:
	$(MAKE) -C source ../Lang

clean:
	rm -f Lang
	$(MAKE) -C source clean

.PHONY: all clean Lang
