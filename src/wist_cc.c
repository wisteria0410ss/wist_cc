#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wist_cc.h"

// トークン列の保存先
Vector *tokens;
// ノードの保存先
Vector *code;

// 現在のトークン読み込み位置
int pos = 0;

void tokenize(char *p){
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if('a' <= *p && *p <= 'z'){
            Token *token = (Token *)malloc(sizeof(Token));
            token->type = TOKEN_ID;
            token->str  = p;
            vector_push(tokens, token);
            p++;
            continue;
        }
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' || *p == '(' || *p == ')' || *p == '=' || *p == ';'){
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

        error("トークナイズできません: %s\n", p);
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
    Node *node  = (Node *)malloc(sizeof(Node));
    node->type  = NODE_NUM;
    node->value = value;

    return node;
}

Node *node_new_id(char name){
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = NODE_ID;
    node->name = name;

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
            error("対応する閉括弧がありません: %s\n", vector_get_token(tokens, pos)->str);
        }
        return node;
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_ID){
        return node_new_id(vector_get_token(tokens, pos++)->str[0]);
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_NUM){
        return node_new_num(vector_get_token(tokens, pos++)->value);
    }

    error("不正なトークンです: %s\n", vector_get_token(tokens, pos)->str);
}

Node *assign(){
    Node *node = add();

    while(1){
        if(consume('=')) node = node_new('=', node, assign());
        else return node;
    }
}

Node *stmt(){
    Node *node = assign();
    if(!consume(';')){
        error(";が必要です: %s\n", vector_get_token(tokens, pos)->str);
    }

    return node;
}

void program(){
    while(vector_get_token(tokens, pos)->type != TOKEN_EOF){
        vector_push(code, stmt());
    }
    vector_push(code, NULL);

    return;
}

void gen_lval(Node *node){
    if(node->type != NODE_ID) error("代入の左辺値が変数ではありません。", "");

    int offset = ('z' - node->name + 1)*8;
    printf("\tmov \trax, rbp\n");
    printf("\tsub \trax, %d\n", offset);
    printf("\tpush\trax\n");

    return;
}

void gen(Node *node){
    if(node->type == NODE_NUM){
        printf("\tpush\t%d\n", node->value);
        return;
    }

    if(node->type == NODE_ID){
        gen_lval(node);
        printf(
            "\tpop \trax\n"
            "\tmov \trax, [rax]\n"
            "\tpush\trax\n"
        );
        return;
    }

    if(node->type == '='){
        gen_lval(node->lhs);
        gen(node->rhs);

        printf(
            "\tpop \trdi\n"
            "\tpop \trax\n"
            "\tmov \t[rax], rdi\n"
            "\tpush\trdi\n"
        );
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

void error(char *msg, char *str){
    fprintf(stderr, msg, str);
    exit(1);
}

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の数が不正です\n", "");
    }

    tokens = vector_new();
    code = vector_new();

    // トークナイズ, 結果はtokensに保存
    tokenize(argv[1]);
    // パース, 結果はcodeに保存
    program();

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
        "main:\n"
    );

    // プロローグ, aからzの領域を確保
    printf(
        "\tpush\trbp\n"
        "\tmov \trbp, rsp\n"
        "\tsub \trsp, 208\n"
    );

    for(int i=0;vector_get_node(code, i);i++){
        gen(vector_get_node(code, i));

        // 式の評価として残っているものをpop
        printf("\tpop \trax\n");
    }

    // エピローグ, 最後の式の結果がraxにあるので返り値になる
    printf(
        "\tmov \trsp, rbp\n"
        "\tpop \trbp\n"
        "\tret\n"
    );

    return 0;
}