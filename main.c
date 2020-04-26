#include "kkcc.h"

// debug

void comment_token(Token *token)
{
  Token *t = token;
  while (t->next != NULL)
  {
    printf("# %s, %d\n", t->str, t->len);
    t = t->next;
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Running Test.....");
    test_tokenize();
    test_parse();
    return 0;
  }

  Token *token = tokenize(argv[1]);
  comment_token(token);
  Node *node = parse(token);
  codegen(node);

  return 0;
}
