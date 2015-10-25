#ifndef A2C_H_
#define A2C_H_

#include <stdio.h>

struct prog *prog;
extern FILE *fin;
extern char *srcfilename;

enum e_lang
{
  LANG_FR   =   0,
  LANG_EN   =   1
};

# define CURRENT_LANG       LANG_EN

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

#endif
