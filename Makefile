.PHONY: test clean

HEADERS := $(wildcard src/*.h)
OBJS    := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
LOBJS   := $(filter-out obj/main.o,$(OBJS))

obj/%.o: src/%.c $(HEADERS) Makefile
	gcc -c $< -o $@

bin/wist_cc: $(OBJS) Makefile
	gcc $(OBJS) -o $@

bin/test: test/test.c $(LOBJS) Makefile
	gcc $< $(LOBJS) -o $@

test: bin/test bin/wist_cc Makefile
	bin/test

clean:
	-rm -f bin/* obj/*.o tmp/*
