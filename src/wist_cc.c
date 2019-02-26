#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
        "main:\n"
        "    mov     rax, %d\n"
        "    ret\n"
        , atoi(argv[1])
    );
    return 0;
}