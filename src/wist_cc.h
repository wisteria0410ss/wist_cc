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

typedef struct Vector{
    void **data;
    int capacity;
    int len;
} Vector;

// vector.c
Vector *vector_new(void);
void vector_push(Vector *vec, void *elem);
Token *vector_get_token(Vector *vec, int pos);

// wist_cc.c
void tokenize(char *p);
Node *node_new(int type, Node *lhs, Node *rhs);
Node *node_new_num(int value);
int consume(int type);
Node *add(void);
Node *mul(void);
Node *term(void);
void gen(Node *node);
void error(int i);
