.PHONY: test clean

CC      := gcc
CFLAGS  := -Wall -std=c11
HEADERS := $(wildcard src/*.h)
OBJS    := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
LOBJS   := $(filter-out obj/main.o,$(OBJS))

obj/%.o: src/%.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

bin/wist_cc: $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

bin/test: test/test.c $(LOBJS) Makefile
	$(CC) $(CFLAGS) $< $(LOBJS) -o $@ $(LDFLAGS)

test: bin/test bin/wist_cc Makefile
	bin/test

clean:
	-rm -f bin/* obj/*.o tmp/*
