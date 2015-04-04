#include "funtable.h"

static size_t hash(struct function* f)
{
    unsigned long hash = 5381;
    int c;
    char *str = f->ident;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int equal(struct function* f1, struct function *f2)
{
    return (strcmp(f1->ident,f2->ident) == 0);
}

fun_table_t empty_fun_table(void)
{
    fun_table_t table;
    ht_init(table, 97, hash, equal);
    return table;
}

void add_function(fun_table_t table, struct function* f)
{
    ht_add(table,f);
}

void delete_function(fun_table_t table, struct function* f)
{
    ht_del(table, f);
}

struct function* get_function(fun_table_t table, char* ident)
{
    struct function *f = malloc(sizeof(struct function));
    f->ident = ident;
    struct function *res = NULL;
    ht_find(table, f, &res);
    free(f);
    return res;
}

void free_fun_table(fun_table_t table)
{
    ht_free(table);
}
