typedef enum
{
  a1,
  a2,
  a3,
  a4,
} t_enum;
typedef int t_vect_entiers[5];
typedef int t_tab_bool[5][6];
typedef struct
{
  int a, b;
  int c;
} t_enregistrement;
typedef int *t_int_ptr;
int variable_globale;
int n;
void test(int b, int *gp, int *bla, char *z)
{
  t_tab_bool tab;
  int a, b;
  double c;
  tab[2][3] = 4;
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
  test(variable_globale, n);
}
error in assignment
