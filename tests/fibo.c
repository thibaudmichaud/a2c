int variable_globale;
int fibo(int n)
{
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
  printf("%d\n", fibo(10));
}
