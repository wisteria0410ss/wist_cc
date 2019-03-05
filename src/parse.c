#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wist_cc.h"

void tokenize(char *p){
    int blocklevel = 0;
    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_'){
            int cap = 16;
            char *name = (char *)malloc(sizeof(char) * cap);
            name[0] = '\0';
            for(int len=1;('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_' || isdigit(*p);p++,len++){
                if(len >= cap){
                    cap *= 2;
                    name = realloc(name, cap);
                }
                sprintf(name, "%s%c", name, *p);
            }
            Token *token = (Token *)malloc(sizeof(Token));
            token->str  = name;
            if(strcmp(name, "return") == 0){
                token->type = TOKEN_RET;
                vector_push(tokens, token);
            }else{
                token->type = TOKEN_ID;
                vector_push(tokens, token);

                for(;isspace(*p);p++) ;
                if(*p == '('){
                    if(blocklevel == 0){
                        blocklevel++;
                        vector_push(local_val, map_new());
                    }
                    continue;
                }
                Map *lval = (Map *)local_val->data[local_val->len-1];
                if(blocklevel > 0 && map_get(lval, name)==NULL){
                    if(lval->keys->len == 0) map_put(lval, name, (void *)8);
                    else map_put(lval, name, lval->vals->data[lval->keys->len-1]+8);
                }
            }
            continue;
        }
        if((*p == '!' || *p == '=') && *(p+1) == '='){
            Token *token = (Token *)malloc(sizeof(Token));
            token->type = (*p=='='?TOKEN_EQ:TOKEN_NEQ);
            token->str = p;
            vector_push(tokens, token);
            p += 2;
            continue;
        }
        if(strchr("+-*/%(){}=;,", *p) != NULL){
            if(*p == '{'){}
            if(*p == '}') blocklevel--;
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

Node *node_new_fnc(char *name, Node *args){
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = NODE_FNC;
    node->name = name;
    node->lhs  = args;

    return node;
}

Node *node_new_fnd(char *name, Node *args){
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = NODE_FND;
    node->name = name;
    node->lhs  = args;

    return node;
}

int consume(int type){
    if(vector_get_token(tokens, pos)->type != type) return 0;
    pos++;
    return 1;
}

Node *cmm(){
    Node *node = cmp();
    while(1){
        if(consume(',')) node = node_new(',', node, cmp());
        else return node;
    }
}

Node *cmp(){
    Node *node = add();

    while(1){
        if(consume(TOKEN_EQ))       node = node_new(NODE_EQ,  node, add());
        else if(consume(TOKEN_NEQ)) node = node_new(NODE_NEQ, node, add());
        else return node;
    }
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
        Node *node = cmp();
        if(!consume(')')){
            error("対応する閉括弧がありません: %s\n", vector_get_token(tokens, pos)->str);
        }
        return node;
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_ID){
        int pos_open = pos;
        pos++;
        if(consume('(')){
            if(consume(')')) return node_new_fnc(vector_get_token(tokens, pos_open)->str, NULL);
            Node *node = cmm();
            if(!consume(')')){
                error("対応する閉括弧がありません: %s\n", vector_get_token(tokens, pos)->str);
            }
            return node_new_fnc(vector_get_token(tokens, pos_open)->str, node);
        }else return node_new_id(vector_get_token(tokens, pos-1)->str);
    }
    if(vector_get_token(tokens, pos)->type == TOKEN_NUM){
        return node_new_num(vector_get_token(tokens, pos++)->value);
    }

    error("不正なトークンです: %s\n", vector_get_token(tokens, pos)->str);
    return NULL;
}

Node *assign(){
    Node *node = cmp();

    while(1){
        if(consume('=')) node = node_new('=', node, assign());
        else return node;
    }
}

Node *stmt(){
    Node *node;
    if(consume(TOKEN_RET)) node = node_new(NODE_RET, assign(), NULL);
    else node = assign();
    if(!consume(';')){
        error(";が必要です: %s\n", vector_get_token(tokens, pos)->str);
    }

    return node;
}

void program(){
    while(vector_get_token(tokens, pos)->type != TOKEN_EOF){
        if(vector_get_token(tokens, pos)->type == TOKEN_ID && vector_get_token(tokens, pos+1)->type == '('){
            char *name = vector_get_token(tokens, pos)->str;
            pos += 2;
            if(vector_get_token(tokens, pos)->type == ')')
                vector_push(code, node_new_fnd(name, NULL));
            else
                vector_push(code, node_new_fnd(name, cmm()));
            if(!consume(')')) error("対応する閉括弧がありません: %s\n", vector_get_token(tokens, pos)->str);
            if(!consume('{')) error("{が必要です: %s\n", vector_get_token(tokens, pos)->str);
            
            while(!consume('}')){
                vector_push(code, stmt());
                if(vector_get_token(tokens, pos)->type == TOKEN_EOF) error("対応する}がありません", "");
            }
        }else error("関数定義ではありません: %s\n", vector_get_token(tokens, pos)->str);
    }
    vector_push(code, NULL);

    return;
}