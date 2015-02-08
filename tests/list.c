#include "data_struct/list/list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  int tab[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  int a;
  list_tpl(int) l;
  list_init(l);

  for (size_t n = 0; n < 10; ++n)
    list_push_back(l, tab[n]);

  // l == { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  assert(l.size == 10);

  for (size_t n = 0; n < 10; ++n)
    assert(list_nth(l, n) == tab[n]);

  // l == { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
  a = list_pop_back(l);
  assert(a == 9);
  assert(l.size == 9);

  // l == { 0, 1, 2, 3, 4, 5, 6, 7 };
  a = list_pop_back(l);
  assert(a == 8);
  assert(l.size == 8);

  // l == { 0, 1, 2, 3, 4, 5, 6 };
  a = list_pop_back(l);
  assert(a == 7);
  assert(l.size == 7);

  list_push_front(l, tab[9]);
  // l == { 9, 0, 1, 2, 3, 4, 5, 6 };
  assert(list_nth(l, 0) == 9);
  assert(l.size == 8);

  list_insert(l, 3, tab[4]);
  // l == { 9, 0, 1, 4, 2, 3, 4, 5, 6 };
  assert(list_nth(l, 3) == tab[4]);

  list_free(l);
  return 0;
}
