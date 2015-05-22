#include <stdio.h>
#include <stdlib.h>
typedef int *t_ptr;
typedef t_ptr *t_ptr_ptr;
int variable_globale;
int n;
t_ptr_ptr p;
void test(t_ptr_ptr p)
{
  p = NULL;
  **p = 1;
}
t_ptr_ptr foo(void)
{
  return NULL;
}
int main(void)
{
  test(p);
}
