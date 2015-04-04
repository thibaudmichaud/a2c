#ifndef _TYPE_TABLE_H
#define _TYPE_TABLE_H

#include "data_struct/hashtable/hashtable.h"
#include "type.h"
#include <string.h>

struct type_sym
{
  char *ident;
  struct type type;
};

typedef ht_tpl(struct type_sym *) type_table_t;
type_table_t empty_type_table(void);
void add_type(type_table_t type_table, struct type_sym *sym);
void del_type(type_table_t type_table, char *ident);
struct type_sym *find_type(type_table_t type_table, char *ident);
void free_type_table(type_table_t type_table);
#endif
