#include "kkcc.h"

static Node *stmt(Token **rest, Token *tok);
static Node *expr(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  return node;
}

static Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;

  return node;
}

static Node *new_node_var(char *str)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (str[0] - 'a' + 1) * 8;
  return node;
}

static Node *stmt(Token **rest, Token *token)
{
  Node *node = expr(&token, token);
  Token *t = skip(token, ";");
  *rest = t;

  return node;
}

static Node *expr(Token **rest, Token *token)
{
  return assign(rest, token);
}

// equality ("=" assign)?
static Node *assign(Token **rest, Token *token)
{
  Node *node = equality(&token, token);

  if (equal(token, "="))
  {
    node = new_node(ND_ASSIGN, node, assign(&token, token->next));
  }

  *rest = token;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *token)
{
  Node *node = relational(&token, token);

  for (;;)
  {
    if (equal(token, "=="))
    {
      node = new_node(ND_EQ, node, relational(&token, token->next));
      continue;
    }
    if (equal(token, "!="))
    {
      node = new_node(ND_NE, node, relational(&token, token->next));
      continue;
    }

    *rest = token;
    return node;
  }
}

static Node *relational(Token **rest, Token *token)
{
  Node *node = add(&token, token);

  for (;;)
  {
    if (equal(token, "<="))
    {
      node = new_node(ND_LE, node, add(&token, token->next));
      continue;
    }
    if (equal(token, "<"))
    {
      node = new_node(ND_LT, node, add(&token, token->next));
      continue;
    }
    if (equal(token, ">="))
    {
      node = new_node(ND_LE, add(&token, token->next), node);
      continue;
    }
    if (equal(token, ">"))
    {
      node = new_node(ND_LT, add(&token, token->next), node);
      continue;
    }

    *rest = token;
    return node;
  }
}

static Node *add(Token **rest, Token *token)
{
  Node *node = mul(&token, token);

  for (;;)
  {
    if (equal(token, "+"))
    {
      node = new_node(ND_ADD, node, mul(&token, token->next));
      continue;
    }
    if (equal(token, "-"))
    {
      node = new_node(ND_SUB, node, mul(&token, token->next));
      continue;
    }

    *rest = token;
    return node;
  }
}

static Node *mul(Token **rest, Token *token)
{
  Node *node = unary(&token, token);

  for (;;)
  {
    if (equal(token, "*"))
    {
      node = new_node(ND_MUL, node, unary(&token, token->next));
      continue;
    }
    if (equal(token, "/"))
    {
      node = new_node(ND_DIV, node, unary(&token, token->next));
      continue;
    }

    *rest = token;
    return node;
  }
}

static Node *unary(Token **rest, Token *token)
{
  if (equal(token, "+"))
    return unary(rest, token->next);

  if (equal(token, "-"))
    return new_node(ND_SUB, new_node_num(0), unary(rest, token->next));

  return primary(rest, token);
}

// primary = "(" expr ")" | ident | num
static Node *primary(Token **rest, Token *token)
{
  for (;;)
  {
    if (equal(token, "("))
    {
      Node *node = expr(&token, token->next);
      Token *t = skip(token, ")");

      *rest = t;
      return node;
    }

    Node *node;

    if (token->kind == TK_IDENT)
      node = new_node_var(token->str);
    else
      node = new_node_num(get_number(token));
    *rest = token->next;
    return node;
  }
}

Node *parse(Token *token)
{
  Node *node = stmt(&token, token);
  return node;
}

// test
static void check(Node *node)
{
  if (node->kind == ND_NUM)
  {
    printf("NUM: %d\n",node->val);
    return;
  }

  if (node->kind == ND_LVAR)
  {
    printf("VAR: offset %d\n", node->offset);
    return;
  }

  check(node->lhs);
  check(node->rhs);

  switch (node->kind)
  {
  case ND_ADD:
    printf("ND_ADD\n");
    break;
  case ND_SUB:
    printf("ND_SUB\n");
    break;
  case ND_MUL:
    printf("ND_MUL\n");
    break;
  case ND_DIV:
    printf("ND_DIV\n");
    break;
  case ND_EQ:
    printf("ND_EQ\n");
    break;
  case ND_NE:
    printf("ND_NE\n");
    break;
  case ND_LT:
    printf("ND_LT\n");
    break;
  case ND_LE:
    printf("ND_LE\n");
    break;
  case ND_LVAR:
    printf("ND_LVAR\n");
    break;
  case ND_ASSIGN:
    printf("ND_ASSIGN\n");
    break;
  case ND_NUM:
    printf("ND_NUM\n");
    break;
  }

  printf("-- Done\n");
}


void test_parse()
{
  printf("\n-- Testing parse\n");
  Node *node = parse(tokenize("c=8;"));
  check(node);
}
