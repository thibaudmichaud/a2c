#ifndef LIST_H_
#define LIST_H_

#define BASE_CAPACITY 4

#define list_tpl(t) \
struct { \
  size_t capacity; \
  size_t size; \
  size_t elem_size; \
  t *data; \
}

#define define_list(t, name) \
struct name { \
  size_t capacity; \
  size_t size; \
  size_t elem_size; \
  t *data; \
}

#define list_init(l) \
(l).capacity = BASE_CAPACITY; \
(l).size = 0; \
(l).data = malloc(sizeof(*(l).data) * (l).capacity)

#define list_push_back(l, elt) \
do { \
  if ((l).size + 1 > (l).capacity) \
  { \
    (l).capacity *= 2; \
    (l).data = realloc((l).data, (l).capacity * (sizeof(*(l).data))); \
  } \
  (l).data[(l).size] = (elt); \
  (l).size++; \
} while (0)

#define list_pop_back(l) (l).data[--(l).size]; \

#define list_insert(l, i, elt) \
do { \
  if ((l).size + 1 > (l).capacity) \
  { \
    (l).data = realloc((l).data, (l).capacity * (sizeof(*(l).data)) * 2); \
    (l).capacity *= 2; \
  } \
  size_t tmp = i; \
  for (size_t j = (l).size; j > (tmp); --j) \
    (l).data[j] = (l).data[j - 1]; \
  (l).data[tmp] = elt; \
  ++(l).size; \
} while (0)

#define list_del(l, i) \
do { \
  assert((l).size > 0); \
  size_t tmp = i; \
  for (size_t j = tmp; j < (l).size; ++j) \
    (l).data[j] = (l).data[j + 1]; \
  --(l).size; \
} while(0)

#define list_push_front(l, elt) list_insert(l, 0, elt)

#define list_pop_front(l) list_del(l, 0)

#define list_nth(l, n) (l).data[n]

#define list_free(l) \
do { \
  free((l).data); \
} while(0)

#endif /* LIST_H_ */
