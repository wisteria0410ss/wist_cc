#include <stdio.h>
#include "wist_cc.h"

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の数が不正です\n", "");
    }

    // グローバル変数の初期化
    global_init();    
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
        "\tsub \trsp, %d\n", 8*local_val->keys->len
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