#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include "parser.h"
#include "grammar.h"
#include "codegen.h"
#include "typecheck.h"
#include "data_struct/list/list.h"
#include "a2c.h"

char *srcfilename = NULL;

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("usage: %s FILENAME", argv[0]);
    _exit(1);
  }

  srcfilename = argv[1];
  yyin = fopen(srcfilename, "r");
  if (yyin == NULL)
    err(1, "Couldn't open file %s", argv[1]);

  syntax_error = 0;
  yyparse();
  if (!syntax_error && check_prog(prog))
    print_prog(prog);

  if (prog)
    free_prog(prog);

  fclose(yyin);
  yylex_destroy();
  return 0;
}
