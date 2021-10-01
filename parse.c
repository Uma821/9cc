#include "9cc.h"

LVar *locals;

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
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

static char *get_ident() {
  if (token->kind != TK_IDENT)
    error_at(token->str, "関数名が必要");
  Token *tok = token;
  token = token->next;
  return strndup(tok->str, tok->len);
}

static Type *declarator(Type *ty);
Function *parse();
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

// type-suffix = ("(" func-params? ")")?
// func-params = param ("," param)*
// param       = declarator
static Type *type_suffix(Type *ty) {
  if (consume("(")) {
    Type head = {};
    Type *cur = &head;

    while (!consume(")")) {
      Type *basety = new_type(TY_INT); // 引数は暗黙のint
      Type *ty = declarator(basety);
      cur = cur->next = copy_type(ty);
      consume(",");
    }

    ty = func_type(ty);
    ty->params = head.next;
  }
  return ty;
}

// declarator = ident type-suffix
static Type *declarator(Type *ty) { // 宣言
  char* name = get_ident();
  
  ty = type_suffix(ty); // 関数の宣言だった時の引数読み込み等
  ty->name = name;
  return ty;
}

static void create_param_lvars(Type *param) {
  if (param) {
    create_param_lvars(param->next);
    new_lvar(param->name, param);
  }
}

static Function *function() {
  Type *ty = new_type(TY_INT); // 戻り値は暗黙のint
  ty = declarator(ty);

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

// program = stmt*
Function *parse() {
  Function head = {};
  Function *cur = &head;

  while (!at_eof())
    cur = cur->next = function();
    //error("正しくパースできませんでした。");
  return head.next;
}

// stmt = expr? ";"
//      | "{" stmt* "}"
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
    while (!consume("}"))
      cur = cur->next = stmt();
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

// assign = equality ("=" assign)?
static Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
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

// add = mul ("+" mul | "-" mul)*
static Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? primary
//       | "*" unary
//       | "&" unary 
static Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  if (consume("*"))
    return new_node(ND_DEREF,unary(), NULL);
  if (consume("&"))
    return new_node(ND_ADDR, unary(), NULL);
  return primary();
}

// funcall = "(" (assign ("," assign)*)? ")"
static Node *funcall(Token *tok) {
  Node head = {};
  Node *cur = &head;

  while (!consume(")")) {
    cur = cur->next = assign();
    consume(",");
  }

  Node *node = new_node(ND_FUNCALL, NULL, NULL);
  node->funcname = strndup(tok->str, tok->len);
  node->args = head.next;
  return node;  
}

// primary = num | ident funcall? | "(" expr ")"
static Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    // 関数呼び出し
    if (consume("(")) {
      return funcall(tok);
    }

    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->lvar = lvar;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      node->lvar = lvar;
      locals = lvar;
    }
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());

}
