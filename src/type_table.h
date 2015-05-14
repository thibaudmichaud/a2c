#ifndef _TYPE_TABLE_H
#define _TYPE_TABLE_H

#include "data_struct/hashtable/hashtable.h"
#include "type.h"
#include <string.h>

typedef ht_tpl(struct type *) type_table_t;
type_table_t* empty_type_table(void);
void add_type(type_table_t* type_table, struct type *sym);
void del_type(type_table_t* type_table, char *ident);
struct type *find_type(type_table_t* type_table, char *name);
void free_type_table(type_table_t* type_table);
#endif
