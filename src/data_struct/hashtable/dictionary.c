#include "dictionary.h"

void* dictionary_get_value(t_dictionary d, void *key)
{
  if dictionary_find(d)
  {
    h = hash_function(key);
    t_dictionary tmp = d->h_table[h];

    while(tmp->key != key)
      tmp = tmp->next;

    return tmp->value;
  }

}
