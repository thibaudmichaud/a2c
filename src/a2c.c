#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include "parser.h"
#include "grammar.h"
#include "codegen.h"
#include "data_struct/list/list.h"

// Global variable filled by yyparse().
struct prog *prog;
extern FILE *yyin;

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

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

  yyparse();

  print_prog(prog);

  fclose(yyin);
  yylex_destroy();
  free_prog(prog);
  return 0;
}
