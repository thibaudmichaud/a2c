#include <standard_lib.h>
#include <stdio.h>
#include <stdlib.h>
struct t_liste_struct;
typedef struct t_liste_struct t_liste_struct;
typedef t_liste_struct *t_liste;
typedef struct t_liste_struct
{
  int a;
  t_liste next;
} t_liste_struct;
int variable_globale;
int n;
void test(void)
{
  t_liste l;
  l = (*l).next;
}
int main(void)
{
  test();
}
