typedef struct t_file
{
  int data;
  struct t_file *suiv;
} t_file;

/* Fonctions */

void enfiler(t_file **file, int data)
{
  file *new = malloc(sizeof * new);
  if(new != NULL)
  {
    new->suiv = NULL;
    new->data = data;
    if(*file == NULL)
      *file = new;
    else
    {
      file *tmp = *queue;
      while(tmp->suiv != NULL)
      {
	tmp = tmp->suiv;
      }
      tmp->suiv = new;
    }
  }
}

int defiler(t_file **file)
{
  int r = -1;
  if(*file != NULL)
  {
    t_file *tmp = (*file)->suiv;
    r = (*file)->data;
    free(*file);
    *file = NULL;
    *file = tmp;
  }
  return r;
}
