typedef struct queue
{
  int data;
  struct queue *suiv;
} Queue;

/* Fonctions */

void enqueue(Queue **queue, int data)
{
  Queue *new = malloc(sizeof * new);
  if(new != NULL)
  {
    new->suiv = NULL;
    new->data = data;
    if(*queue == NULL)
      *queue = new;
    else
    {
      Queue *tmp = *queue;
      while(tmp->suiv != NULL)
      {
	tmp = tmp->suiv;
      }
      tmp->suiv = new;
    }
  }
}

int dequeue(Queue **queue)
{
  int r = -1;
  if(*queue != NULL)
  {
    Queue *tmp = (*queue)->suiv;
    r = (*queue)->data;
    free(*queue);
    *queue = NULL;
    *queue = tmp;
  }
  return r;
}
