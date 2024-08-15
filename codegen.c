#include "9cc.h"

static char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
static struct Function *current_fn;

static void gen();

static long count(void) {
  static long i = 1;
  return i++;
}

static void gen_variable(struct Node *node) {
  if (node->kind == ND_LVAR) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %ld\n", node->lvar->offset);
    printf("  push rax\n");
    return;
  } else if (node->kind == ND_GVAR) {
    printf("  push offset %s\n", node->gvar->name);
    return;
  } else if (node->kind == ND_DEREF) {
    gen(node->lhs);
    return;
  } else {
    error("代入の左辺値が変数ではありません");
  }
  
}

static void gen(struct Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %ld\n", node->val);
    return;
  } else if (node->kind == ND_STR) {
    printf("  push offset .LC%ld\n", node->string->num);
    return;
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
    return;
  } else if (node->kind == ND_ADDR) {
    gen_variable(node->lhs);
    return;
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
    return;
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
    return;
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
    return;
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
    return;
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
    return;
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
    return;
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
    return;
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
    return;
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
    return;
  } else if (node->kind == ND_BLOCK) {
    for (struct Node *n = node->body; n; n = n->next) {
      gen(n);
      printf("  pop rax\n");
    }
    printf("  push rax\n");
    return;
  } else if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  jmp .L.return.%s\n", current_fn->name);
    return;
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
    return;
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
    return;
  } else if (node->kind == ND_FUNCALL) {
    long nargs = 0;
    for (struct Node *arg = node->args; arg; arg = arg->next) {
      gen(arg);
      nargs++;
    }

    for (long i = nargs - 1; i >= 0; i--)
      printf("  pop %s\n", argreg[i]);
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->funcname);
    printf("  push rax\n");
    return;
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
  for (struct GVar *gvar = globals; gvar; gvar = gvar->next) {
    // グローバル変数を定義
    printf("%s:\n", gvar->name);
    struct Node *gvar_init = gvar->init;
    if (gvar_init) {
      if (gvar_init->kind == ND_BLOCK) { // 配列
        for (struct Node *n = gvar_init->body; n; n = n->next) {
          if (n->kind == ND_STR && n->ty->kind == TY_ARRAY) { // 文字列リテラル
            printf("  .string %s\n", n->string->literal);
            continue;
          }

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
  for (struct Str *str = strings; str; str = str->next) {
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
      printf("  mov [rax], %s\n", argreg[i++]);
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
