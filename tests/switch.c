int variable_globale;
int testswitch(int n)
{
  int a, b;
  a = 0;
  b = 0;
  switch (n)
  {
    case 1:
      b = 1;
      break;
    default:
      b = 0;
      break;
  }
  return b;
}
int main(void)
{
  ecrire(testswitch(2));
}
