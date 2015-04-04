int variable_globale;
int testswitch(int a)
{
  int b;
  a = 0;
  b = 0;
  switch(a)
  {
    case 1:
      b = 1;
      break;
    case 2:
      b = 2;
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
