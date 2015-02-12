#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdio.h>
#include <stdlib.h>

#define MAX 100 



typedef struct s_dico_data * t_dico_data;
typedef struct s_hashtable * t_hashtable;
typedef struct t_hashtable t_dictionary;

struct s_dico_data
{
  void * key;
  void * value;
  t_dico_data next;
};

struct s_hashtable
{
  t_dico_data *h_table;
  size_t size;
  size_t (*hash_function)(void*, size_t);
};

int hash_function(void * p, size_t size)
{
  int i_p = ((int) p) % size;
  if (i_p < 0)
    return -i_p;
  return i_p;
}

int is_cell_empty(t_hashtable table, int cell)
{
  if (table->h_table[cell])
    return 0;
  return 1;
}



void hash_table_print(t_hashtable ht);

t_hashtable hash_table_init(size_t size, size_t (*h_function)(void*, size_t));

void dico_data_free(t_dico_data dd);

void hash_table_free(t_hashtable t);

int hash_table_find_add(t_hashtable t, void * data);

int hash_table_find(t_hashtable t, void * key);

void hash_table_dico_add(t_hashtable t, void * key, void * value);

void hash_table_add(t_hashtable t, void * key);

void hash_table_remove(t_hashtable t, void * key);


/* definition of dictionary */

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


#endif /* HASHTABLE_H_ */
