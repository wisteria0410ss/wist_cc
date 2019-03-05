#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "../src/wist_cc.h"

extern int sum_10(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);

void compile(char *code);
void test_ret(char *code, int expect);
void test_num(char *msg, int result, int expect);

void compile(char *code){
    int ret;
    char cmdline[2048];

    sprintf(cmdline, "bin/wist_cc \'%s\' > tmp/test.s", code);
    ret = WEXITSTATUS(system(cmdline));
    if(ret != 0){
        fprintf(stderr, "compile error: %d\ntest aborted.\n", ret);
        exit(1);
    }
    system("gcc -no-pie tmp/test.s obj/func.o -o tmp/test");
}

void test_ret(char *code, int expect){
    printf("[\x1b[36mR\x1b[0m] %s\n", code);
    
    compile(code);

    int ret;
    ret = WEXITSTATUS(system("tmp/test"));

    if(ret == expect){
        printf("[\x1b[32mo\x1b[0m] %d\n\n", ret);
    }else{
        printf("[\x1b[31mx\x1b[0m] %d, expected %d\n\n", ret, expect);
        exit(1);
    }

    return;
}

void test_num(char *msg, int result, int expect){
    printf("[\x1b[36mN\x1b[0m] %s\n", msg);
    if(result == expect){
        printf("[\x1b[32mo\x1b[0m] %d\n\n", result);
    }else{
        printf("[\x1b[31mx\x1b[0m] %d, expected %d\n\n", result, expect);
        exit(1);
    }

    return;
}

int main(){
    test_ret("main(){0;}", 0);
    test_ret("main(){42;}", 42);
    test_ret("main(){1+3;}", 1+3);
    test_ret("main(){5+20-4;}", 5+20-4);
    test_ret("main(){12 + 34 - 5;}", 12+34-5);
    test_ret("main(){3 -  10+\t9;}", 3-10+9);
    test_ret("main(){5+6*7;}", 5+6*7);
    test_ret("main(){5*(9-6);}", 5*(9-6));
    test_ret("main(){(3+5)/2;}", (3+5)/2);
    test_ret("main(){(4+3)%5;}", (4+3)%5);
    test_ret("main(){12-9%2;}", 12-9%2);
    test_ret("main(){1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1;}", 64);
    test_ret("main(){a=3;b=5*6-8;a+b/2;}", 14);
    test_ret("main(){hoge=3;fugafugafuga=5*6-8;hoge+fugafugafuga/2;}", 14);
    test_ret(
        "main(){\n"
        "\ta=1;b=2;c=3;d=1;e=1;f=2;g=3;h=1;i=1;j=2;k=3;l=1;m=1;n=2;o=3;p=1;q=20;\n"
        "\tr=1;s=2;t=3;u=1;v=1;w=2;x=3;y=1;z=1;aa=2;ab=3;ac=1;ad=1;ae=2;af=3;ag=1;ah=20;\n"
        "\ta=12;a+ah;\n"
        "}\n", 32
    );
    test_ret("main(){a=4;b=2;a==b;}", 0);
    test_ret("main(){a=4;b=2;a!=b;}", 1);
    test_ret("main(){3==1==1;}", (3==1)==1);
    test_ret("main(){3!=1==1;}", (3!=1)==1);
    test_ret("main(){((2==3+1)-1!=3-4)*12;}", 0);
    test_ret("main(){1+foo()*3;}", 4);
    test_ret("main(){2*bar(2, 3, 5)+8;}", 22);
    test_ret("main(){a=sum_10(1,2,3,4,5,6,7,8,9,10,112);a%256;}", sum_10(1,2,3,4,5,6,7,8,9,10)%256);
    test_ret("main(){putchar(72);putchar(101);putchar(108);putchar(108);putchar(111);putchar(10);}", 10);
    test_ret("main(){putchar(72);putchar(101);putchar(108);return putchar(108);putchar(111);putchar(10);}", 108);
    test_ret("main(){return 42;}", 42);
    test_ret("one(){return 1;}\nmain(){a=one();return a*2+3;}", 5);
    test_ret("db(x){return x*2;}main(){return db(4);}", 8);
    test_ret("sum2(x,y){return x+y;}sum3(x,y,z){return x+y+z;}main(){sum2(1,2)+sum3(3,4+2,5);}", 1+2+3+6+5);
    test_ret(
        "main(){\n"
        "    ret = sum10(1,2,3,4,5,6,7,8,9,10);\n"
        "    return ret % 256;\n"
        "}\n"
        "sum10(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9){\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        "    putchar(48+a9-1);\n"
        
        "    return 0*a0 + 1*a1 + 2*a2 + 3*a3 + 4*a4 + 5*a5 + 6*a6 + 7*a7 + 8*a8 + 9*a9;\n"
        "}\n", sum_10(1,2,3,4,5,6,7,8,9,10)%256);

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

    return 0;
}