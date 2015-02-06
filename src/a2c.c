#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include "parser.h"
#include "grammar.h"

// Global variable filled by yyparse().
extern struct expr *expression;
extern FILE *yyin;

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

void print_expression(struct expr *e)
{
  switch (e->exprtype)
  {
    case inttype:
      printf("%d", e->val.intval);
      break;
    case binopexprtype:
      printf("(");
      print_expression(e->val.binopexpr.e1);
      printf(" %s ", getopstr(e->val.binopexpr.op));
      print_expression(e->val.binopexpr.e2);
      printf(")");
      break;
    case unopexprtype:
      printf("(");
      printf("%s ", getopstr(e->val.unopexpr.op));
      print_expression(e->val.unopexpr.e);
      printf(")");
      break;
    default:
      printf("Not handled yet\n");
  }
}

void free_expression(struct expr *e)
{
  switch (e->exprtype)
  {
    case binopexprtype:
      free_expression(e->val.binopexpr.e1);
      free_expression(e->val.binopexpr.e2);
      break;
    case unopexprtype:
      free_expression(e->val.unopexpr.e);
      break;
    default:
      break;
  }
  free(e);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("usage: %s FILENAME", argv[0]);
    _exit(1);
  }

  yyin = fopen(argv[1], "r");
  if (yyin == NULL)
    err(1, "Couldn't open file %s", argv[1]);

  expression = NULL;
  yyparse();
  print_expression(expression);
  printf("\n");
  yylex_destroy();
  free_expression(expression);
  return 0;
}
