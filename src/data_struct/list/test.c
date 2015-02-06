#include "list.c"

int main()
{
  t_list* nl = list_init(5);
  int a = 8;
  int b = 1;

  list_push(nl, a);
  list_push(nl, 2);
  list_push(nl, 4);
  list_push(nl, 5);
  list_push(nl, 9);
  list_push(nl, 5);

  list_pop(nl);

  list_add(nl, b);


  list_print(nl);
  list_free(nl);
  return 0;
}
