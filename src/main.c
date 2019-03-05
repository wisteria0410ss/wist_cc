#include <stdio.h>
#include "wist_cc.h"

int main(int argc, char **argv){
    if(argc != 2){
        error("引数の数が不正です\n", "");
    }

    // グローバル変数の初期化
    global_init();    
    //fprintf(stderr, "initialized.\n");
    // トークナイズ, 結果はtokensに保存
    tokenize(argv[1]);
    //fprintf(stderr, "tokenized.\n");
    // パース, 結果はcodeに保存
    program();
    //fprintf(stderr, "parsed.\n");

    printf(
        ".intel_syntax noprefix\n"
        ".global main\n"
        "\n"
    );

    int i = 0;
    for(int func_id=0;vector_get_node(code, i);func_id++){
        Map *lval = (Map *)local_val->data[func_id];
        if(vector_get_node(code, i)->type != NODE_FND)
            error("関数定義ではないノードです: %s\n", vector_get_node(code, i)->name);
        printf("%s:\n", vector_get_node(code, i)->name);
        // プロローグ, 引数とローカル変数の領域を確保
        printf(
            "\tpush\trbp\n"
            "\tmov \trbp, rsp\n"
            "\tsub \trsp, %d\n", 8*lval->keys->len
        );
        // 引数がある場合は適切にコピー
        if(vector_get_node(code, i)->lhs){
            int vcnt = 0, v = 0;
            for(Node *n=vector_get_node(code, i)->lhs;n->lhs;n=n->lhs) vcnt++;
            printf("\tpush\trdi\n");
            for(Node *n=vector_get_node(code, i)->lhs;;v++){
                if(n->lhs){
                    //vcnt-v番目の引数がn->rhsに
                    gen_lval(n->rhs, func_id);
                    if(vcnt-v >= 6){
                        printf(
                            "\tpop \trax\n"
                            "\tmov \trdi, [rbp+%d]\n"
                            "\tmov \t[rax], rdi\n", 16 + (vcnt-v-6)*8
                        );
                    }else{
                        char reg[6][4] = {"rdi", "rsi", "rdx", "rcx", "r8 ", "r9 "};
                        printf(
                            "\tpop \trax\n"
                            "\tmov \t[rax], %s\n", reg[vcnt-v]
                        );
                    }
                    n = n->lhs;
                }
                else{
                    gen_lval(n, func_id);
                    printf(
                        "\tpop \trax\n"
                        "\tpop \trdi\n"
                        "\tmov \t[rax], rdi\n"
                    );
                    break;
                }
            }
            
        }

        i++;
        for(;vector_get_node(code, i)!=NULL && vector_get_node(code, i)->type!=NODE_FND;i++){
            gen(vector_get_node(code, i), func_id);

            // 式の評価として残っているものをpop
            printf("\tpop \trax\n");        
        }
        
        // エピローグ, 最後の式の結果がraxにあるので返り値になる
        printf(
            "\tmov \trsp, rbp\n"
            "\tpop \trbp\n"
            "\tret\n"
        );
    }

    return 0;
}