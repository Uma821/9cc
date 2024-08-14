// 'n'を'align'の最も近い倍数に切り上げる。
// align_to(5, 8) == 8, align_to(11, 8) == 16.
long align_to(long n, long align) {
  return (n + align - 1) / align * align;
}

struct Type;
struct LVar;
struct Function;
struct Str;
struct Node;

struct LVar {
  struct LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  struct Type *ty;   // Type
  long len;    // 名前の長さ
  long offset; // RBPからのオフセット
};

struct Function {
  struct Function *next;
  char *name;
  struct LVar *params;

  struct Node *body;
  struct LVar *locals;
  struct Type *ret_ty;
  long stack_size;
};

struct Str {
  struct Str *next; // 次の変数かNULL
  char *literal; // 文字列リテラル
  struct Type *ty; // Type
  long num; // 何番目の文字列リテラルか
};

long get_type_size(struct Type *ty);
struct Str *get_strings();

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