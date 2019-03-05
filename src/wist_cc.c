#include <stdio.h>
#include <stdlib.h>
#include "wist_cc.h"

// トークン列の保存先
Vector *tokens;
// ノードの保存先
Vector *code;
// 現在のトークン読み込み位置
int pos;
// ローカル変数のオフセットを記録する連想配列のVector
Vector *local_val;

void error(char *msg, char *str){
    fprintf(stderr, msg, str);
    exit(1);
}

void global_init(){
    tokens = vector_new();
    code = vector_new();
    local_val = vector_new();
    pos = 0;
}
