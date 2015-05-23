typedef struct t_pile
{
  int data;
  struct t_pile *prev;
} t_pile;

/* Fonctions */

void empiler(t_pile **pile, int data)
{
  t_pile *new = malloc(sizeof *new);
  if(new != NULL)
  {
    new->data = data;
    new->prev = *pile;
    *pile = new;
  }

}

int depiler(t_pile **pile)
{
  int r = -1;
  if(pile != NULL)
  {
    t_pile *temp = (*pile)->prev;
    r = (*pile)->data;
    free(*pile);
    *pile = NULL;
    *pile = temp;
  }
  return r;
}

void vider(t_pile **pile)
{
  while(pile)
  {
    depiler(pile);
  }
}
