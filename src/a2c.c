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
instructionlist_t block;

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

  block = parse();
  print_instructions(block, 0);
  free_instructions(block);

  fclose(fin);
  return 0;
}
