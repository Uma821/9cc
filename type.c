#include "9cc.h"

Type *new_type(TypeKind kind) {
  Type *type = calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}

Type *func_type(Type *return_ty) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_FUNC;
  ty->return_ty = return_ty;
  return ty;
}

bool is_integer(Type *ty) {
  return ty->kind == TY_INT;
}

Type *pointer_to(Type *base) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_PTR;
  ty->base = base;
  return ty;
}

// ポインタの演算と整数型の演算では処理が変わるため
void add_type(Node *node) {
  if (!node || node->ty)
    return;

  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);

  for (Node *n = node->body; n; n = n->next)
    add_type(n);

  switch (node->kind) {
  case ND_ADD: // new_addでひっくり返すから問題なし
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
    node->ty = node->lhs->ty;
    return;
  case ND_ASSIGN:
    if (node->lhs->ty->kind != node->rhs->ty->kind)
      error_at(node->lhs->tok->str, "代入不可");
    node->ty = node->lhs->ty;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_NUM:
  case ND_FUNCALL:
    node->ty = new_type(TY_INT);
    return;
  case ND_LVAR:
    node->ty = node->lvar->ty;
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
