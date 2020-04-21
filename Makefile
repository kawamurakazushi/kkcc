CFLAGS=-std=c11 -g -static

kkcc: kkcc.c

test: kkcc
	./test.sh

clean:
	rm -f kkcc *.o *~ tmp*

.PHONY: test clean
