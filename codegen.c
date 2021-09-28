#include "9cc.h"

static int count(void) {
  static int i = 1;
  return i++;
}

// 'n'を'align'の最も近い倍数に切り上げる。
// align_to(5, 8) == 8, align_to(11, 8) == 16.
static int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

static void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->lvar->offset);
  printf("  push rax\n");
}

static void gen(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_BLOCK:
    for (Node *n = node->body; n; n = n->next) {
      gen(n);
      printf("  pop rax\n");
    }
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  jmp .L.return\n");
    return;
  case ND_IF: {
    int c = count();
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .L.else.%d\n", c);
    gen(node->then);
    printf("  pop rax\n");
    printf("  jmp .L.end.%d\n", c);
    printf(".L.else.%d:\n", c);
    if (node->els){
      gen(node->els);
      printf("  pop rax\n");
    }
    printf(".L.end.%d:\n", c);
    return;
  }
  case ND_LOOP: {
    int c = count();
    if (node->init)
      gen(node->init);
    printf(".L.begin.%d:\n", c);
    if (node->cond) {
      gen(node->cond);
      printf("  cmp rax, 0\n");
      printf("  je  .L.end.%d\n", c);
    }
    gen(node->then);
    if (node->inc)
      gen(node->inc);
    printf("  jmp .L.begin.%d\n", c);
    printf(".L.end.%d:\n", c);
    return;
  }
  case ND_FUNCALL: 
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->funcname);
    printf("  push rax\n");
    return;
  default:
    break;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  default:
    break;
  }

  printf("  push rax\n");
}

// ローカル変数にオフセット割り当て
static void assign_lvar_offsets(Function *prog) {
  int offset = 0;
  for (LVar *var = prog->locals; var; var = var->next) {
    offset += 8;
    var->offset = offset;
  }
  prog->stack_size = align_to(offset, 16); // stack_sizeを16の倍数にする(RSPを16の倍数にしなければならないらしい)
}

void codegen(Function *prog) {
  assign_lvar_offsets(prog);

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  // 使用した変数分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", prog->stack_size);

  // 先頭の式から順にコード生成
  //for (int i = 0; code[i]; i++) {
    gen(prog->body);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    //printf("  pop rax\n");
  //}


  printf(".L.return:\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  
}
