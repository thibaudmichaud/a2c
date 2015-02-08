#include "data_struct/list/list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void list_check(t_list* l, size_t n, int i)
{
  assert(*(int *)l->data[n] == i);
}

int main()
{
  int tab[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  int a;
  t_list* l = list_init(5);

  for (size_t n = 0; n < 10; ++n)
    list_push_back(l, &tab[n]);

  assert(l->size == 10);

  for (size_t n = 0; n < 10; ++n)
    assert(*(int *)list_nth(l, n) == tab[n]);

  a = *(int *)list_pop_back(l);
  assert(a == 9);
  assert(l->size == 9);

  a = *(int *)list_pop_back(l);
  assert(a == 8);
  assert(l->size == 8);

  a = *(int *)list_pop_back(l);
  assert(a == 7);
  assert(l->size == 7);

  list_push_front(l, &tab[9]);
  assert(*(int *)list_nth(l, 0) == 9);
  assert(l->size == 8);

  list_insert(l, 3, &tab[4]);
  assert(*(int *)list_nth(l, 3) == tab[4]);

  list_free(l);
  return 0;
}
