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

FILE *fin = NULL;
char *srcfilename = NULL;
struct expr *expr = NULL;

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("usage: %s FILENAME", argv[0]);
    _exit(1);
  }

  srcfilename = argv[1];
  fin = fopen(srcfilename, "r");
  if (fin == NULL)
    err(1, "Couldn't open file %s", argv[1]);

  syntax_error = 0;
  expr = parse();
  if (expr)
    print_expression(expr);

  if (expr)
    free_expression(expr);

  fclose(fin);
  return 0;
}
