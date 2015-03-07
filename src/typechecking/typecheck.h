#ifndef TYPECHECK_H_
#define TYPECHECK_H_

#include "grammar.h"

typedef enum{
  t_INT,
  t_CHAR,
  t_STR,
  t_REAL,
  t_BOOL,
  t_NUL,
  t_error
}types;

struct symbole {
  char* ident;
  types* type;
  struct expr* args;
};

struct symbole_table{
  struct symbole* symbole;
};

struct symbole* get_ident(char* ident);

bool check_algo(struct algo* al);
bool check_prog(struct prog* prog);
bool check_expr(struct expr *e);

types get_expr_type(struct expr *e);

#endif
