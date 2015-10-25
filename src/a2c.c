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

void usage(char **argv)
{
  printf("usage: %s FILENAME\n", argv[0]);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    usage(argv);
    return 1;
  }
  if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    usage(argv);
    return 1;
  }

  current_lang = LANG_FR;

  if (strcmp(argv[1], "--en") == 0)
  {
    if (argc < 3)
    {
      usage(argv);
      return 1;
    }
    current_lang = LANG_EN;
    srcfilename = argv[2];
  }
  else
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
