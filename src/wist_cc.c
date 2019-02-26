#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum{
    TOKEN_NUM = 256,    // 整数
    TOKEN_EOF,          // EOF
};

typedef struct Token{
    int   type;         // トークンの型
    int   value;        // 整数型の場合、その値
    char *str;          // トークン文字列
} Token;

enum{
    NODE_NUM = 256,     // 整数
};

typedef struct Node{
    int type;
    struct Node *lhs;
    struct Node *rhs;
    int value;
} Node;

void tokenize(char *p);
Node *node_new(int type, Node *lhs, Node *rhs);
Node *node_new_num(int value);
int consume(int type);
Node *add();
Node *mul();
Node *term();
void gen(Node *node);
void error(int i);

// トークン列の保存先
Token tokens[100];

// 現在のトークン読み込み位置
int pos = 0;

void tokenize(char *p){
    int idx = 0;        // 次に読むトークンの添字
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' || *p == '(' || *p == ')'){
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
    if(tokens[pos].type != type) return 0;
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
            fprintf(stderr, "対応する閉括弧がありません: %s\n", tokens[pos].str);
            exit(1);
        }
        return node;
    }
    if(tokens[pos].type == TOKEN_NUM){
        return node_new_num(tokens[pos++].value);
    }

    fprintf(stderr, "不正なトークンです: %s\n", tokens[pos].str);
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
    fprintf(stderr, "予期しないトークンです: %s\n", tokens[i].str);
    exit(1);
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

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