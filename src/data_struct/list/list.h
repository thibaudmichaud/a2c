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

void list_realloc(t_list* l);

void array_move(t_list *l, size_t i);

t_list* list_init(size_t size);

void list_free(t_list* l);

void list_push(t_list* l, void * data);

void *list_pop(t_list* l);

void list_add(t_list* l, void *data);

void list_print(t_list* l);


#endif /* LIST_H_ */
