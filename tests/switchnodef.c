int variable_globale;
int testswitch(int n)
{
  int a, b;
  a = 0;
  b = 0;
  switch (n)
  {
    case 1:
    case 2:
      b = 1;
      break;
    case 3:
      b = 2;
      break;
  }
  return b;
}
int main(void)
{
  ecrire(testswitch(2));
}
