#include <standard_lib.h>
#include <stdio.h>
#include <stdlib.h>
struct t_enregistrement;
typedef struct t_enregistrement t_enregistrement;
typedef struct t_enregistrement
{
  int a;
  int b;
} t_enregistrement;
typedef t_enregistrement *t_ptr;
void test(void)
{
  t_ptr p;
  p = malloc(sizeof(*(p)));
  (*p).a = 1;
  (*p).b = 1;
}
int main(void)
{
  test();
}
