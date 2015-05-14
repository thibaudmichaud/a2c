#ifndef _FUNTABLE_H
#define _FUNTABLE_H

#include "data_struct/hashtable/hashtable.h"
#include "data_struct/list/list.h"
#include "type.h"
#include <string.h>

struct argument
{
    struct type* type;
    int global;
};

typedef list_tpl(struct argument *) args_t;

struct function
{
    char* ident;
    struct type* ret;
    args_t arg;
};

typedef ht_tpl(struct function*) fun_table_t;
fun_table_t* empty_fun_table(void);
void add_function(fun_table_t* fun, struct function* f);
void delete_function(fun_table_t* table, struct function* f);
struct function* get_function(fun_table_t* table, char *ident);
void free_fun_table(fun_table_t* table);
#endif
