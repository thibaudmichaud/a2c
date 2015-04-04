#ifndef _VAR_TABLE_H
#define _VAR_TABLE_H

#include "data_struct/hashtable/hashtable.h"
#include "type.h"
#include <string.h>

struct var_sym
{
  char *ident;
  type type;
};

typedef ht_tpl(struct var_sym *) var_table_t;
var_table_t empty_var_table(void);
void add_var(var_table_t var_table, struct var_sym *sym);
void del_var(var_table_t var_table, char *ident);
struct var_sym *find_var(var_table_t var_table, char *ident);
void free_var_table(var_table_t var_table);
#endif
