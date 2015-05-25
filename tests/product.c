#include <standard_lib.h>
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
int product(int a, int b)
{
  if ((a == 0))
  {
    return 0;
  }
  else
  {
    if ((a == 1))
    {
      return b;
    }
    else
    {
      if ((b < 0))
      {
        return multiply((- a), (- b));
      }
      else
      {
        return multiply(a, b);
      }
    }
  }
}
int main(void)
{
  printf("%d\n", product(2, (- 3)));
}
