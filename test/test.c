#include <stdio.h>
#include <stdlib.h>

void compile(char *code);
void test_ret(char *code, int expect);

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

void main(){
    test_ret("0", 0);
    test_ret("42", 42);
    test_ret("123", 123);
    test_ret("1+3", 1+3);
    test_ret("5+20-4", 5+20-4);
    test_ret("12 + 34 - 5", 12+34-5);
    test_ret("3 -  10+\t9", 3-10+9);
    test_ret("5+6*7", 5+6*7);
    test_ret("5*(9-6)", 5*(9-6));
    test_ret("(3+5)/2", (3+5)/2);

    exit(0);
}