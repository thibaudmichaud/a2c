#include "lexer.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "get_line.h"
#include "a2c.h"

void error(struct pos pos, char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "%s:%d:%d: error: ",
      srcfilename, pos.line, pos.charstart);
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
  char *linestr = get_line(fin, pos.line);
  fprintf(stderr, "%s", linestr);
  free(linestr);
  for (unsigned i = 0; i < pos.charstart; ++i)
    fprintf(stderr, " ");
  for (unsigned i = 0; i < pos.len; ++i)
    fprintf(stderr, "^");
  fprintf(stderr, "\n");
}
