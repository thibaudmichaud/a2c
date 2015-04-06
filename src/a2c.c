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

  yyparse();
  
  if (!check_prog(prog))
    print_prog(prog);
  fclose(yyin);
  yylex_destroy();
  free_prog(prog);
  return 0;
}
