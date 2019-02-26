#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum{
    TOKEN_NUM = 256,    // 整数
    TOKEN_EOF,          // EOF
};

typedef struct{
    int   type;         // トークンの型
    int   value;        // 整数型の場合、その値
    char *str;          // トークン文字列
} Token;

// トークン列の保存先
Token tokens[100];

void tokenize(char *p){
    int idx = 0;        // 次に読むトークンの添字
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p == '+' || *p == '-'){
            tokens[idx].type = *p;
            tokens[idx].str  = p;
            idx++;
            p++;
            continue;
        }
        if(isdigit(*p)){
            tokens[idx].type  = TOKEN_NUM;
            tokens[idx].str   = p;
            tokens[idx].value = strtol(p, &p, 10);
            idx++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[idx].type = TOKEN_EOF;
    tokens[idx].str  = p;
    idx++;

    return;
}

void error(int i){
    fprintf(stderr, "予期しないトークンです: %s\n", tokens[i].str);
    exit(1);
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    tokenize(argv[1]);

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
        "main:\n"
    );

    if(tokens[0].type != TOKEN_NUM) error(0);
    printf("\tmov \trax, %d\n", tokens[0].value);
    
    int idx = 1;
    while(tokens[idx].type != TOKEN_EOF){
        switch(tokens[idx].type){
            case '+':
                idx++;
                if(tokens[idx].type != TOKEN_NUM) error(idx);
                printf("\tadd \trax, %d\n", tokens[idx].value);
                idx++;
                continue;
            case '-':
                idx++;
                if(tokens[idx].type != TOKEN_NUM) error(idx);
                printf("\tsub \trax, %d\n", tokens[idx].value);
                idx++;
                continue;
            default:
                error(idx);
        }
    }

    printf("\tret\n");
    return 0;
}