#ifndef _TYPECHECK_H
#define _TYPECHECK_H

#include <stdio.h>
#include <string.h>
#include "codegen.h"
#include "funtable.h"
#include "var_table.h"
#include "type_table.h"
#include "grammar.h"

char *algo_to_c_type(char *ident);
bool check_algo(struct algo* al);
bool check_prog(struct prog* prog);
bool check_expr(struct expr *e);
char* expr_type(struct expr *e);
type get_expr_type(struct expr *e);

#endif
