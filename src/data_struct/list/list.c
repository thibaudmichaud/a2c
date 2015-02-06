#include <stdio.h>
#include <stdlib.h>

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

void list_push(t_list* l, void *data)
{
  list_realloc(l);
  l->data[l->size] = data;
  l->size++;
}

void *list_pop(t_list* l)
{
  l->size--;
  return l->data[l->size];
}

void list_print(t_list* l)
{
  int i;
  for (i = 0; i < l->size; i++)
    printf("[%p] ->", l->data[i]);
}

void list_realloc(t_list *l)
{
  if (l->size + 1 > l->size_max)
  {
    l->data = realloc(l->data, l->size_max * (sizeof(void*))* 2);
    l->size_max *= 2;
  }
}

void array_move(t_list* l, size_t i)
{
  void* tmp1 = l->data[i];
  for (int j = i; j < l->size; j++)
  {
    void* tmp2 = l->data[j+1];
    l->data[j+1] = tmp1;
    tmp1 = tmp2;
  }
}

void *list_add(t_list *l, void *data)
{
  list_realloc(l);
  l->size++;
  array_move(l, 0);
  l->data[0] = data;
}
