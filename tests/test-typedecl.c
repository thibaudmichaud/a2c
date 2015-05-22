#include <stdio.h>
#include <stdlib.h>
struct t_enregistrement;
typedef struct t_enregistrement t_enregistrement;
typedef enum
{
  a1,
  a2,
  a3,
  a4,
} t_enum;
typedef int t_vect_entiers[5];
typedef int t_tab_bool[5][6];
typedef t_enregistrement *t_ptr;
typedef struct t_enregistrement
{
  int a, b;
  int c;
} t_enregistrement;
typedef int *t_int_ptr;
int variable_globale;
int n;
void test(int d, int *gp, int *bla)
{
  t_tab_bool tab;
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
  test(variable_globale, n, 2);
}
