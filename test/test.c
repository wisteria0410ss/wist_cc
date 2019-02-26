#include <stdio.h>
#include <stdlib.h>
//extern int main(int argc, char **argv);

void compile(char *code){
    int ret;
    char cmdline[256];

    sprintf(cmdline, "bin/wist_cc %s > tmp/test.s", code);
    ret = WEXITSTATUS(system(cmdline));
    if(ret != 0){
        fprintf(stderr, "compile error: %d\ntest aborted.\n", ret);
        exit(1);
    }
    system("gcc tmp/test.s -o tmp/test");
}

int test_ret(char *code, int expect){
    compile(code);

    int ret;
    ret = WEXITSTATUS(system("tmp/test"));

    if(ret == expect){
        printf("[o] %s\n\t=> %d\n", code, ret);
        return 0;
    }else{
        printf("[x] %s\n\t=> %d, expected %d\n", code, ret, expect);
        exit(1);
        return 1;
    }
}

void test_run(){
    test_ret("0", 0);
    test_ret("42", 42);
    test_ret("123", 123);

    exit(0);
}

int main(){
    test_run();
    return 0;
}
