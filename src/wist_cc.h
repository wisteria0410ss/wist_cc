enum{
    TOKEN_NUM = 256,    // 整数
    TOKEN_ID,           // 識別子
    TOKEN_EOF,          // EOF
};

typedef struct Token{
    int   type;         // トークンの型
    int   value;        // 整数型の場合、その値
    char *str;          // トークン文字列
} Token;

enum{
    NODE_NUM = 256,     // 整数
    NODE_ID,
};

typedef struct Node{
    int type;           // ノードの型
    struct Node *lhs;   // 左の子
    struct Node *rhs;   // 右の子
    int value;          // 整数型の場合、その値
    char name;          // 識別子型の場合、その名前
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
Node *vector_get_node(Vector *vec, int pos);

// wist_cc.c
void tokenize(char *p);
Node *node_new(int type, Node *lhs, Node *rhs);
Node *node_new_num(int value);
Node *node_new_id(char name);
int consume(int type);
Node *add(void);
Node *mul(void);
Node *term(void);
Node *assign(void);
Node *stmt(void);
void program(void);
void gen(Node *node);
void gen_lval(Node *node);
void error(char *msg, char *str);
