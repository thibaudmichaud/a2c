#include "type.h"
#include "type_table.h"

static size_t hash(struct type *s)
{
  char *str = s->name;
  size_t hash = 5381;
  char c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

static int equal(struct type *s1, struct type *s2)
{
  return (strcmp(s1->name, s2->name) == 0);
}

type_table_t* empty_type_table(void)
{
  type_table_t* type_table= malloc(sizeof(type_table_t));
  ht_init(*type_table, 97, hash, equal, free_type);
  return type_table;
}

void add_type(type_table_t* type_table, struct type *sym)
{
  ht_add(*type_table, sym);
}

void del_type(type_table_t* type_table, char *name)
{
  struct type sym;
  sym.name = name;
  ht_del(*type_table, &sym);
}

struct type *find_type(type_table_t* type_table, char *name)
{
  if (!name)
    return NULL;
  struct type sym;
  sym.name = name;
  struct type *res = NULL;
  ht_find(*type_table, &sym, &res);
  return res;
}

void free_type_table(type_table_t* type_table)
{
    ht_free(*type_table);
    free(type_table);
}
