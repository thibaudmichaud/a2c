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
#include "lexer.h"

FILE *fin = NULL;
char *srcfilename = NULL;
struct prog *prog = NULL;

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

  bool error = false;
  prog = parse();
  if (prog)
  {
    struct symtable *syms = empty_symtable();
    if (check_prog(prog, syms))
      print_prog(prog);
    else
      error = true;
    free_symtable(syms);
    free_prog(prog);
  }
  else
    error = true;

  fclose(fin);
  return error;
}
