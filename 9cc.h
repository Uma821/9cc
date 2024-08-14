#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define true 1
#define false 0

typedef struct Type Type;
typedef struct Node Node;

// errno.h プロトタイプ宣言
long *__errno_location(void);
// ctype.h プロトタイプ宣言
long ispunct(long c);
long isspace(long c);
long isdigit(long c);

//
// tokenize.c
//

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_STR,      // 文字列リテラル
  TK_KEYWORD,  // キーワード
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

// トークン型
typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  long val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  long len;        // トークンの長さ
};

// 入力プログラム
extern char *user_input;
// 入力ファイル名
extern char *filename;

// 現在着目しているトークン
extern Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
long consume(char *op);
long consume_keyword(char *op);
long equal_keyword(char *op);
void expect(char *op);
long expect_number();
long at_eof();
long at_block();
Token *consume_ident();
Token *consume_str();
void tokenize();

//
// parse.c
//

// ローカル変数の型
typedef struct LVar LVar;
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  Type *ty;   // Type
  long len;    // 名前の長さ
  long offset; // RBPからのオフセット
};

// ローカル変数
extern LVar *locals;

// グローバル変数の型
typedef struct GVar GVar;
struct GVar {
  GVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  Type *ty;   // Type
  long len;    // 名前の長さ
  Node *init; // 初期化式
};

// グローバル変数
extern GVar *globals;

// 文字列リテラルを管理する型
typedef struct Str Str;
struct Str {
  Str *next; // 次の変数かNULL
  char *literal; // 文字列リテラル
  Type *ty; // Type
  long num; // 何番目の文字列リテラルか
};

// 文字列リテラル
extern Str *strings;

typedef struct Struct Struct;
typedef struct MStruct MStruct;
// structの線形リスト(実装はType内に)
struct Struct {
  Struct *next;  // 次の構造体かNULL
  Type *decl;    // 宣言
};
// structのメンバ
struct MStruct {
  MStruct *next; // 次の構造体かNULL
  char *name;    // メンバの名前
  Type *ty;      // Type
  long len;       // 名前の長さ
  long offset;    // 構造体の先頭アドレスからのオフセット
};

// ローカル変数
extern Struct *Structs;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_REM,     // %
  ND_ASSIGN,  // =
  ND_ADDASGN, // +=
  ND_SUBASGN, // -=
  ND_MULASGN, // *=
  ND_DIVASGN, // /=
  ND_REMASGN, // %=
  ND_LOGOR,   // ||
  ND_LOGAND,  // &&
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
  ND_GVAR,    // グローバル変数
  ND_NUM,     // 整数
  ND_STR,     // 文字列リテラル
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの型
  Node *next;     // 次のノード
  Token *tok;     // このノードのトークン
  Type *ty;       // そのノードのタイプ,longやlongへのポインタなど
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  long val;        // kindがND_NUMの場合のみ使う
  Str *string;    // kindがND_STRの場合のみ使う
  LVar *lvar;     // kindがND_LVARの場合のみ使う
  GVar *gvar;     // kindがND_GVARの場合のみ使う
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
  Type *ret_ty;
  long stack_size;
};

// codegen_helper.c
long align_to(long n, long align);
void assign_lvar_offsets(struct Function *funcs);
void assign_string_literal_num();

// 関数
extern Function *functions;
extern Function *prototypes;

// 関数毎に内容、ローカル変数、ローカル変数用のスタックサイズの保存
typedef struct Program Program;
struct Program {
  Function *funcs;
};

Program *parse();

//
// type.c
//

typedef enum {
  TY_INT,
  TY_CHAR,
  TY_VOID,
  TY_PTR,
  TY_ARRAY,
  TY_FUNC,
  TY_STRUCT,
} TypeKind;

struct Type {
  TypeKind kind;
  long size;           // sizeof
  long align;          // alignof
 
  Type *base;         // 〇へのポインタ
  size_t array_size;  // 配列
  Type *elem;         // 配列
  long decayed;        // 配列->ポインタへの降格発生時のフラグ
  char *name;         // 定義
  Token *tok;         // 変数の位置情報など
  // 関数
  Type *return_ty;
  Type *params;
  Type *next;
  // 構造体
  char *tag_name;   // 構造体の名前
  long tag_len;      // 構造体の名前の長さ
  MStruct *member;  // メンバへのポインタ
};

Type *new_type(TypeKind kind);
Type *func_type(Type *return_ty);
long is_integer(Type *ty);
Type *pointer_to(Type *base);
Type *array_of(Type *elem, size_t size);
void add_type(Node *node);

//
// codegen.c
//

void codegen(Program *prog);
