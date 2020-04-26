#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize.c

typedef enum
{
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token
{
  TokenKind kind;
  Token *next;
  int val; // if kind is `TK_NUM`, save the value here
  char *str;
  int len;
};

bool equal(Token *token, char *op);
Token *skip(Token *token, char *op);
long get_number(Token *token);
Token *tokenize(char *p);
void test_tokenize();

// parse.c

typedef enum
{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_LT,
  ND_LE,
  ND_EQ,
  ND_NE,
  ND_LVAR,
  ND_ASSIGN,
} NodeKind;

typedef struct Node Node;
struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // on ND_NUM
  int offset; // on ND_LVAR
};

Node *parse(Token *tok);

// codegen.c

void codegen(Node *node);
