typedef struct stack
{
  int data;
  struct stack *prev;
} Stack;

/* Fonctions */

void push(Stack **stack, int data)
{
  Stack *new = malloc(sizeof *new);
  if(new != NULL)
  {
    new->data = data;
    new->prev = *stack;
    *stack = new;
  }

}

int pop(Stack **stack)
{
  int r = -1;
  if(stack != NULL)
  {
    Stack *temp = (*stack)->prev;
    r = (*stack)->data;
    free(*stack);
    *stack = NULL;
    *stack = temp;
  }
  return r;
}

void clear(Stack **stack)
{
  while(Stack **stack)
  {
    pop(stack);
  }
}
