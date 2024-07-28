#include "9cc.h"

LVar *locals;
GVar *globals;
Str *strings;

// ローカル変数を名前で検索する。見つからなかった場合はNULLを返す。
static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}
// グローバル変数を名前で検索する。見つからなかった場合はNULLを返す。
static GVar *find_gvar(Token *tok) {
  for (GVar *var = globals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->tok = token;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  node->tok = token;
  return node;
}

static Node *new_node_lvar(LVar *lvar, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->lvar = lvar;
  node->tok = tok;
  return node;
}
static Node *new_node_gvar(GVar *gvar, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_GVAR;
  node->gvar = gvar;
  node->tok = tok;
  return node;
}

static Node *new_node_str(Str *str, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_STR;
  node->string = str;
  node->tok = tok;
  return node;
}

static Type *arr_to_ptr(Type *ty) {
  Type *new_type = calloc(1, sizeof(Type));
  memcpy(new_type, ty, sizeof(Type));
  new_type->kind = TY_PTR;
  new_type->size = 8;
  new_type->base = new_type->elem;
  new_type->decayed = 1; // 降格

  return new_type;
}

static void decay_arr(Node *node) {
  // nodeに対してまだ型が与えられていない or nodeの型が配列である配列型をポインタに降格させる
  // 配列のローカル変数に対する降格
  if ((!node->ty || node->ty->kind == TY_ARRAY) && node->kind == ND_LVAR && node->lvar->ty->kind == TY_ARRAY) // このオペランドの型が配列だったらポインタにする
    node->ty = arr_to_ptr(node->lvar->ty); // ポインタに変換して付け替え
  // 配列のグローバル変数に対する降格
  if ((!node->ty || node->ty->kind == TY_ARRAY) && node->kind == ND_GVAR && node->gvar->ty->kind == TY_ARRAY)
    node->ty = arr_to_ptr(node->gvar->ty);
  // 文字列リテラルに対する降格
  if ((!node->ty || node->ty->kind == TY_ARRAY) && node->kind == ND_STR && node->string->ty->kind == TY_ARRAY)
    node->ty = arr_to_ptr(node->string->ty);
}

static LVar *new_lvar(char *name, Type *ty) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->name = name;
  lvar->ty = ty;
  lvar->len = strlen(name);
  lvar->next = locals;
  locals = lvar;
  return lvar;
}
static GVar *new_gvar(char *name, Type *ty) {
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->name = name;
  gvar->ty = ty;
  gvar->len = strlen(name);
  gvar->next = globals;
  globals = gvar;
  return gvar;
}

static Str *new_str(Token *tok) {
  Type *ty = array_of(new_type(TY_CHAR), tok->len-1);
  ty->tok = tok;

  Str *str = calloc(1, sizeof(Str));
  str->literal = strndup(tok->str, tok->len);
  str->ty = ty;
  str->next = strings;
  strings = str;
  return str;
}

static char *get_ident() {
  if (token->kind != TK_IDENT)
    error_at(token->str, "識別子が必要");
  Token *tok = token;
  token = token->next;
  return strndup(tok->str, tok->len);
}

// decl_basictype = ("int" | "char")
// 変数宣言のことも考慮して別の関数に振り分けた
static Type *decl_basictype() {
  if (consume_keyword("int"))
    return new_type(TY_INT);
  if (!consume_keyword("char"))
    error_at(token->str, "型名が必要");
  return new_type(TY_CHAR);
}

static Type *declarator(Type *ty);
Program *parse();
static Function *function();
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *logor();
static Node *logand();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *postfix();
static Node *primary();

// type-suffix = ("(" func-params? ")")
// func-params = param ("," param)*
// param       = declarator
static Type *type_suffix(Type *ty) {
  expect("(");
  Type head = {};
  Type *cur = &head;

  while (!consume(")")) {
    Type *basety = decl_basictype();
    Type *ty = declarator(basety);
    cur = cur->next = ty;
    consume(",");
  }

  ty = func_type(ty);
  ty->params = head.next;
  
  return ty;
}

// declarator = "*" * ident ( "[" number "]" )?
static Type *declarator(Type *ty) { // 宣言子
  while (consume("*"))
    ty = pointer_to(ty);

  Token *tok_lval = token;
  char *name = get_ident();
  if (consume("(")) // 宣言子の後ろに括弧が来たら関数定義、宣言として読み込み失敗
    return NULL;
  if (consume("[")) {
    ty = array_of(ty, expect_number());
    expect("]");
  }
  
  ty->name = name;
  ty->tok = tok_lval;
  return ty;
}
// function_declarator = "*" * ident type-suffix
static Type *function_declarator(Type *ty) { // 関数宣言子
  while (consume("*"))
    ty = pointer_to(ty);

  Token *tok_lval = token;
  char *name = get_ident();

  ty = type_suffix(ty); // 関数の宣言時の引数読み込み等
  ty->name = name;
  ty->tok = tok_lval;
  return ty;
}

// declaration = decl_basictype (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static Node *declaration() {
  Type *basety = decl_basictype();

  Node head = {};
  Node *cur = &head;

  while (!consume(";")) {
    Type *ty = declarator(basety);
    if (!ty) // 宣言として読み込み失敗
      return NULL;
    LVar *lvar = new_lvar(ty->name, ty);

    if (consume("=")) {
      Node *lhs = new_node_lvar(lvar, ty->tok);
      Node *rhs = assign();
      decay_arr(rhs); // 右辺が配列ならポインタに降格
      cur = cur->next = new_node(ND_ASSIGN, lhs, rhs);
    }

    consume(",");
  }

  Node *node = new_node(ND_BLOCK, NULL, NULL);
  node->body = head.next;
  return node;
}
// gvar_declaration = decl_basictype (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static Node *gvar_declaration() {
  Type *basety = decl_basictype();

  Node head = {};
  Node *cur = &head;

  while (!consume(";")) {
    Type *ty = declarator(basety);
    if (!ty) // 宣言として読み込み失敗
      return NULL;
    GVar *gvar = new_gvar(ty->name, ty);

    if (consume("=")) {
      Node *lhs = new_node_gvar(gvar, ty->tok);
      Node *rhs = assign();
      decay_arr(rhs); // 右辺が配列ならポインタに降格
      cur = cur->next = new_node(ND_ASSIGN, lhs, rhs);
    }

    consume(",");
  }

  Node *node = new_node(ND_BLOCK, NULL, NULL);
  node->body = head.next;
  return node;
}

static void create_param_lvars(Type *param) {
  if (param) {
    create_param_lvars(param->next);
    new_lvar(param->name, param);
  }
}

// program = (function | gvar_declaration ";")*
Program *parse() {
  Program *prog = calloc(1, sizeof(Program));
  Function func_head = {};
  Function *func_cur = &func_head;
  Node gvar_head = {};
  Node *gvar_cur = &gvar_head;

  while (!at_eof()) {
    Token *origin = token; // 解析失敗時はトークンを元に戻す
    Node *globalvar = gvar_declaration();
    if (globalvar) { // グローバル変数宣言として読むことができるか
      globalvar->kind = ND_GVAR;
      gvar_cur = gvar_cur->next = globalvar;
      continue;
    }
    token = origin;
    func_cur = func_cur->next = function();
  }
    //error("正しくパースできませんでした。");

  prog->funcs = func_head.next;
  prog->gvar_declarations = gvar_head.next;
  return prog;
}

// function = decl_basictype function_declarator stmt
static Function *function() { // Function definition
  Type *ty = decl_basictype();
  ty = function_declarator(ty);

  locals = NULL; // NULLのときが終端とするため

  Function *fn = calloc(1, sizeof(Function));
  fn->name = ty->name;
  create_param_lvars(ty->params);
  fn->params = locals;

  if (!at_block())
    error("中括弧で覆われていません。");
  fn->body = stmt();
  fn->locals = locals;
  return fn;
}

// stmt = expr? ";"
//      | "{" (declaration | stmt)* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "return" expr ";"
static Node *stmt() {
  Node *node;

  if (consume("{")) {
    Node head;
    head.next = NULL;
    Node *cur = &head;
    while (!consume("}")) {
      if (equal_keyword("int") || equal_keyword("char"))
        cur = cur->next = declaration();
      else
        cur = cur->next = stmt();
      add_type(cur);
    }
    
    node = new_node(ND_BLOCK, NULL, NULL);
    node->body = head.next;
    return node;
  } else if (consume_keyword("if")) {
    node = new_node(ND_IF, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume_keyword("else"))
      node->els = stmt();
    return node;
  } else if (consume_keyword("for")) {
    node = new_node(ND_LOOP, NULL, NULL);
    expect("(");
    node->init = stmt(); // ";"の部分も含めてstmt
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    }
    node->then = stmt();
    return node;
  } else if (consume_keyword("while")) {
    node = new_node(ND_LOOP, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  } else if (consume_keyword("return")) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else if (consume(";")) { // ";"だけの文
    return new_node(ND_BLOCK, NULL, NULL); // 空のブロック
  } else {
    node = expr();
  }

  expect(";");
  return node;
}

// expr = assign
static Node *expr() {
  return assign();
}

// assign = logor ("=" assign)?
static Node *assign() {
  Node *node = logor();
  if (consume("=")) {
    Node *rhs = assign();
    decay_arr(rhs); // 右辺が配列ならポインタに降格
    node = new_node(ND_ASSIGN, node, rhs);
  }
  return node;
}

// logor = logand ("||" logand)*
static Node *logor() {
  Node *node = logand();
  
  for (;;) {
    if (consume("||"))
      node = new_node(ND_LOGOR, node, logand());
    else
      return node;
  }
}

// logand = equality ("&&" equality)*
static Node *logand() {
  Node *node = equality();
  
  for (;;) {
    if (consume("&&"))
      node = new_node(ND_LOGAND, node, equality());
    else
      return node;
  }
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

// ポインタ演算の場合、pをポインタ、nを整数とすると
// p + nはpにnをそのまま加算するのではなく、
// sizeof(*p)*nをpの値に加算する。
static Node *new_add(Node *lhs, Node *rhs) {
  add_type(lhs);
  add_type(rhs);

  // num + num
  if (is_integer(lhs->ty) && is_integer(rhs->ty))
    return new_node(ND_ADD, lhs, rhs);

  // 配列に対するaddは先頭要素のアドレスに降格
  decay_arr(lhs); // 配列ならポインタに降格
  decay_arr(rhs);

  // ptr + ptr は計算できない
  if (lhs->ty->base && rhs->ty->base)
    error_at(token->str, "無効な演算(pointer+pointer)");

  // num + ptr を ptr + num にひっくり返す.
  if (!lhs->ty->base && rhs->ty->base) {
    Node *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // ptr + num
  rhs = new_node(ND_MUL, rhs, new_node_num(lhs->ty->base->size)); // シフト演算子を定義したら変更する
  return new_node(ND_ADD, lhs, rhs);
}

// -演算子にもポインタと整数に対しての演算がある
static Node *new_sub(Node *lhs, Node *rhs) {
  add_type(lhs);
  add_type(rhs);

  // num - num
  if (is_integer(lhs->ty) && is_integer(rhs->ty))
    return new_node(ND_SUB, lhs, rhs);

  // 配列に対するsubは先頭要素のアドレスに降格
  decay_arr(lhs); // 配列ならポインタに降格
  decay_arr(rhs);

  // ptr - num
  if (lhs->ty->base && is_integer(rhs->ty)) {
    rhs = new_node(ND_MUL, rhs, new_node_num(lhs->ty->base->size)); // シフト演算子を定義したら変更する
    add_type(rhs);
    Node *node = new_node(ND_SUB, lhs, rhs);
    node->ty = lhs->ty;
    return node;
  }

  // ptr - ptr
  // 2つのポインタ間の要素数を返す
  if (lhs->ty->base && rhs->ty->base) {
    Node *node = new_node(ND_SUB, lhs, rhs);
    node->ty = new_type(TY_INT);
    return new_node(ND_DIV, node, new_node_num(lhs->ty->base->size)); // シフト演算子を定義したら変更する
  }

  error_at(token->str, "無効な演算");
  return NULL;
}

// add = mul ("+" mul | "-" mul)*
static Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_add(node, mul());
      // ポインタ演算との場合分けなどの関数
    else if (consume("-"))
      node = new_sub(node, mul());
      // ポインタ演算との場合分けなどの関数
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary | "%" unary)*
static Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else if (consume("%"))
      node = new_node(ND_REM, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? postfix
//       | "*" unary
//       | "&" unary 
//       | postfix
static Node *unary() {
  if (consume("+"))
    return postfix();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), postfix());
  if (consume("*")) {
    Node *node = unary();
    decay_arr(node); // 配列ならポインタに降格
    return new_node(ND_DEREF, node, NULL);
  }
  if (consume("&"))
    return new_node(ND_ADDR, unary(), NULL);
  return postfix();
}

// postfix = primary ("[" expr "]")*
static Node *postfix() {
  Node *node = primary();
  while (consume("[")) {
    node = new_node(ND_DEREF, new_add(node, expr()), NULL);
    expect("]");
  }
  return node;
}

// funcall = "(" (assign ("," assign)*)? ")"
static Node *funcall(Token *tok) {
  Node head = {};
  Node *cur = &head;

  while (!consume(")")) {
    cur = cur->next = assign();
    decay_arr(cur);
    consume(",");
  }

  Node *node = new_node(ND_FUNCALL, NULL, NULL);
  node->funcname = strndup(tok->str, tok->len);
  node->args = head.next;
  return node;  
}

// primary = num | string_literal | ident funcall? | "(" expr ")" | "sizeof" unary
static Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  if (consume_keyword("sizeof")) {
    Node *node = unary();
    add_type(node);
    return new_node_num(node->ty->size);
  }

  Token *tok = consume_ident();
  if (tok) {
    // 関数呼び出し
    if (consume("(")) {
      return funcall(tok);
    }

    LVar *lvar = find_lvar(tok);
    if (lvar) 
      return new_node_lvar(lvar, tok);
    // 見つからなければグローバル変数を探す
    GVar *gvar = find_gvar(tok);
    if (!gvar) 
      error_at(token->str, "未定義の変数");

    return new_node_gvar(gvar, tok);
  }

  tok = consume_str();
  if (tok) {
    Str *string = new_str(tok);
    return new_node_str(string, tok);
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());

}
