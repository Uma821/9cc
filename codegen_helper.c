

// typedef struct Type Type;
// typedef struct Node Node;

// errno.h プロトタイプ宣言
long *__errno_location();
// ctype.h プロトタイプ宣言
int ispunct(int c);
int isspace(int c);
int isdigit(int c);

//
// tokenize.c
//

struct Type;
struct LVar;
struct GVar;
struct Function;
struct Str;
struct Node;
struct Token;
struct Struct;
struct MStruct;
struct Enum;

// トークンの種類
/*typedef*/ enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_STR,      // 文字列リテラル
  TK_KEYWORD,  // キーワード
  TK_EOF,      // 入力の終わりを表すトークン
} /*TokenKind*/;

// トークン型
// typedef struct Token Token;
struct Token {
  long /*TokenKind*/ kind; // トークンの型
  struct Token *next;    // 次の入力トークン
  long val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  long len;        // トークンの長さ
};

// 入力プログラム
// extern char *user_input;
// 入力ファイル名
// extern char *filename;

// 現在着目しているトークン
// extern struct Token *token;

// void error(char *fmt, ...);
// void error_at(char *loc, char *fmt, ...);
long consume(char *op);
long consume_keyword(char *op);
long equal_keyword(char *op);
void expect(char *op);
long expect_number();
long at_eof();
long at_block();
struct Token *consume_ident();
struct Token *consume_str();
void tokenize();

//
// parse.c
//

// ローカル変数の型
// typedef struct LVar LVar;
struct LVar {
  struct LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  struct Type *ty;   // Type
  long len;    // 名前の長さ
  long offset; // RBPからのオフセット
};

// ローカル変数
// extern struct LVar *locals;

// グローバル変数の型
// typedef struct GVar GVar;
struct GVar {
  struct GVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  struct Type *ty;   // Type
  long len;    // 名前の長さ
  struct Node *init; // 初期化式
};

// グローバル変数
// extern struct GVar *globals;

// 文字列リテラルを管理する型
// typedef struct Str Str;
struct Str {
  struct Str *next; // 次の変数かNULL
  char *literal; // 文字列リテラル
  struct Type *ty; // Type
  long num; // 何番目の文字列リテラルか
};

// 文字列リテラル
// extern struct Str *strings;

// typedef struct Struct Struct;
// typedef struct MStruct MStruct;
// structの線形リスト(実装はType内に)
struct Struct {
  struct Struct *next;  // 次の構造体かNULL
  struct Type *decl;    // 宣言
};
// structのメンバ
struct MStruct {
  struct MStruct *next; // 次の構造体かNULL
  char *name;    // メンバの名前
  struct Type *ty;      // Type
  long len;       // 名前の長さ
  long offset;    // 構造体の先頭アドレスからのオフセット
};

// ローカル変数
// extern struct Struct *Structs;

// 抽象構文木のノードの種類
/*typedef*/ enum {
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
} /*NodeKind*/;

// typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  long /*NodeKind*/ kind;  // ノードの型
  struct Node *next;     // 次のノード
  struct Token *tok;     // このノードのトークン
  struct Type *ty;       // そのノードのタイプ,longやlongへのポインタなど
  struct Node *lhs;      // 左辺
  struct Node *rhs;      // 右辺
  long val;        // kindがND_NUMの場合のみ使う
  struct Str *string;    // kindがND_STRの場合のみ使う
  struct LVar *lvar;     // kindがND_LVARの場合のみ使う
  struct GVar *gvar;     // kindがND_GVARの場合のみ使う
  struct Node *cond;     // kindがND_IF,ND_LOOPの場合のみ使う
  struct Node *then;     // kindがND_IF,ND_LOOPの場合のみ使う
  struct Node *els;      // kindがND_IFの場合のみ使う
  struct Node *init;     // kindがND_LOOPの場合のみ使う
  struct Node *inc;      // kindがND_LOOPの場合のみ使う
  struct Node *body;     // kindがND_BLOCK(複文)の場合のみ使う
  char *funcname; // kindがND_FUNCALLの場合のみ使う
  struct Node *args;     // kindがND_FUNCALLの場合のみ使う
};

// 関数毎に内容、ローカル変数、ローカル変数用のスタックサイズの保存
// typedef struct Function Function;
struct Function {
  struct Function *next;
  char *name;
  struct LVar *params;

  struct Node *body;
  struct LVar *locals;
  struct Type *ret_ty;
  long stack_size;
};

// Enum中に出てきた識別子を覚えておく型
// typedef struct Enum Enum;
struct Enum {
  struct Enum *next; // 次の変数かNULL
  char *name; // 識別子の名前
  struct Node *node; // 値定義
};

// codegen_helper.c
long align_to(long n, long align);
void assign_lvar_offsets(struct Function *funcs);
void assign_string_literal_num();

// 関数
// extern struct Function *functions;
// extern struct Function *prototypes;

// 関数毎に内容、ローカル変数、ローカル変数用のスタックサイズの保存
// typedef struct Program Program;
struct Program {
  struct Function *funcs;
};

struct Program *parse();

//
// type.c
//

/*typedef*/ enum {
  TY_INT,
  TY_CHAR,
  TY_VOID,
  TY_PTR,
  TY_ARRAY,
  TY_FUNC,
  TY_STRUCT,
} /*TypeKind*/;

struct Type {
  long /*TypeKind*/ kind;
  long size;           // sizeof
  long align;          // alignof
 
  struct Type *base;         // 〇へのポインタ
  long array_size;  // 配列
  struct Type *elem;         // 配列
  long decayed;        // 配列->ポインタへの降格発生時のフラグ
  char *name;         // 定義
  struct Token *tok;         // 変数の位置情報など
  // 関数
  struct Type *return_ty;
  struct Type *params;
  struct Type *next;
  // 構造体
  char *tag_name;   // 構造体の名前
  long tag_len;      // 構造体の名前の長さ
  struct MStruct *member;  // メンバへのポインタ
};

struct Type *new_type(long /*TypeKind*/ kind);
struct Type *func_type(struct Type *return_ty);
long is_integer(struct Type *ty);
struct Type *pointer_to(struct Type *base);
struct Type *array_of(struct Type *elem, long size);
void add_type(struct Node *node);

//
// codegen.c
//

void codegen(struct Program *prog);
// #include "9cc.h"

// 'n'を'align'の最も近い倍数に切り上げる。
// align_to(5, 8) == 8, align_to(11, 8) == 16.
long align_to(long n, long align) {
  return (n + align - 1) / align * align;
}

char *get_reg(long i) {
  char *argreg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  return argreg[i];
}
static struct Function *current_fn;

static void gen();

long get_type_size(struct Type *ty);
struct Str *get_strings();
struct GVar *get_globals();

// ローカル変数にオフセット割り当て
void assign_lvar_offsets(struct Function *funcs) {
  for (struct Function *fn = funcs; fn; fn = fn->next) {
    long offset = 0;
    for (struct LVar *lvar = fn->locals; lvar; lvar = lvar->next) {
      offset += get_type_size(lvar->ty);
      // if(get_type_size(lvar->ty) == 0)
      //   fprintf(stderr, "size==0???\n");
      lvar->offset = offset;
    }
    fn->stack_size = align_to(offset, 16); // stack_sizeを16の倍数にする(RSPを16の倍数にしなければならないらしい)
  }
}

// 文字列リテラルに番号を振り分ける
void assign_string_literal_num() {
  long number = 0;
  for (struct Str *str = get_strings(); str; str = str->next)
    str->num = number++;
}

long codegen_global_count = 1;
static long count() {
  return codegen_global_count++;
}

static void gen_variable(struct Node *node) {
  if (node->kind == ND_LVAR) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %ld\n", node->lvar->offset);
    printf("  push rax\n");
    return 0;
  } else if (node->kind == ND_GVAR) {
    printf("  push offset %s\n", node->gvar->name);
    return 0;
  } else if (node->kind == ND_DEREF) {
    gen(node->lhs);
    return 0;
  } else {
    error("代入の左辺値が変数ではありません");
  }
  return 0;
}

static void gen(struct Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %ld\n", node->val);
    return 0;
  } else if (node->kind == ND_STR) {
    printf("  push offset .LC%ld\n", node->string->num);
    return 0;
  } else if (node->kind == ND_LVAR || node->kind == ND_GVAR) {
    gen_variable(node);
    if(!node->ty->decayed) { // 配列からポインタへの降格？暗黙型変換が生じたか？
    // 配列は先頭要素へのアドレスを返せばよい、以下3行はその中身を取り出すため必要ない
      printf("  pop rax\n");
      if(node->ty->size == 1) {
        printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
        printf("  push rax\n");
      } else {
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
      }
    }
    return 0;
  } else if (node->kind == ND_ADDR) {
    gen_variable(node->lhs);
    return 0;
  } else if (node->kind == ND_DEREF) {
    gen(node->lhs);
    printf("  pop rax\n");
    if(node->ty->size == 1) {
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
      printf("  push rax\n");
    } else {
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
    }
    return 0;
  } else if (node->kind == ND_LOGOR) {
    long c = count();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .L.logor.t.%ld\n", c); // 短絡評価
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .L.logor.t.%ld\n", c);
    printf("  mov rax, 0\n");
    printf("  jmp .L.logor.end.%ld\n", c);
    printf(".L.logor.t.%ld:\n", c);
    printf("  mov rax, 1\n");
    printf(".L.logor.end.%ld:\n", c);
    printf("  push rax\n");
    return 0;
  } else if (node->kind == ND_LOGAND) {
    long c = count();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.logand.f.%ld\n", c); // 短絡評価
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.logand.f.%ld\n", c);
    printf("  mov rax, 1\n");
    printf("  jmp .L.logand.end.%ld\n", c);
    printf(".L.logand.f.%ld:\n", c);
    printf("  mov rax, 0\n");
    printf(".L.logand.end.%ld:\n", c);
    printf("  push rax\n");
    return 0;
  }

  else if (node->kind == ND_ASSIGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    if(node->lhs->ty->size == 1) {
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_ADDASGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  push rax\n"); // アドレス保存

    if(node->lhs->ty->size == 1)
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
    else
      printf("  mov rax, [rax]\n");

    printf("  add rdi, rax\n");

    if(node->lhs->ty->size == 1) {
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_SUBASGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  push rax\n"); // アドレス保存

    if(node->lhs->ty->size == 1)
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
    else
      printf("  mov rax, [rax]\n");

    printf("  sub rax, rdi\n");
    printf("  mov rdi, rax\n");

    if(node->lhs->ty->size == 1) {
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_MULASGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  push rax\n"); // アドレス保存

    if(node->lhs->ty->size == 1)
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
    else
      printf("  mov rax, [rax]\n");

    printf("  imul rdi, rax\n");

    if(node->lhs->ty->size == 1) {
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_DIVASGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  push rax\n"); // アドレス保存

    if(node->lhs->ty->size == 1)
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
    else
      printf("  mov rax, [rax]\n");

    printf("  cqo\n"); // RAXに入っている64ビットの値を128ビットに伸ばしてRDXとRAXにセット
    printf("  idiv rdi\n"); // 被除数は[RDX,RAX] (128)ビット整数とみなす
    printf("  mov rdi, rax\n"); // 商はraxレジスタに格納される

    if(node->lhs->ty->size == 1) {
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_REMASGN) {
    gen_variable(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  push rax\n"); // アドレス保存

    if(node->lhs->ty->size == 1)
      printf("  movsx rax, BYTE PTR [rax]\n"); // signed char
    else
      printf("  mov rax, [rax]\n");

    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rdi, rdx\n"); // 剰余はrdxレジスタに格納される

    if(node->lhs->ty->size == 1) {
      printf("  pop rax\n");
      printf("  mov [rax], dil\n"); // 8ビットレジスタに変換
      printf("  push rdi\n"); // 先頭56ビットを0埋めできてない
    } else {
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
    }
    return 0;
  } else if (node->kind == ND_BLOCK) {
    for (struct Node *n = node->body; n; n = n->next) {
      gen(n);
      printf("  pop rax\n");
    }
    printf("  push rax\n");
    return 0;
  } else if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  jmp .L.return.%s\n", current_fn->name);
    return 0;
  }

  else if (node->kind == ND_IF) {
    long c = count();
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .L.else.%ld\n", c);
    gen(node->then);
    printf("  pop rax\n");
    printf("  jmp .L.end.%ld\n", c);
    printf(".L.else.%ld:\n", c);
    if (node->els){
      gen(node->els);
      printf("  pop rax\n");
    }
    printf(".L.end.%ld:\n", c);
    printf("  push rax\n");
    return 0;
  } else if (node->kind == ND_LOOP) {
    long c = count();
    if (node->init) {
      gen(node->init);
      printf("  pop rax\n");
    }
    printf(".L.begin.%ld:\n", c);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .L.end.%ld\n", c);
    }
    gen(node->then);
    printf("  pop rax\n");
    if (node->inc){
      gen(node->inc);
      printf("  pop rax\n");
    }
    printf("  jmp .L.begin.%ld\n", c);
    printf(".L.end.%ld:\n", c);
    printf("  push rax\n");
    return 0;
  } else if (node->kind == ND_FUNCALL) {
    long nargs = 0;
    for (struct Node *arg = node->args; arg; arg = arg->next) {
      gen(arg);
      nargs++;
    }

    for (long i = nargs - 1; i >= 0; i--)
      printf("  pop %s\n", get_reg(i));
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->funcname);
    printf("  push rax\n");
    return 0;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  if (node->kind == ND_ADD)
    printf("  add rax, rdi\n");
  else if (node->kind == ND_SUB)
    printf("  sub rax, rdi\n");
  else if (node->kind == ND_MUL)
    printf("  imul rax, rdi\n");
  else if (node->kind == ND_DIV) {
    printf("  cqo\n"); // RAXに入っている64ビットの値を128ビットに伸ばしてRDXとRAXにセット
    printf("  idiv rdi\n"); // 被除数は[RDX,RAX] (128)ビット整数とみなす
  } else if (node->kind == ND_REM) {
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rax, rdx\n"); // 剰余はrdxレジスタに格納される
  } else if (node->kind == ND_EQ) {
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
  } else if (node->kind == ND_NE) {
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
  } else if (node->kind == ND_LT) {
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
  } else if (node->kind == ND_LE) {
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
  }

  printf("  push rax\n");
}

void codegen(struct Program *prog) {
  assign_lvar_offsets(prog->funcs);
  assign_string_literal_num();
  printf(".intel_syntax noprefix\n");

  printf(".data\n");
  for (struct GVar *gvar = get_globals(); gvar; gvar = gvar->next) {
    // グローバル変数を定義
    printf("%s:\n", gvar->name);
    struct Node *gvar_init = gvar->init;
    if (gvar_init) {
      if (gvar_init->kind == ND_BLOCK) { // 配列
        for (struct Node *n = gvar_init->body; n; n = n->next) {
          if (n->kind == ND_STR && n->ty->kind == TY_ARRAY) { // 文字列リテラル
            printf("  .string %s\n", n->string->literal);
          } else {

            if (gvar->ty->elem->kind == TY_INT)
              printf("  .quad "); // 8バイト
            else if (gvar->ty->elem->kind == TY_CHAR || gvar->ty->elem->kind == TY_VOID)
              printf("  .byte ");
            else if (gvar->ty->elem->kind == TY_PTR)
              printf("  .quad ");

            if (n->kind == ND_NUM)
              printf("%ld\n", n->val);
            else if (n->kind == ND_STR)
              printf(".LC%ld\n", n->string->num);
          }
        }
      } else { // 配列以外の初期化
        if (gvar->ty->kind == TY_INT)
          printf("  .quad "); // 8バイト
        else if (gvar->ty->kind == TY_CHAR || gvar->ty->kind == TY_VOID)
          printf("  .byte ");
        else if (gvar->ty->kind == TY_PTR)
          printf("  .quad ");

        if (gvar_init->kind == ND_NUM)
          printf("%ld\n", gvar_init->val);
        else if (gvar_init->kind == ND_STR)
          printf(".LC%ld\n", gvar_init->string->num);
      }
    } else {
      printf("  .zero %ld\n", gvar->ty->size);
    }
  }
  for (struct Str *str = get_strings(); str; str = str->next) {
    // 文字列リテラルを配置
    printf(".LC%ld:\n", str->num);
    printf("  .string %s\n", str->literal);
  }

  printf(".text\n");
  for (struct Function *fn = prog->funcs; fn; fn = fn->next) {
    // アセンブリの前半部分を出力
    printf(".globl %s\n", fn->name);
    printf("%s:\n", fn->name);
    current_fn = fn;
  
    // プロローグ
    // 使用した変数分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %ld\n", fn->stack_size);

    // レジスタによって渡された引数をスタックに保存する
    long i = 0;
    for (struct LVar *lvar = fn->params; lvar; lvar = lvar->next) {
      printf("  mov rax, rbp\n");
      printf("  sub rax, %ld\n", lvar->offset);
      printf("  mov [rax], %s\n", get_reg(i++));
    }

    gen(fn->body);
    printf("  pop rax\n");
  
    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf(".L.return.%s:\n", fn->name);
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
  }
}
