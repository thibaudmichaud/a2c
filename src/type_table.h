#ifndef _TYPE_TABLE_H
#define _TYPE_TABLE_H

#include "data_struct/hashtable/hashtable.h"
#include "typechecking/typecheck.h"
#include <string.h>

struct type_sym
{
  char *ident;
  type type;
};

typedef ht_tpl(struct type_sym *) type_table_t;
int equal(struct type_sym *s1, struct type_sym *s2);
type_table_t empty_type_table(void);
void add_var(type_table_t type_table, struct type_sym *sym);
void del_var(type_table_t type_table, char *ident);
struct type_sym *find_var(type_table_t type_table, char *ident);

#endif
