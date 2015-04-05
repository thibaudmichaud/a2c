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
bool check_algo(struct algo* al, fun_table_t functions);
bool check_prog(struct prog* prog);
bool check_inst(struct instruction *e, struct function* f,  fun_table_t functions, var_table_t* variables, type_table_t types);
bool check_expr(struct expr *e, fun_table_t functions, var_table_t* variables, type_table_t types);
char* expr_type(struct expr *e);
struct type* get_expr_type(struct expr *e, fun_table_t functions, var_table_t* variables, type_table_t types);

#endif
