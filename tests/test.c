#include <standard_lib.h>
#include <stdio.h>
#include <stdlib.h>
int variable_globale;
void test(void)
{
  int a, b;
  double c;
  a = 0;
  c = 4.5;
  if ((a == b))
  {
    b = (a + b);
  }
  else
  {
    a = (a - b);
  }
}
int main(void)
{
  test();
}
