#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wist_cc.h"

// トークン列の保存先
Vector *tokens;

// 現在のトークン読み込み位置
int pos = 0;

void tokenize(char *p){
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' || *p == '(' || *p == ')'){
            Token *token = (Token *)malloc(sizeof(Token));
            token->type = *p;
            token->str  = p;
            vector_push(tokens, token);
            p++;
            continue;
        }
        if(isdigit(*p)){
            Token *token = (Token *)malloc(sizeof(Token));
            token->type  = TOKEN_NUM;
            token->str   = p;
            token->value = strtol(p, &p, 10);
            vector_push(tokens, token);
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    Token *token = (Token *)malloc(sizeof(Token));
    token->type = TOKEN_EOF;
    token->str  = p;
    vector_push(tokens, token);

    return;
}

Node *node_new(int type, Node *lhs, Node *rhs){
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    node->lhs  = lhs;
    node->rhs  = rhs;
    
    return node;
}

Node *node_new_num(int value){
    Node *node = (Node *)malloc(sizeof(Node));
    node->type  = NODE_NUM;
    node->value = value;

    return node;
}

int consume(int type){
    if(vector_get_token(tokens, pos)->type != type) return 0;
    pos++;
    return 1;
}

Node *add(){
    Node *node = mul();

    while(1){
        if(consume('+'))      node = node_new('+', node, mul());
        else if(consume('-')) node = node_new('-', node, mul());
        else return node;
    }
}

Node *mul(){
    Node *node = term();

    while(1){
        if(consume('*'))      node = node_new('*', node, term());
        else if(consume('/')) node = node_new('/', node, term());
        else if(consume('%')) node = node_new('%', node, term());
        else return node;
    }
}

Node *term(){
    if(consume('(')){
        Node *node = add();
        if(!consume(')')){
            fprintf(stderr, "対応する閉括弧がありません: %s\n", vector_get_token(tokens, pos)->str);
            exit(1);
        }
        return node;
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_NUM){
        return node_new_num(vector_get_token(tokens, pos++)->value);
    }

    fprintf(stderr, "不正なトークンです: %s\n", vector_get_token(tokens, pos)->str);
    exit(1);
}

void gen(Node *node){
    if(node->type == NODE_NUM){
        printf("\tpush\t%d\n", node->value);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);
    printf("\tpop \trdi\n");
    printf("\tpop \trax\n");
    
    switch(node->type){
        case '+':
            printf("\tadd \trax, rdi\n");
            break;
        case '-':
            printf("\tsub \trax, rdi\n");
            break;
        case '*':
            printf("\tmul \trdi\n");
            break;
        case '/':
            printf("\txor \trdx, rdx\n");
            printf("\tdiv \trdi\n");
            break;   
        case '%':
            printf("\txor \trdx, rdx\n");
            printf("\tdiv \trdi\n");
            printf("\tmov \trax, rdx\n");
    }
    printf("\tpush\trax\n");

    return;
}

void error(int i){
    fprintf(stderr, "予期しないトークンです: %s\n", vector_get_token(tokens, i)->str);
    exit(1);
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    tokens = vector_new();
    tokenize(argv[1]);
    Node *node = add();

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
        "main:\n"
    );

    gen(node);

    printf(
        "\tpop \trax\n"
        "\tret\n"
    );

    return 0;
}