#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wist_cc.h"

void tokenize(char *p){
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if('a' <= *p && *p <= 'z'){
            int cap = 16;
            char *name = (char *)malloc(sizeof(char) * cap);
            name[0] = '\0';
            for(int len=1;'a'<=*p && *p<='z';p++,len++){
                if(len >= cap){
                    cap *= 2;
                    name = realloc(name, cap);
                }
                sprintf(name, "%s%c", name, *p);
            }
            Token *token = (Token *)malloc(sizeof(Token));
            token->type = TOKEN_ID;
            token->str  = name;
            vector_push(tokens, token);
            if(map_get(local_val, name)==NULL){
                if(local_val->keys->len == 0) map_put(local_val, name, (void *)8);
                else map_put(local_val, name, local_val->vals->data[local_val->keys->len-1]+8);
            }
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

Node *node_new_id(char *name){
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
        return node_new_id(vector_get_token(tokens, pos++)->str);
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_NUM){
        return node_new_num(vector_get_token(tokens, pos++)->value);
    }

    error("不正なトークンです: %s\n", vector_get_token(tokens, pos)->str);
    return NULL;
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