#include <stdio.h>
#include <stdlib.h>
#include "../src/wist_cc.h"

void compile(char *code);
void test_ret(char *code, int expect);
void test_num(char *msg, int result, int expect);

void compile(char *code){
    int ret;
    char cmdline[256];

    sprintf(cmdline, "bin/wist_cc \'%s\' > tmp/test.s", code);
    ret = WEXITSTATUS(system(cmdline));
    if(ret != 0){
        fprintf(stderr, "compile error: %d\ntest aborted.\n", ret);
        exit(1);
    }
    system("gcc tmp/test.s -o tmp/test");
}

void test_ret(char *code, int expect){
    compile(code);

    int ret;
    ret = WEXITSTATUS(system("tmp/test"));

    if(ret == expect){
        printf("[\x1b[32mo\x1b[0m] %s\n\t=> %d\n", code, ret);
    }else{
        printf("[\x1b[31mx\x1b[0m] %s\n\t=> %d, expected %d\n", code, ret, expect);
        exit(1);
    }

    return;
}

void test_num(char *msg, int result, int expect){
    if(result == expect){
        printf("[\x1b[32mo\x1b[0m] %s\n\t=> %d\n", msg, result);
    }else{
        printf("[\x1b[31mx\x1b[0m] %s\n\t=> %d, expected %d\n", msg, result, expect);
        exit(1);
    }

    return;
}

void main(){
    test_ret("0;", 0);
    test_ret("42;", 42);
    test_ret("123;", 123);
    test_ret("1+3;", 1+3);
    test_ret("5+20-4;", 5+20-4);
    test_ret("12 + 34 - 5;", 12+34-5);
    test_ret("3 -  10+\t9;", 3-10+9);
    test_ret("5+6*7;", 5+6*7);
    test_ret("5*(9-6);", 5*(9-6));
    test_ret("(3+5)/2;", (3+5)/2);
    test_ret("(4+3)%5;", (4+3)%5);
    test_ret("12-9%2;", 12-9%2);
    test_ret("1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1;", 64);
    test_ret("a=3;b=5*6-8;a+b/2;", 14);

    Vector *vec = vector_new();
    test_num("[Vector, len]", vec->len, 0);

    for(long i=0;i<100;i++) vector_push(vec, (void *)i);
    test_num("[Vector, len]", vec->len, 100);
    test_num("[Vector, item]", (long)vec->data[ 0],  0);
    test_num("[Vector, item]", (long)vec->data[50], 50);
    test_num("[Vector, item]", (long)vec->data[99], 99);

    Map *map = map_new();
    test_num("[Map, NULL]", (long)map_get(map, "hoge"), 0);
    map_put(map, "hoge", (void *)2);
    test_num("[Map, item]", (long)map_get(map, "hoge"), 2);
    map_put(map, "fuga", (void *)4);
    test_num("[Map, item]", (long)map_get(map, "fuga"), 4);
    map_put(map, "hoge", (void *)6);
    test_num("[Map, item]", (long)map_get(map, "hoge"), 6);

    exit(0);
}