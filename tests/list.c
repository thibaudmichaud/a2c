#include "data_struct/list/list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void print_list(t_list* l)
{
  for (size_t n = 0; n < l->size; ++n)
  {
    printf("%d -> ", *(int *)(l->data[n]));
  }
  printf("\n");
}

int main()
{
  int tab[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  int a;
  t_list* l = list_init(5);

  for (size_t n = 0; n < 10; ++n)
    list_push_back(l, &tab[n]);

  // l == { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  assert(l->size == 10);

  for (size_t n = 0; n < 10; ++n)
    assert(*(int *)list_nth(l, n) == tab[n]);

  a = *(int *)list_pop_back(l);
  // l == { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
  assert(a == 9);
  assert(l->size == 9);

  a = *(int *)list_pop_back(l);
  // l == { 0, 1, 2, 3, 4, 5, 6, 7 };
  assert(a == 8);
  assert(l->size == 8);

  a = *(int *)list_pop_back(l);
  // l == { 0, 1, 2, 3, 4, 5, 6 };
  assert(a == 7);
  assert(l->size == 7);

  list_push_front(l, &tab[9]);
  // l == { 9, 0, 1, 2, 3, 4, 5, 6 };
  assert(*(int *)list_nth(l, 0) == 9);
  assert(l->size == 8);

  list_insert(l, 3, &tab[4]);
  // l == { 9, 0, 1, 4, 2, 3, 4, 5, 6 };
  assert(*(int *)list_nth(l, 3) == tab[4]);

  list_free(l);
  return 0;
}
