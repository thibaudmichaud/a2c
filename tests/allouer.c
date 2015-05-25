#include <stdio.h>
#include <stdlib.h>
struct t_enregistrement;
typedef struct t_enregistrement t_enregistrement;
typedef struct t_enregistrement
{
  int a;
  int b;
} t_enregistrement;
void test(void)
{
  t_enregistrement e;
  malloc(sizeof(t_enregistrement));
}
int main(void)
{
  test();
}
