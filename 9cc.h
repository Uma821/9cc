#define _XOPEN_SOURCE 700 // POSIX 関数の要求(strndup)
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;

//
// tokenize.c
//

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_KEYWORD,  // キーワード
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

// トークン型
typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// 入力プログラム
extern char *user_input;

// 現在着目しているトークン
extern Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
bool consume_keyword(char *op);
bool equal_keyword(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
bool at_block();
Token *consume_ident();
void tokenize();

//
// parse.c
//

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  Type *ty;   // Type
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// ローカル変数
extern LVar *locals;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_ASSIGN,  // =
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // <
  ND_LE,      // <=
  ND_IF,      // if
  ND_LOOP,    // for,while
  ND_RETURN,  // return
  ND_BLOCK,   // { ... }
  ND_FUNCALL, // 関数呼び出し
  ND_ADDR,    // 単項*
  ND_DEREF,   // 単項&
  ND_LVAR,    // ローカル変数
  ND_NUM,     // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの型
  Node *next;     // 次のノード
  Token *tok;     // このノードのトークン
  Type *ty;       // そのノードのタイプ,intやintへのポインタなど
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
  LVar *lvar;     // kindがND_LVARの場合のみ使う
  Node *cond;     // kindがND_IF,ND_LOOPの場合のみ使う
  Node *then;     // kindがND_IF,ND_LOOPの場合のみ使う
  Node *els;      // kindがND_IFの場合のみ使う
  Node *init;     // kindがND_LOOPの場合のみ使う
  Node *inc;      // kindがND_LOOPの場合のみ使う
  Node *body;     // kindがND_BLOCK(複文)の場合のみ使う
  char *funcname; // kindがND_FUNCALLの場合のみ使う
  Node *args;     // kindがND_FUNCALLの場合のみ使う
};

// 関数毎に内容、ローカル変数、ローカル変数用のスタックサイズの保存
typedef struct Function Function;
struct Function {
  Function *next;
  char *name;
  LVar *params;

  Node *body;
  LVar *locals;
  int stack_size;
};

Function *parse();

//
// type.c
//

typedef enum {
  TY_INT,
  TY_PTR,
  TY_ARRAY,
  TY_FUNC,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;           // sizeof
 
  Type *base;         // 〇へのポインタ
  size_t array_size;  // 配列
  Type *elem;         // 配列
  int decayed;        // 配列->ポインタへの降格発生時のフラグ
  char *name;         // 定義
  Token *tok;         // 変数の位置情報など
  // 関数
  Type *return_ty;
  Type *params;
  Type *next;
};

Type *new_type(TypeKind kind);
Type *func_type(Type *return_ty);
bool is_integer(Type *ty);
Type *pointer_to(Type *base);
Type *array_of(Type *elem, size_t size);
void add_type(Node *node);

//
// codegen.c
//

void codegen(Function *prog);
