#include <stdio.h>
#include <stdlib.h>
struct t_enregistrement;
typedef struct t_enregistrement t_enregistrement;
typedef struct t_enregistrement
{
  int i, j, k;
  int b;
  char c;
} t_enregistrement;
typedef t_enregistrement *t_ptr;
void test(void)
{
  t_ptr p;
  (*p).i = 1;
}
int main(void)
{
  test();
}
