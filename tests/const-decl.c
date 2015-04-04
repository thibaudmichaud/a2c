int variable_globale;
int fibo(int n)
{
  const int a = 0;
  const double r = 4.2;
  int i, a, b;
  a = 0;
  b = 1;
  for (i = 1; i <= n; ++(i))
  {
    b = (a + b);
    a = (b - a);
  }
  return a;
}
int main(void)
{
  ecrire(fibo(10));
}
