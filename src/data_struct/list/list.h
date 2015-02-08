#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>

typedef struct s_list t_list;

struct s_list
{
  size_t size_max;
  size_t size;
  void ** data;
};

t_list* list_init(size_t size);

void list_free(t_list* l);

void list_push_back(t_list* l, void *data);

void *list_pop_back(t_list* l);

void list_realloc(t_list *l);

void list_insert(t_list* l, size_t i, void *elt);

void list_del(t_list *l, size_t i);

void list_push_front(t_list *l, void *data);

void list_pop_front(t_list *l);

void *list_nth(t_list *l, size_t i);

#endif /* LIST_H_ */
