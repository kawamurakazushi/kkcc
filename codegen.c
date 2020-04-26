#include "kkcc.h"

static void comment(char *c)
{
  printf("  # %s\n", c);
}

static void gen_lval(Node *node)
{
  if (node->kind != ND_LVAR)
  {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

static void gen(Node *node)
{
  switch (node->kind)
  {
  case ND_NUM:
    comment("ND_NUM");
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    comment("ND_LVAR");
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    comment("ND_ASSIGN");
    gen_lval(node->lhs); // スタックに変数のアドレスを格納する
    gen(node->rhs);      // stackに式の演算結果を保存する

    comment("Assigning");
    printf("  pop rdi\n");        // stackから式演算結果をrdiに格納
    printf("  pop rax\n");        // 変数のアドレスをraxに格納
    printf("  mov [rax], rdi\n"); // raxのアドレスに、rdiの値を格納
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind)
  {
  case ND_ADD:
    comment("ND_ADD");
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    comment("ND_SUB");
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    comment("ND_MUL");
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    comment("ND_DIV");
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    comment("ND_EQ");
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    comment("ND_NE");
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    comment("ND_LT");
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    comment("ND_LE");
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LVAR:
    break;
  case ND_ASSIGN:
    break;
  case ND_NUM:
    break;
  }

  printf("  push rax\n");
}

void codegen(Node *node)
{
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  comment("For variables a-z");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (Node *n = node; n; n = n->next)
  {
    gen(n);
    comment("Pop result to RAX");
    printf("  pop rax\n");
  }

  comment("Reset rsp");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");

  printf("  ret\n");
}
