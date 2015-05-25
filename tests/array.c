#include <standard_lib.h>
#include <stdio.h>
#include <stdlib.h>
typedef int t_tab_bool[5][6];
void test(void)
{
  int i, j;
  t_tab_bool tab;
  for (i = 1; i <= 5; ++(i))
  {
    for (j = 1; j <= 6; ++(j))
    {
      tab[i - 1][j - 1] = (i * j);
    }
  }
}
int main(void)
{
  test();
}
