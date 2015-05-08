#ifndef A2C_H_
#define A2C_H_

#include <stdio.h>

struct prog *prog;
extern FILE *fin;
extern char *srcfilename;
extern int syntax_error;

// This is needed to free the global variables allocated by the lexer.
int yylex_destroy(void);

#endif
