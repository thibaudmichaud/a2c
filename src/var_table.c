#include "var_table.h"
#include <err.h>

static size_t hash(struct var_sym *s)
{
  char *str = s->ident;
  size_t hash = 5381;
  char c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

void free_var(struct var_sym* sym)
{
  free(sym);
}

static int equal(struct var_sym *s1, struct var_sym *s2)
{
  return (strcmp(s1->ident, s2->ident) == 0);
}

var_table_t* empty_var_table(void)
{
  var_table_t* var_table = malloc(sizeof(var_table_t));
  ht_init(*var_table, 97, hash, equal, free_var);
  return var_table;
}

void add_var(var_table_t* var_table, struct var_sym *sym)
{
  ht_add(*var_table, sym);
}

void del_var(var_table_t* var_table, char *ident)
{
  struct var_sym sym;
  sym.ident = ident;
  ht_del(*var_table, &sym);
}

struct var_sym *find_var(var_table_t* var_table, char *ident)
{
  struct var_sym sym;
  sym.ident = ident;
  struct var_sym *res = NULL;
  ht_find(*var_table, &sym, &res);
  return res;
}

void free_var_table(var_table_t* var_table)
{
    ht_free(*var_table);
    free(var_table);
}
