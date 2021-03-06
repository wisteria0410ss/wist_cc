enum{
    TOKEN_NUM = 256,    // 整数
    TOKEN_ID,           // 識別子
    TOKEN_EQ,           // 比較演算子==
    TOKEN_NEQ,          // 比較演算子!=
    TOKEN_RET,          // キーワード return
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
    NODE_FNC,
    NODE_EQ,
    NODE_NEQ,
    NODE_RET,
    NODE_FND,
};

typedef struct Node{
    int type;           // ノードの型
    struct Node *lhs;   // 左の子
    struct Node *rhs;   // 右の子
    int value;          // 整数型の場合、その値
    char *name;         // 識別子型の場合、その名前
} Node;

typedef struct Vector{
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct Map{
    Vector *keys;
    Vector *vals;
} Map;

// vector.c
Vector *vector_new(void);
void vector_push(Vector *vec, void *elem);
Token *vector_get_token(Vector *vec, int pos);
Node *vector_get_node(Vector *vec, int pos);

// map.c
Map *map_new(void);
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

// parse.c
void tokenize(char *p);
Node *node_new(int type, Node *lhs, Node *rhs);
Node *node_new_num(int value);
Node *node_new_id(char *name);
Node *node_new_fnc(char *name, Node *args);
Node *node_new_fnd(char *name, Node *args);
int consume(int type);
Node *cmm(void);
Node *cmp(void);
Node *add(void);
Node *mul(void);
Node *term(void);
Node *assign(void);
Node *stmt(void);
void program(void);

// codegen.c
void gen(Node *node, int func_id);
void gen_lval(Node *node, int func_id);

// wist_cc.c
extern Vector *tokens, *code;
extern int pos;
extern Vector *local_val;
void error(char *msg, char *str);
void global_init(void);