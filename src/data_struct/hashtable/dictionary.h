#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "hashtable.c"

typedef t_hashtable t_dictionary;

t_dictionary dictionary_init(size_t size, size_t (*h_function)(void*, size_t))
{
  return hash_table_init(size, h_function);
}

void dictionary_free(t_dictionary t)
{
  hash_table_free(t);
}

int dictionary_find(t_dictionary t, void * key)
{
  return hash_table_find(t, key);
}

void dictionary_add(t_dictionary t, void *key, void *value)
{
  hash_table_dico_add(t, key, value);
}

void dictionary_remove(t_dictionary t, void *key)
{
  hash_table_remove(t, key);
}

void* dictionary_get_value(t_dictionary t, void *key);
#endif /* DICTIONARY_H_ */
