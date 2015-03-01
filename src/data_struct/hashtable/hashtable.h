#ifndef _DATA_STRUCT_HASHTABLE_H
#define _DATA_STRUCT_HASHTABLE_H

#include <stdlib.h>
#include <stdio.h>

#define ht_tpl(type)                                                          \
struct                                                                        \
{                                                                             \
  size_t size;                                                                \
  size_t length;                                                              \
  size_t (*hash)(type);                                                       \
  int (*equal)(type, type);                                                   \
  struct bucket { type elt; struct bucket *next; } **buckets;                 \
}

#define ht_init(_ht, _size, _hash, _equal)                                    \
do                                                                            \
{                                                                             \
  (_ht).size = (_size);                                                       \
  (_ht).length = 0;                                                           \
  (_ht).hash = _hash;                                                         \
  (_ht).equal = _equal;                                                       \
  (_ht).buckets = calloc(_size, sizeof(*(_ht).buckets));                      \
} while(0)

#define ht_add(_ht, _elt)                                                     \
do {                                                                          \
  typedef __typeof__((_ht).buckets[0]) bucket;                                \
  typedef __typeof__(_elt) type;                                              \
  size_t i = (_ht).hash(_elt) % (_ht).size;                                   \
  bucket nhead = malloc(sizeof(struct { type elt; bucket next; }));           \
  nhead->elt = (_elt);                                                        \
  nhead->next = (_ht).buckets[i];                                             \
  (_ht).buckets[i] = nhead;                                                   \
  ++(_ht).length;                                                             \
} while(0)

#define ht_del(_ht, _elt)                                                     \
do {                                                                          \
  typedef __typeof__((_ht).buckets[0]) bucket;                                \
  size_t i = (_ht).hash(_elt) % (_ht).size;                                   \
  bucket prev = NULL;                                                         \
  bucket b = (_ht).buckets[i];                                                \
  while(b)                                                                   \
  {                                                                           \
    if ((_ht).equal(b->elt, _elt))                                            \
    {                                                                         \
      if (prev)                                                               \
      {                                                                       \
        prev->next = b->next;                                                 \
        free(b);                                                              \
        b = prev;                                                             \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        (_ht).buckets[i] = b->next;                                           \
        free(b);                                                              \
        b = (_ht).buckets[i];                                                 \
      }                                                                       \
      --(_ht).length;                                                         \
    }                                                                         \
  }                                                                           \
} while(0)

#define ht_find(_ht, _elt, _res) ({                                           \
  typedef __typeof__((_ht).buckets[0]) bucket;                                \
  int found = 0;                                                              \
  size_t i = (_ht).hash(_elt) % (_ht).size;                                   \
  for (bucket b = (_ht).buckets[i]; b != NULL; b = b->next)                   \
  {                                                                           \
    if ((_ht).equal(b->elt, _elt))                                            \
    {                                                                         \
      *_res = b->elt;                                                         \
      found = 1;                                                              \
      break;                                                                  \
    }                                                                         \
  }                                                                           \
  found; })

#define ht_free(_ht)                                                          \
do {                                                                          \
  typedef __typeof__((_ht).buckets[0]) bucket;                                \
  for (unsigned i = 0; i < (_ht).size; ++i)                                   \
  {                                                                           \
    bucket next;                                                              \
    bucket b = (_ht).buckets[i];                                              \
    while (b)                                                                 \
    {                                                                         \
      next = b->next;                                                         \
      free(b);                                                                \
      b = next;                                                               \
    }                                                                         \
  }                                                                           \
  free(ht.buckets);                                                           \
} while(0)

#endif
