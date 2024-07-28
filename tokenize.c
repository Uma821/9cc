#include "9cc.h"

// 入力プログラム
char *user_input;
// 入力ファイル名
char *filename;

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラーの起きた場所を報告するための関数
// 下のようなフォーマットでエラーメッセージを表示する
//
// foo.c:10: x = y + + 5;
//                   ^ 式ではありません
void error_at(char *loc, char *msg, ...) {
  va_list ap;
  va_start(ap, msg);

  // locが含まれている行の開始地点と終了地点を取得
  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // 見つかった行が全体の何行目なのかを調べる
  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  // 見つかった行を、ファイル名と行番号と一緒に表示
  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // エラー箇所を"^"で指し示して、エラーメッセージを表示
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, msg, ap);
  fprintf(stderr, "\n");
  exit(1); 
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_keyword(char *op) {
  if (token->kind != TK_KEYWORD ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool equal_keyword(char *op) {
  if (token->kind != TK_KEYWORD ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "\"%s\"ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_block() {
  if (token->kind != TK_RESERVED ||
      token->len != 1 ||
      token->str[0] != '{')
    return false;
  return true;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}

Token *consume_str() {
  if (token->kind != TK_STR)
    return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

static bool isdoublequote(char c) {
  return c == '"';
}

// cが識別子の最初の文字として有効な場合、trueを返す
static bool is_ident1(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// cが識別子の2番目以降の文字として有効な場合、trueを返す
static bool is_ident2(char c) {
  return is_ident1(c) || ('0' <= c && c <= '9');
}

// pから区切子を読み取り、その長さを返す
static int read_punct(char *p) {
  if (startswith(p, "==") || startswith(p, "!=") ||
      startswith(p, "<=") || startswith(p, ">="))
    return 2;
  return ispunct(*p) ? 1 : 0;
}

static int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

// キーワードかどうかを調べる。
static int is_keyword(const char * const p) {
  static char const * const kws[] = {
    "if", "else", "for", "while", "return", "int", "char", "sizeof"};
  for (int i = 0; i < sizeof(kws) / sizeof(*kws); i++)
    if (strncmp(p, kws[i], strlen(kws[i])) == 0 && !is_alnum(p[strlen(kws[i])]))
      return strlen(kws[i]);
  return 0;
}

// 入力文字列user_inputをトークナイズしてそれを返す
void tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 行コメントをスキップ
    if(strncmp(p, "//", 2) == 0) {
      p += 2;
      while(*p != '\n') {
        p++;
      }
      continue;
    }

    // ブロックコメントをスキップ
    if(strncmp(p, "/*", 2) == 0) {
      char *q = strstr(p + 2, "*/");
      if (!q)
        error_at(p, "コメントが閉じられていません");
      p = q + 2;
      continue;
    }

    // 整数リテラル
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    // 文字列リテラル
    if (isdoublequote(*p)) {
      char *start = p;
      do {
        p++;
      } while (!isdoublequote(*p));
      ++p;
      cur = new_token(TK_STR, cur, start, p-start);
      continue;
    }

    // キーワード
    int key_len;
    if ((key_len = is_keyword(p))) {
      cur = new_token(TK_KEYWORD, cur, p, key_len);
      p += key_len;
      continue;
    }

    // 識別子
    if (is_ident1(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident2(*p));
      cur = new_token(TK_IDENT, cur, start, p-start);
      continue;
    }

    // 区切子
    int punct_len = read_punct(p);
    if (punct_len) {
      cur = new_token(TK_RESERVED, cur, p, punct_len);
      p += cur->len;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  token = head.next;
  return;
}
