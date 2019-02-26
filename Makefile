.PHONY: test clean

bin/wist_cc: src/wist_cc.c Makefile
	gcc src/wist_cc.c -o bin/wist_cc

bin/test: test/test.c Makefile
	gcc test/test.c -o bin/test 

test: bin/test bin/wist_cc
	bin/test

clean:
	-rm -f bin/* obj/*.o tmp/*
