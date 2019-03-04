#include <stdio.h>

int foo(){
    printf("OK\n");
    return 1;
}

int bar(int a, int b){
    return 2*a+b;
}

int sum_10(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9){
    return 0*a0 + 1*a1 + 2*a2 + 3*a3 + 4*a4 + 5*a5 + 6*a6 + 7*a7 + 8*a8 + 9*a9;
}