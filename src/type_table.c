#include "type_table.h"

static size_t hash(struct type_sym *s)
{
  char *str = s->ident;
  size_t hash = 5381;
  char c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

static int equal(struct type_sym *s1, struct type_sym *s2)
{
  return (strcmp(s1->ident, s2->ident) == 0);
}

type_table_t empty_type_table(void)
{
  type_table_t type_table;
  ht_init(type_table, 97, hash, equal);
  return type_table;
}

void add_type(type_table_t type_table, struct type_sym *sym)
{
  ht_add(type_table, sym);
}

void del_type(type_table_t type_table, char *ident)
{
  struct type_sym sym;
  sym.ident = ident;
  ht_del(type_table, &sym);
}

struct type_sym *find_type(type_table_t type_table, char *ident)
{
  struct type_sym sym;
  sym.ident = ident;
  struct type_sym *res = NULL;
  ht_find(type_table, &sym, &res);
  return res;
}

