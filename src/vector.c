#include <stdlib.h>
#include "wist_cc.h"

Vector *vector_new(void){
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    vec->data = (void **)malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;

    return vec;
}

void vector_push(Vector *vec, void *elem){
    if(vec->capacity == vec->len){
        vec->capacity *= 2;
        vec->data = (void **)realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len] = elem;
    vec->len++;

    return;
}

Token *vector_get_token(Vector *vec, int pos){
    return (Token *)vec->data[pos];
}

Node *vector_get_node(Vector *vec, int pos){
    return (Node *)vec->data[pos];
}