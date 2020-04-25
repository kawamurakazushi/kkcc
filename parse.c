#include "kkcc.h"

static Node *expr(Token **rest, Token *tok);
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

static Node *expr(Token **rest, Token *token)
{
  return equality(rest, token);
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

    Node *node = new_node_num(get_number(token));
    *rest = token->next;
    return node;
  }
}

Node *parse(Token *token)
{
  Node *node = expr(&token, token);
  return node;
}
