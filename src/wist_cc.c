#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    char *p = argv[1];

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
        "main:\n"
    );
    printf("    mov     rax, %ld\n", strtol(p, &p, 10));
    while(*p){
        switch(*p){
            case '+':
                p++;
                printf("    add      rax, %ld\n", strtol(p, &p, 10));
                continue;
            default:
                fprintf(stderr, "予期しない文字です: '%c'\n", *p);
                return 1;
        }
    }

    printf("    ret\n");
    return 0;
}