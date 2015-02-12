#include "hashtable.h"

/* print hashtable for debug  */
void hash_table_print(t_hashtable ht)
{
  int i;
  for (i = 0; i < ht->size; i++)
  {
      printf("%p", ht->h_table[i]);
      if (ht->h_table[i])
      {
        t_dico_data temp = NULL;
        temp = ht->h_table[i]->next;
        while(temp)
        {
          printf(" -> %p", temp);
          temp = temp->next;
        }
      }
      printf("\n");

  }
}

/* create hashtable  */
t_hashtable hash_table_init(size_t size, size_t (*h_function)(void*, size_t))
{
  t_hashtable new_htab = malloc(sizeof (struct s_hashtable));
  new_htab->size = size;
  new_htab->h_table = malloc (sizeof (struct s_dico_data) * size);
  int i;
  for (i = 0; i < new_htab->size; i++)
    new_htab->h_table[i] = NULL;

  new_htab->hash_function = h_function;
  
  return new_htab;
}

void dico_data_free(t_dico_data dd)
{
  if(dd->next)
    dico_data_free(dd->next);
  free(dd);
}

/* free hashtable */
void hash_table_free(t_hashtable t)
{
  int i;
  for (i = 0; i < t->size; i++)
  {

      if(t->h_table[i])
        dico_data_free(t->h_table[i]);
      printf("%d\n",i);
  }
  free(t);

}

/* look if the key exist, doesnt exist or if the cell is empty */
int hash_table_find_add(t_hashtable t, void * data)
{
  size_t h = t->hash_function(data, t->size);
  if (is_cell_empty(t, h))
    return 2;
  t_dico_data aux = t->h_table[h];
  while(aux->next && aux->key != data)
  {
    if (aux->next)
      aux = aux->next;
  }
  if (aux->key == data)
    return 1;

  return 0;
}

/* look for a key in the hashtable, return 0 if not found 1 if found */
int hash_table_find(t_hashtable t, void * key)
{
  if (hash_table_find_add(t, key) == 1)
    return 1;
  return 0;
}

/* add a dictionary element (key, value) */
void hash_table_dico_add(t_hashtable t, void * key, void * value)
{
  size_t h = t->hash_function(key, t->size);
  t_dico_data nd = malloc(sizeof (struct s_dico_data));
  nd->key = key;
  nd->value = value;
  nd->next = t->h_table[h];
  t->h_table[h] = nd;
  }
}

/* add a key in the hashtable */
void hash_table_add(t_hashtable t, void * key)
{
  hash_table_dico_add(t, key, NULL);
}

/* remove a key in the hashtable */
void hash_table_remove(t_hashtable t, void * key)
{
  size_t h = t->hash_function(key, t->size);
  int find = hash_table_find(t, key);
  if (find)
  {
    t_dico_data tmp = t->h_table[h];
    t_dico_data tmp_f = t->h_table[h];

    while(tmp->key != key)
    {
      if (tmp->next->key == key)
        tmp_f = tmp;
      tmp = tmp->next;
    }
    tmp_f->next = tmp->next;
    free(tmp);
  }

}


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


