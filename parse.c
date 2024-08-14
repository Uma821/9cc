#include "9cc.h"

LVar *locals;
GVar *globals;
Str *strings;
Struct *struct_defs;
Struct *struct_decls;
Function *functions;
Function *prototypes;

static size_t strnlen(const char *s, size_t n) {
	const char *p = memchr(s, 0, n);
	return p ? p-s : n;
}

static char *strndup(const char *s, size_t n) {
	size_t l = strnlen(s, n);
	char *d = malloc(l+1);
	if (!d) return NULL;
	memcpy(d, s, l);
	d[l] = 0;
	return d;
}

// static Struct *reverse_struct(Struct* head) {
//   Struct *prev = NULL;
//   Struct *current = head;
//   Struct *next = NULL;

//   while (current != NULL) {
//     next = current->next; // 次のノードを保存
//     current->next = prev; // currentの次をprevに変更
//     prev = current;       // prevをcurrentに移動
//     current = next;       // currentを次のノードに移動
//   }

//   head = prev;
//   return head;
// }

// 構造体のリストから指定したタグ名に合う構造体を見つける
static Type *find_struct(char *ident) {
  for (Struct *_struct = struct_defs; _struct; _struct = _struct->next) {
    if (!strncmp(ident, _struct->decl->tag_name, _struct->decl->tag_len))
      return _struct->decl;
  }
  for (Struct *_struct = struct_decls; _struct; _struct = _struct->next) {
    if (!strncmp(ident, _struct->decl->tag_name, _struct->decl->tag_len))
      return _struct->decl;
  }
  error_at(token->str, "構造体宣言が見つかりません");
  return NULL;
}

// 構造体のメンバ群から指定したメンバを探す
static MStruct *find_member(Type *struct_ty, char *ident) {
  for (MStruct *mem = struct_ty->member; mem; mem = mem->next) {
    if (!strncmp(ident, mem->name, mem->len))
      return mem;
  }
  error_at(token->str, "メンバが見つかりません");
  return NULL;
}

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

static Node *new_node_num(long val) {
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

// static void integer_promotion(Node *lhs, Node *rhs) {
//   // オペランドの型をlongに揃える
//   if(is_integer(lhs->ty))
//     lhs->ty->kind = TY_INT;
//   if(is_integer(rhs->ty))
//     rhs->ty->kind = TY_INT;
// }

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
static MStruct *new_mstruct(char *name, Type *ty) {
  MStruct *smem = calloc(1, sizeof(MStruct));
  smem->name = name;
  smem->ty = ty;
  smem->len = strlen(name);
  return smem;
}
static Struct *new_struct(char *name) {
  Struct *_struct = calloc(1, sizeof(Struct));
  _struct->decl = calloc(1, sizeof(Type));
  _struct->decl->kind = TY_STRUCT;
  _struct->decl->tag_name = name;
  _struct->decl->tag_len = strlen(name);
  return _struct;
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

// decl_basictype = ("int" | "char" | "long" | "void" | "struct" ident)
// 変数宣言のことも考慮して別の関数に振り分けた
static Type *decl_basictype() {
  if (consume_keyword("struct"))
    return find_struct(get_ident());
  if (consume_keyword("int"))
    return new_type(TY_INT);
  if (consume_keyword("char"))
    return new_type(TY_CHAR);
  if (consume_keyword("long"))
    return new_type(TY_INT);
  if (!consume_keyword("void"))
    error_at(token->str, "型名が必要");
  return new_type(TY_VOID);
}

// equal_basictype = ("int" | "char" | "long" | "void" | "struct")
static long equal_basictype() {
  return equal_keyword("int") || equal_keyword("char") || equal_keyword("long") || equal_keyword("void") || equal_keyword("struct");
}

static Type *declarator(Type *ty);
Program *parse();
static Function *function_definition();
static Function *prototype_declaration();
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *logor();
static Node *logand();
static Node *equality();
static Node *relational();
static Node *new_add(Node *, Node *);
static Node *new_add_asgn(Node *, Node *);
static Node *new_sub_asgn(Node *, Node *);
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
// declarator_except_ident = "*" * ( "[" number "]" )?
static Type *declarator_except_ident(Type *ty) { // 識別子を覗いた宣言子
  while (consume("*"))
    ty = pointer_to(ty);

  Token *tok_lval = token;
  if (consume("(")) // 宣言子の後ろに括弧が来たら関数定義、宣言として読み込み失敗
    return NULL;
  if (consume("[")) {
    ty = array_of(ty, expect_number());
    expect("]");
  }

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

// declaration = decl_basictype (declarator ("=" (assign | ("{" assign ("," assign)* "}")))? ("," declarator ("=" (assign | ("{" assign ("," assign)* "}")))?)*)? ";"
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
      if (lvar->ty->kind == TY_ARRAY) { // 配列に対する初期化
        if (consume("{")) { // 初期化子リスト
          long param_cnt = 0;
          Node *lhs = new_node_lvar(lvar, ty->tok);

          while (!consume("}")) {
            Node *rhs = assign();
            decay_arr(rhs); // 右辺が配列ならポインタに降格
            if (param_cnt<lvar->ty->array_size) // 配列の要素数を超えた
              cur = cur->next = new_node(ND_ASSIGN, new_node(ND_DEREF, new_add(lhs, new_node_num(param_cnt)), NULL), rhs);
            ++param_cnt;
            consume(",");
          }
          while (param_cnt<lvar->ty->array_size) { // 残りの要素は0で初期化
            cur = cur->next = new_node(ND_ASSIGN, new_node(ND_DEREF, new_add(lhs, new_node_num(param_cnt)), NULL), new_node_num(0));
            ++param_cnt;
          }
        } else { // 文字列リテラル
          Token *tok = consume_str();
          if (tok) {
            long param_cnt = 0;
            Node *lhs = new_node_lvar(lvar, ty->tok);

            for(char *ptr = tok->str+1;param_cnt<tok->len-2 && param_cnt<lvar->ty->array_size;++ptr) {
              cur = cur->next = new_node(ND_ASSIGN, new_node(ND_DEREF, new_add(lhs, new_node_num(param_cnt)), NULL), new_node_num(*ptr));
              ++param_cnt;
            }
            while (param_cnt<lvar->ty->array_size) { // 残りの要素は0で初期化
              cur = cur->next = new_node(ND_ASSIGN, new_node(ND_DEREF, new_add(lhs, new_node_num(param_cnt)), NULL), new_node_num(0));
              ++param_cnt;
            }
          } else {
            error_at(token->str, "不明な初期化式");
          }
        }
      } else { // 配列以外
        Node *lhs = new_node_lvar(lvar, ty->tok);
        Node *rhs = assign();
        decay_arr(rhs); // 右辺が配列ならポインタに降格
        cur = cur->next = new_node(ND_ASSIGN, lhs, rhs);
      }
    }

    consume(",");
  }

  Node *node = new_node(ND_BLOCK, NULL, NULL);
  node->body = head.next;
  return node;
}
// gvar_declaration = decl_basictype (declarator ("=" (num | string_literal | ("{" num ("," num)* "}")))? ("," declarator ("=" (num | string_literal | ("{" num ("," num)* "}")))?)*)? ";"
static long gvar_declaration() {
  Type *basety = decl_basictype();

  while (!consume(";")) {
    Type *ty = declarator(basety);
    if (!ty) // 宣言として読み込み失敗
      return false;
    GVar *gvar = new_gvar(ty->name, ty);

    if (consume("=")) {
      if (gvar->ty->kind == TY_ARRAY) { // 配列に対する初期化
        if (consume("{")) { // 初期化子リスト
          Node head = {};
          Node *cur = &head;
          long param_cnt = 0;

          while (!consume("}")) {
            if (param_cnt<gvar->ty->array_size) // 配列の要素数を超えた
              cur = cur->next = new_node_num(expect_number());
            ++param_cnt;
            consume(",");
          }
          while (param_cnt<gvar->ty->array_size) { // 残りの要素は0で初期化
            cur = cur->next = new_node_num(0);
            ++param_cnt;
          }
          Node *node = new_node(ND_BLOCK, NULL, NULL);
          node->body = head.next;
          add_type(node);
          gvar->init = node;
        } else { // 文字列リテラル
          Token *tok = consume_str();
          if (tok) {
            Node head = {};
            Node *cur = &head;

            Str *string = new_str(tok);
            cur = cur->next = new_node_str(string, tok);
            long param_cnt = string->ty->array_size;

            while (param_cnt<gvar->ty->array_size) { // 残りの要素は0で初期化
              cur = cur->next = new_node_num(0);
              ++param_cnt;
            }
            Node *node = new_node(ND_BLOCK, NULL, NULL);
            node->body = head.next;
            add_type(node);
            gvar->init = node;
          } else {
            error_at(token->str, "不明な初期化式");
          }
        }
      } else { // 配列以外
        Token *tok = consume_str();
        if (tok) {
          Str *string = new_str(tok);
          Node *node  = new_node_str(string, tok);

          decay_arr(node); // 右辺が配列ならポインタに降格
          add_type(node);
          gvar->init = node;
        } else {
          gvar->init = new_node_num(expect_number());
        }
      }
    }

    consume(",");
  }

  return true;
}

// 構造体にオフセット割り当て
static void assign_struct_offsets(Struct *_struct) {
  long offset = 0;
  long struct_align = 0;

  for(MStruct *member = _struct->decl->member; member; member = member->next) {
    if (!member->ty->align)
      error_at(member->ty->tok->str, "alignment is not defigned");
    member->offset = offset = align_to(offset, member->ty->align);
    if (struct_align < member->ty->align)
      struct_align = member->ty->align;
    offset += member->ty->size;
  }

  offset = align_to(offset, struct_align);
  _struct->decl->align = struct_align;
  _struct->decl->size = offset;
}

// struct_declaration = "struct" ident "{" (decl_basictype declarator ";")* "}" ";"
static long struct_declaration() {
  if (!consume_keyword("struct")) {
    return false;
  }
  char *name = get_ident();
  Struct *_struct = new_struct(name);
  if(consume("{")) {
    MStruct head = {};
    MStruct *cur = &head;

    while (!consume("}")) {
      Type *basety = decl_basictype();
      Type *ty = declarator(basety);
      expect(";");
      cur = cur->next = new_mstruct(ty->name, ty);
    }
    _struct->decl->member = head.next;
    _struct->next = struct_defs;
    struct_defs = _struct;
    assign_struct_offsets(_struct);
    expect(";");

    return true;
  } else {
    if(!consume(";"))
      return false;
    _struct->next = struct_decls;
    struct_decls = _struct;
    return true;
  }
}

static void create_param_lvars(Type *param) {
  if (param) {
    create_param_lvars(param->next);
    new_lvar(param->name, param);
  }
}

// program = (function_definition | prototype_declaration | gvar_declaration | struct_declaration | struct_declaration)*
Program *parse() {
  Program *prog = calloc(1, sizeof(Program));
  Function func_head = {};
  Function *func_cur = &func_head;

  while (!at_eof()) {
    Token *origin = token; // 解析失敗時はトークンを元に戻す
    if (struct_declaration()) { // 構造体宣言
      continue;
    }
    token = origin;
    if (gvar_declaration()) { // グローバル変数宣言として読むことができるか
      continue;
    }
    token = origin;
    Function *func = function_definition();
    if (func) {
      func_cur = func_cur->next = func;
      functions = func_head.next;
      continue;
    }
    token = origin;
    func = prototype_declaration();
    func->next = prototypes;
    prototypes = func;
  }
    //error("正しくパースできませんでした。");

  prog->funcs = func_head.next;
  return prog;
}

// function_definition = decl_basictype function_declarator stmt
static Function *function_definition() {
  Type *ty = decl_basictype();
  ty = function_declarator(ty);

  locals = NULL; // NULLのときが終端とするため

  Function *fn = calloc(1, sizeof(Function));
  fn->name = ty->name;
  create_param_lvars(ty->params);
  fn->params = locals;

  if (!at_block())
    return NULL;
  fn->body = stmt();
  fn->locals = locals;
  fn->ret_ty = ty->return_ty;
  return fn;
}

// prototype_declaration = decl_basictype function_declarator ";"
static Function *prototype_declaration() {
  Type *ty = decl_basictype();
  ty = function_declarator(ty);

  locals = NULL; // NULLのときが終端とするため

  Function *fn = calloc(1, sizeof(Function));
  fn->name = ty->name;
  create_param_lvars(ty->params);
  fn->params = locals;

  expect(";");
  fn->locals = locals;
  fn->ret_ty = ty->return_ty;
  return fn;
}

// stmt = expr? ";"
//      | "{" (declaration | stmt)* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" (declaration | stmt) expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "return" expr ";"
static Node *stmt() {
  Node *node;

  if (consume("{")) {
    Node head;
    head.next = NULL;
    Node *cur = &head;
    while (!consume("}")) {
      if (equal_basictype())
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
    if (equal_basictype())
      node->init = declaration(); // 初期化式で変数定義
    else
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
    node = expr();
    add_type(node);
    decay_arr(node);
    node = new_node(ND_RETURN, node, NULL);
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

// assign = logor (("="|"+="|"-="|"*="|"/="|"%=") assign)?
static Node *assign() {
  Node *node = logor();
  if (consume("=")) {
    Node *rhs = assign();
    decay_arr(rhs); // 右辺が配列ならポインタに降格
    node = new_node(ND_ASSIGN, node, rhs);
  } else if (consume("+=")) {
    Node *rhs = assign();
    decay_arr(rhs);
    node = new_add_asgn(node, rhs);
  } else if (consume("-=")) {
    Node *rhs = assign();
    decay_arr(rhs);
    node = new_sub_asgn(node, rhs);
  } else if (consume("*=")) {
    Node *rhs = assign();
    decay_arr(rhs);
    node = new_node(ND_MULASGN, node, rhs);
  } else if (consume("/=")) {
    Node *rhs = assign();
    decay_arr(rhs);
    node = new_node(ND_DIVASGN, node, rhs);
  } else if (consume("%=")) {
    Node *rhs = assign();
    decay_arr(rhs);
    node = new_node(ND_REMASGN, node, rhs);
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
  if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
    // lhsの型がadd計算後の型になる
    // char + int を int + char にひっくり返す
    if (lhs->ty->kind == TY_CHAR && rhs->ty->kind == TY_INT) {
      Node *tmp = lhs;
      lhs = rhs;
      rhs = tmp;
    }
    return new_node(ND_ADD, lhs, rhs);
  }

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
  if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
    // char - int を -int + char にする.
    if (lhs->ty->kind == TY_CHAR && rhs->ty->kind == TY_INT)
      return new_add(new_node(ND_SUB, new_node_num(0), rhs), lhs);
    return new_node(ND_SUB, lhs, rhs);
  }

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

static Node *new_add_asgn(Node *lhs, Node *rhs) {
  // if (!is_value(lhs))
  //   error_at(token->str, "非左辺値への代入");
  add_type(lhs);
  add_type(rhs);

  // num += num
  if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
    // lhsの型がadd_assign計算後の型になる
    return new_node(ND_ADDASGN, lhs, rhs);
  }

  // array += ? は計算できない
  if (lhs->ty->kind == TY_ARRAY)
    error_at(token->str, "無効な演算(array+=num)");

  // 配列に対するaddは先頭要素のアドレスに降格
  decay_arr(rhs);

  // ptr += ptr は計算できない
  if (lhs->ty->base && rhs->ty->base)
    error_at(token->str, "無効な演算(pointer+pointer)");

  // num += ptr
  if (!lhs->ty->base && rhs->ty->base) {
    lhs = new_node(ND_MUL, lhs, new_node_num(rhs->ty->base->size));
    return new_node(ND_ADDASGN, lhs, rhs);
  }

  if (lhs->ty->kind != TY_PTR)
    error_at(token->str, "左辺値はポインタではありません");
  if (!is_integer(rhs->ty))
    error_at(token->str, "右辺値は整数型ではありません");

  // ptr += num
  rhs = new_node(ND_MUL, rhs, new_node_num(lhs->ty->base->size)); // シフト演算子を定義したら変更する
  return new_node(ND_ADDASGN, lhs, rhs);
}

static Node *new_sub_asgn(Node *lhs, Node *rhs) {
  add_type(lhs);
  add_type(rhs);

  // num -= num
  if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
    // lhsの型がsub_assign計算後の型になる
    return new_node(ND_SUBASGN, lhs, rhs);
  }

  // array -= ? は計算できない
  if (lhs->ty->kind == TY_ARRAY)
    error_at(token->str, "無効な演算(array-=num)");

  // 配列に対するsubは先頭要素のアドレスに降格
  decay_arr(rhs);

  // ptr -= num
  if (lhs->ty->base && is_integer(rhs->ty)) {
    rhs = new_node(ND_MUL, rhs, new_node_num(lhs->ty->base->size)); // シフト演算子を定義したら変更する
    add_type(rhs);
    Node *node = new_node(ND_SUBASGN, lhs, rhs);
    node->ty = lhs->ty;
    return node;
  }

  // ptr -= ptr
  // ptr - ptr は本来intのはずなので対応しない
  // if (lhs->ty->base && rhs->ty->base) {
  //   lhs = new_node(ND_DIVASGN, lhs, new_node_num(lhs->ty->base->size));
  //   rhs = new_node(ND_DIV, rhs, new_node_num(rhs->ty->base->size));
  //   return new_node(ND_SUBASGN, lhs, rhs); // lhsが2回評価される
  // }

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

// unary = ("++"|"--")? unary
//       | ("+" | "-")? postfix
//       | "*" unary
//       | "&" postfix
//       | "!" unary
static Node *unary() {
  if (consume("++")) // 前置インクリメント
    return new_add_asgn(unary(), new_node_num(1));
  if (consume("--")) // 前置デクリメント
    return new_sub_asgn(unary(), new_node_num(1));
  if (consume("+"))
    return new_node(ND_ADD, new_node_num(0), postfix());
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), postfix());
  if (consume("*")) {
    Node *node = unary();
    decay_arr(node); // 配列ならポインタに降格
    return new_node(ND_DEREF, node, NULL);
  }
  if (consume("&"))
    return new_node(ND_ADDR, postfix(), NULL);
  if (consume("!"))
    return new_node(ND_EQ, new_node_num(0), unary()); // 0 == E
  return postfix();
}

// postfix = primary ("[" expr "]" | "." ident | "->" ident | "++" | "--")*
static Node *postfix() {
  Node *node = primary();

  for (;;) {
    if (consume("[")) {
      node = new_node(ND_DEREF, new_add(node, expr()), NULL);
      expect("]");
    }
    else if (consume(".")) {
      add_type(node);
      MStruct *member = find_member(node->ty, get_ident());
      node = new_node(ND_DEREF, new_node(ND_ADD, new_node(ND_ADDR, node, NULL), new_node_num(member->offset)), NULL);
      node->ty = member->ty;
    }
    else if (consume("->")) {
      add_type(node);
      decay_arr(node);
      if (node->ty->kind != TY_PTR)
        error_at(token->str, "'->' 演算子は構造体のポインタ型のみに使用できます");
      MStruct *member = find_member(node->ty->base, get_ident());
      node = new_node(ND_DEREF, new_node(ND_ADD, node, new_node_num(member->offset)), NULL);
      node->ty = member->ty;
    }

    else if (consume("++")) // 後置インクリメント
      return new_sub(new_add_asgn(node, new_node_num(1)), new_node_num(1));
    else if (consume("--")) // 後置デクリメント
      return new_add(new_sub_asgn(node, new_node_num(1)), new_node_num(1));
    else
      return node;
  }
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
    Token *origin = token; // 解析失敗時はトークンを元に戻す
    if (consume("(") && equal_basictype()) {
      Type *basety = decl_basictype();
      Type *ty = declarator_except_ident(basety);
      expect(")");
      return new_node_num(ty->size);
    }

    token = origin;
    Node *node = unary();
    add_type(node);
    return new_node_num(node->ty->size);
  }

  if (consume_keyword("_Alignof")) {
    expect("(");
    Type *basety = decl_basictype();
    Type *ty = declarator_except_ident(basety);
    expect(")");
    return new_node_num(ty->align);
  }

  if (consume_keyword("offsetof") || consume_keyword("__builtin_offsetof")) {
    expect("(");
    Type *type = decl_basictype();
    expect(",");
    char *member = get_ident();
    expect(")");
    return new_node_num(find_member(find_struct(type->tag_name), member)->offset);
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
