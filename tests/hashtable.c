#include "data_struct/hashtable/hashtable.h"
#include <assert.h>
#include <stdlib.h>

#define N 10
#define HT_CAPACITY 11

size_t hash_int(int i)
{
  return i;
}

int int_equal(int i, int j)
{
  return i == j;
}

void noop(int i)
{
  (void)i;
}

int main()
{
  int a;
  ht_tpl(int) ht;
  ht_init(ht, HT_CAPACITY, hash_int, int_equal, noop);

  for (int n = 0; n < N; ++n)
    ht_add(ht, n);

  assert(ht.length == 10);

  for (int n = 0; n < 10; ++n)
  {
    assert(ht_find(ht, n, &a));
    assert(a == n);
  }

  ht_del(ht, 5);
  assert(ht.length == N - 1);
  assert(!ht_find(ht, 5, &a));

  ht_del(ht, 4);
  assert(ht.length == N - 2);
  assert(!ht_find(ht, 4, &a));
  assert(ht_find(ht, 7, &a));

  ht_free(ht);
  return 0;
}
