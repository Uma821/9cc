CFLAGS=-std=c11 -g -static -Wall
# tmp.cとtest.cを除く全ての.cファイルをコンパイルする
SRCS=$(filter-out tmp.c, $(filter-out test.c, $(wildcard *.c)))
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
		./test.sh

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean
