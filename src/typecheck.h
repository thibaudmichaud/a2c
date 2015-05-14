#ifndef _TYPECHECK_H
#define _TYPECHECK_H

#include <stdio.h>
#include <string.h>
#include <err.h>
#include "parser.h"
#include "codegen.h"
#include "data_struct/list/list.h"
#include "codegen.h"
#include "funtable.h"
#include "var_table.h"
#include "type_table.h"
#include "grammar.h"

struct symtable
{
  type_table_t *types;
  fun_table_t *functions;
  var_table_t *variables;
};

char *algo_to_c_type(char *ident);
bool check_algo(struct algo* al, struct symtable *syms);
bool check_prog(struct prog* prog);
bool check_inst(struct instruction *e, struct type *ret,  struct symtable *syms);
char *check_expr(struct expr *e, struct symtable *syms);
char* expr_type(struct expr *e);

#endif
