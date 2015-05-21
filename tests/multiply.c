#include <stdio.h>
#include <stdlib.h>
int multiply(int a, int b)
{
  int p, i;
  p = 0;
  for (i = 1; i <= b; ++(i))
  {
    p = (p + a);
  }
  return p;
}
int main(void)
{
  printf("%d\n", multiply(3, 4));
}
