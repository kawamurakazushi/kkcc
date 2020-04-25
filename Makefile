CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

kkcc: $(OBJS)
	$(CC) -o kkcc $(OBJS) $(LDFLAGS)

$(OBJS): kkcc.h

test: kkcc
	./test.sh

clean:
	rm -f kkcc *.o *~ tmp*

.PHONY: test clean
