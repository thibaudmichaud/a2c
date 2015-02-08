#include "data_struct/list/list.h"
#include <assert.h>
#include <stdlib.h>

int main()
{
  t_list* nl = list_init(5);
  int a = 8;
  int b = 1;

  list_push(nl, &a);
  list_push(nl, &a);
  list_push(nl, &a);
  assert(nl->size == 3);
  list_push(nl, &a);
  list_push(nl, &a);
  list_push(nl, &a);
  assert(nl->size == 6);

  list_pop(nl);
  assert(nl->size == 5);

  list_pop(nl);
  assert(nl->size == 4);

  list_add(nl, &b);


  list_print(nl);
  list_free(nl);
  return 0;
}
