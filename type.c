#include "9cc.h"

struct Type *new_type(long /*TypeKind*/ kind) {
  struct Type *ty = calloc(1, sizeof(struct Type));
  ty->kind = kind;
  if (kind == TY_CHAR || kind == TY_VOID)
    ty->align = ty->size = 1; // sizeof(char) == 1
  else
    ty->align = ty->size = 8;
  return ty;
}

struct Type *func_type(struct Type *return_ty) {
  struct Type *ty = calloc(1, sizeof(struct Type));
  ty->kind = TY_FUNC;
  ty->return_ty = return_ty;
  return ty;
}

long is_integer(struct Type *ty) {
  return ty->kind == TY_INT || ty->kind == TY_CHAR || ty->kind == TY_VOID;
}

// bool is_lvalue(struct Node *node) {
//   return node->kind == ND_LVAR || node->kind == ND_GVAR;
// }

struct Type *pointer_to(struct Type *base) {
  struct Type *ty = calloc(1, sizeof(struct Type));
  ty->kind = TY_PTR;
  ty->align = ty->size = 8;
  ty->base = base;
  return ty;
}

struct Type *array_of(struct Type *elem, size_t array_size) {
  struct Type *ty = calloc(1, sizeof(struct Type));
  ty->kind = TY_ARRAY;
  ty->size = array_size * elem->size; // sizeof
  ty->align = elem->align;
  ty->array_size = array_size; // 要素数
  ty->elem = elem;
  return ty;
}

long get_type_size(struct Type *ty) {
  return ty->size;
}
struct Str *get_strings() {
  return strings;
}
struct GVar *get_globals() {
  return globals;
}
struct Token *get_token() {
  return token;
}
void set_token(struct Token *t) {
  token = t;
  return;
}

static struct Function* find_function(char *funcname) {
  for (struct Function *fn = functions; fn; fn = fn->next) {
    if (!strcmp(funcname, fn->name))
      return fn;
  }
  for (struct Function *fn = prototypes; fn; fn = fn->next) {
    if (!strcmp(funcname, fn->name))
      return fn;
  }
  return NULL;
}

static struct Type* detect_func_rettype(char *funcname) {
  struct Function *fn = find_function(funcname);
  if (fn) return fn->ret_ty;
  return new_type(TY_INT);
}

// ポインタの演算と整数型の演算では処理が変わるため
void add_type(struct Node *node) {
  if (!node || node->ty)
    return;

  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);

  for (struct Node *n = node->body; n; n = n->next)
    add_type(n);
  for (struct Node *n = node->args; n; n = n->next)
    add_type(n);

  switch (node->kind) {
  case ND_ADD: // new_addでひっくり返すから問題なし
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_REM:
  case ND_ADDASGN:
  case ND_SUBASGN:
  case ND_MULASGN:
  case ND_DIVASGN:
  case ND_REMASGN:
    node->ty = node->lhs->ty;
    return;
  case ND_ASSIGN:
    if (node->lhs->ty->kind != node->rhs->ty->kind)
      if (node->lhs->ty->kind == TY_CHAR && node->rhs->ty->kind == TY_INT) {} // 変換可能
      else error_at(node->lhs->tok->str, "代入不可");
    node->ty = node->lhs->ty;
    return;
  case ND_LOGOR:
  case ND_LOGAND:
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_NUM:
    node->ty = new_type(TY_INT);
    return;
  case ND_FUNCALL:
    node->ty = detect_func_rettype(node->funcname);
    return;
  case ND_LVAR:
    if(!node->ty) // 降格が起こっていない(デフォルト)
      node->ty = node->lvar->ty;
    return;
  case ND_GVAR:
    if(!node->ty) // 降格が起こっていない(デフォルト)
      node->ty = node->gvar->ty;
    return;
  case ND_STR:
    if(!node->ty) // 降格が起こっていない(デフォルト)
      node->ty = node->string->ty;
    return;
  case ND_ADDR:
    node->ty = pointer_to(node->lhs->ty);
    return;
  case ND_DEREF:
    if (node->lhs->ty->kind != TY_PTR)
      error_at(node->lhs->tok->str, "間接演算子の型として無効");
    node->ty = node->lhs->ty->base;
    return;
  default:
    return;
  }
}
