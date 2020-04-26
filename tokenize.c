#include "kkcc.h"

static char *user_input;

void error_at(char *loc, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool equal(Token *token, char *op)
{
  return strlen(op) == token->len && !strncmp(token->str, op, token->len);
}

Token *skip(Token *token, char *op)
{
  if (!equal(token, op))
    error_at(token->str, "'%s'ではありません", op);

  return token->next;
}

long get_number(Token *token)
{
  if (token->kind != TK_NUM)
    error_at(token->str, "Not a number");

  return token->val;
}

bool at_eof(Token *token)
{
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, int len, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;

  return tok;
}

bool startswith(char *p, char *q)
{
  return memcmp(p, q, strlen(q)) == 0;
}

// Tokenize `user_input` and returns new tokens.
Token *tokenize(char *p)
{
  user_input = p;

  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (startswith(p, ">=") || startswith(p, "<=") ||
        startswith(p, "==") || startswith(p, "!="))
    {
      cur = new_token(TK_RESERVED, 2, cur, p);
      p += 2;
      continue;
    }

    if (strchr("=+-*/()><;", *p))
    {
      cur = new_token(TK_RESERVED, 1, cur, p++);
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, 1, cur, p);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    if ('a' <= *p && *p <= 'z')
    {
      cur = new_token(TK_IDENT, 1, cur, p++);
      continue;
    }

    error_at(p, "Invalid token.");
  }

  new_token(TK_EOF, 1, cur, p);

  return head.next;
}

// token

char *token_to_string(TokenKind kind)
{
  switch (kind)
  {
  case TK_RESERVED:
    return "Reserved";
  case TK_IDENT:
    return "Indentifier";
  case TK_NUM:
    return "Number";
  case TK_EOF:
    return "End of File";
  default:
    return "none";
  }
}

void test_tokenize()
{
  Token *token = tokenize("1+1;a=2");
  TokenKind expected[8] = {
      TK_NUM,
      TK_RESERVED,
      TK_NUM,
      TK_RESERVED,
      TK_IDENT,
      TK_RESERVED,
      TK_NUM,
      TK_EOF,
  };

  for (int i = 0; i < 8; i++)
  {
    if (expected[i] == token->kind)
    {
      printf("pass %s\n", token_to_string(expected[i]));
      token = token->next;
      continue;
    }

    printf("expected %s, got %s\n", token_to_string(expected[i]), token_to_string(token->kind));

    exit(1);
  }

  printf("Tokenize test complete\n");
}
