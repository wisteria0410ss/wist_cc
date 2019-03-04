#include <stdio.h>
#include "wist_cc.h"

void gen_lval(Node *node){
    if(node->type != NODE_ID) error("代入の左辺値が変数ではありません。", "");

    int offset = (long)map_get(local_val, node->name);
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

    if(node->type == NODE_FUNC){
        if(node->lhs == NULL){
            printf("\tcall\t%s\n", node->name);
            printf("\tpush\trax\n");
        }else{
            int cnt = 1, rest;
            Node *ar;
            for(ar=node->lhs;ar->type==',';ar=ar->lhs) cnt++;
            rest = cnt;

            char reg[6][4] = {"rdi", "rsi", "rdx", "rcx", "r8 ", "r9 "};
            if(cnt>6 && cnt%2==1) printf("\tsub \trsp, 8\n");
            for(ar=node->lhs;ar->type==',';ar=ar->lhs){
                gen(ar->rhs);
                rest--;
                if(rest<6) printf("\tpop \t%s\n", reg[rest]);
            }
            gen(ar);
            rest--;
            printf("\tpop \t%s\n", reg[rest]);
            if(rest != 0) error("引数の処理に失敗しました。", "");

            printf("\tcall\t%s\n", node->name);
            if(cnt>6) printf("\tadd \trsp, %d\n", (cnt-5)/2*16);
            printf("\tpush\trax\n");
        }
        
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
            break;
        case NODE_EQ:
            printf("\tcmp \trdi, rax\n");
            printf("\tsete\tal\n");
            printf("\tmovzb\trax, al\n");
            break;
        case NODE_NEQ:
            printf("\tcmp \trdi, rax\n");
            printf("\tsetne\tal\n");
            printf("\tmovzb\trax, al\n");
            break;
    }
    printf("\tpush\trax\n");

    return;
}
