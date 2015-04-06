#ifndef A2C_H_
#define A2C_H_

#include <stdio.h>

struct prog *prog;
extern FILE *yyin;
extern char *srcfilename;

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

#endif
