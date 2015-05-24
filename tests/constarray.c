#include <standard_lib.h>
#include <stdio.h>
#include <stdlib.h>
const int max = 5;
const int min = 3;
typedef int t_tab_int[max][min];
void blabla(void)
{
  int i, j;
  t_tab_int mat;
  i = 1;
  j = 2;
  mat[i - 1][j - 1] = 4;
}
int main(void)
{
  blabla();
}
