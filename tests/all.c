int variable_globale;
int coucou(int a, int b)
{
  char c;
  int i, n;
  n = 10;
  for (i = 1; i <= n; ++(i))
  {
    b = (b + 1);
  }
  while ((b < 10))
  {
    b = (b + 1);
  }
  do
  {
    a = 1;
  } while ((b < 20));
  if ((b < 10))
  {
    a = 0;
  }
  else
  {
    b = 1;
  }
  switch (n)
  {
    case 1:
      b = 1;
      break;
    case 2:
      b = 2;
      break;
  }
  return 0;
}
int main(void)
{
  coucou(1, 1);
}
