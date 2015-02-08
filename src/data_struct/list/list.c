#include <stdlib.h>
#include <assert.h>

#include "list.h"

t_list* list_init(size_t size)
{
  t_list* new_list = malloc(sizeof (t_list));
  new_list->size = 0;
  new_list->size_max = size;
  new_list->data = malloc(sizeof(void*) * size);
  return new_list;
}

void list_free(t_list* l)
{
  free(l->data);
  free(l);
}

void list_push_back(t_list* l, void *data)
{
  list_realloc(l);
  l->data[l->size] = data;
  l->size++;
}

void *list_pop_back(t_list* l)
{
  l->size--;
  return l->data[l->size];
}

void list_realloc(t_list *l)
{
  if (l->size + 1 > l->size_max)
  {
    l->data = realloc(l->data, l->size_max * (sizeof(void*))* 2);
    l->size_max *= 2;
  }
}

void list_insert(t_list* l, size_t i, void *elt)
{
  list_realloc(l);

  for (size_t tmp = l->size; tmp > i; --tmp)
    l->data[tmp] = l->data[tmp - 1];

  l->data[i] = elt;
  ++l->size;
}

void list_del(t_list *l, size_t i)
{
  assert(l->size > 0);

  for (; i < l->size; ++i)
    l->data[i] = l->data[i + 1];

  --l->size;
}

void list_push_front(t_list *l, void *data)
{
  list_insert(l, 0, data);
}

void list_pop_front(t_list *l)
{
  list_del(l, 0);
}

void *list_nth(t_list *l, size_t n)
{
  return l->data[n];
}
