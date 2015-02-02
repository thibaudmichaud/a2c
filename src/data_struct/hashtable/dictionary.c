#include "dictionary.h"

void* dictionary_get_value(t_dictionary d, void *key)
{
  if (dictionary_find(d, key))
  {
    size_t h = d->hash_function(key, d->size);
    t_dico_data tmp = d->h_table[h];

    while(tmp->key != key)
      tmp = tmp->next;

    return tmp->value;
  }

}
