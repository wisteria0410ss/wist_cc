.PHONY: test clean

bin/wist_cc: src/wist_cc.c src/wist_cc.h src/vector.c Makefile
	gcc src/wist_cc.c src/vector.c -o bin/wist_cc

bin/test: test/test.c src/vector.c Makefile
	gcc test/test.c src/vector.c -o bin/test

test: bin/test bin/wist_cc
	bin/test

clean:
	-rm -f bin/* obj/*.o tmp/*
