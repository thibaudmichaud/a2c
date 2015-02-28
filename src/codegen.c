#include <stdio.h>
#include "grammar.h"
#include "codegen.h"
#include "parser.h"

char *getopstr(int op)
{
  switch (op)
  {
    case PLUS:
      return "+";
    case MINUS:
      return "-";
    case STAR:
      return "*";
    case SLASH:
      return "/";
    case DIV:
      return "div";
    case OR:
      return "||";
    case AND:
      return "&&";
    case MOD:
      return "%";
    case EQ:
      return "=";
    case NEQ:
      return "<>";
    default:
      return NULL;
  }
}

void print_algo(struct algo *algo)
{
  printf("void ");
  printf("%s(void)\n{\n", algo->ident);
  print_instructions(algo->instructions);
  printf("}\n");
}

void free_algo(struct algo *algo)
{
  free_instructions(algo->instructions);
  free(algo->ident);
  free(algo);
}

void print_instructions(struct block *instructions)
{
  for (unsigned i = 0; i < instructions->list.size - 1; ++i)
    print_instruction(list_nth(instructions->list, i));
  print_instruction(list_nth(instructions->list, instructions->list.size - 1));
}

void print_instruction(struct instruction *i)
{
  switch (i->kind)
  {
    case assignment:
      print_expression(i->instr.assignment.e1);
      printf(" = ");
      print_expression(i->instr.assignment.e2);
      printf(";\n");
      break;
    case whiledo:
      printf("while (");
      print_expression(i->instr.whiledo.cond);
      printf(") {\n");
      print_instructions(i->instr.whiledo.instructions);
      printf("}\n");
      break;
    default:
      printf("instruction not handled yet\n");
  }
}

void free_instructions(struct block *instructions)
{
  for (unsigned i = 0; i < instructions->list.size; ++i)
    free_instruction(list_nth(instructions->list, i));
  list_free(instructions->list);
  free(instructions);
}

void free_instruction(struct instruction *i)
{
  switch (i->kind)
  {
    case funcall:
      // TODO
      break;
    case assignment:
      free_expression(i->instr.assignment.e1);
      free_expression(i->instr.assignment.e2);
      break;
    case ifthenelse:
      // TODO
      break;
    case switchcase:
      // TODO free list of instructions once lists are implemented
      break;
    case dowhile:
      break;
    case whiledo:
      free_expression(i->instr.whiledo.cond);
      free_instructions(i->instr.whiledo.instructions);
      break;
    case forloop:
      // TODO
      break;
    case returnstmt:
      break;
  }
  free(i);
}

void print_expression(struct expr *e)
{
  switch (e->exprtype)
  {
    case inttype:
      printf("%d", e->val.intval);
      break;
    case binopexprtype:
      printf("(");
      print_expression(e->val.binopexpr.e1);
      // TODO handle the XOR case here, because there is no corresponding
      // operator in C.
      printf(" %s ", getopstr(e->val.binopexpr.op));
      print_expression(e->val.binopexpr.e2);
      printf(")");
      break;
    case unopexprtype:
      printf("(");
      printf("%s ", getopstr(e->val.unopexpr.op));
      print_expression(e->val.unopexpr.e);
      printf(")");
      break;
    case arrayexprtype:
      print_expression(e->val.arrayexpr.e1);
      printf("[");
      print_expression(e->val.arrayexpr.e2);
      printf("]");
      break;
    case identtype:
      printf("%s", e->val.ident);
      break;
    case stringtype:
      printf("%s", e->val.stringval);
      break;
    case booltype:
      printf("%d", e->val.boolval == true);
      break;
    case dereftype:
      printf("*");
      print_expression(e->val.deref.e);
      break;
    case funcalltype:
      printf("%s(", e->val.funcall.fun_ident);
      unsigned i = 0;
      for (; i < e->val.funcall.args->list.size - 1; ++i)
      {
        print_expression(list_nth(e->val.funcall.args->list, i));
        printf(", ");
      }
      print_expression(list_nth(e->val.funcall.args->list, i));
      printf(")");
      break;
    default:
      printf("expr not handled yet\n");
  }
}

void free_expressions(struct exprlist *l)
{
  for (unsigned i = 0; i < l->list.size; ++i)
    free_expression(list_nth(l->list, i));
  list_free(l->list);
  free(l);
}

void free_expression(struct expr *e)
{
  switch (e->exprtype)
  {
    case binopexprtype:
      free_expression(e->val.binopexpr.e1);
      free_expression(e->val.binopexpr.e2);
      break;
    case unopexprtype:
      free_expression(e->val.unopexpr.e);
      break;
    case arrayexprtype:
      free_expression(e->val.arrayexpr.e1);
      free_expression(e->val.arrayexpr.e2);
      break;
    case identtype:
      free(e->val.ident);
      break;
    case stringtype:
      free(e->val.stringval);
      break;
    case dereftype:
      free_expression(e->val.deref.e);
      break;
    case funcalltype:
      free_expressions(e->val.funcall.args);
      free(e->val.funcall.fun_ident);
      break;
    case inttype:
    case booltype:
    default:
      break;
  }
  free(e);
}
