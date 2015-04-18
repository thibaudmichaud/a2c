#include <stdlib.h>
#include <stdio.h>
#include "get_line.h"

char* get_line(FILE *f, int line)
{
  char *l = NULL;
  long pos = ftell(f);
  size_t len = 0;

  if (f)
  {
    rewind(f);
    for (int i = 0; i < line + 1 && (getline(&l, &len, f) != -1); i++)
      ;
  }

  fseek(f, pos, SEEK_SET);
  return l;
}

