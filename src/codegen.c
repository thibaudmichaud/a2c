#include <stdio.h>
#include "grammar.h"
#include "codegen.h"
#include "parser.h"
#define INDENT_WIDTH 2

void print_indent(int indent)
{
  for (int i = 0; i < indent; ++i)
    printf(" ");
}

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
      return "==";
    case LE:
      return "<=";
    case LT:
      return "<";
    case GT:
      return ">";
    case GE:
      return ">=";
    case NEQ:
      return "<>";
    default:
      return NULL;
  }
}

void print_single_var_decl(struct single_var_decl *single_var_decl, int indent)
{
  print_indent(indent);
  printf("%s ", single_var_decl->type_ident);
  unsigned i = 0;
  for (; i + 1 < single_var_decl->var_idents.size; ++i)
    printf("%s, ", list_nth(single_var_decl->var_idents, i));
  printf("%s;\n", list_nth(single_var_decl->var_idents, i));
}

void print_var_decl(vardecllist_t var_decl, int indent)
{
  for (unsigned i = 0; i < var_decl.size; ++i)
    print_single_var_decl(list_nth(var_decl, i), indent);
}

void print_enum(char *ident, struct enum_def *enum_def)
{
  printf("typedef enum\n{\n");
  for (unsigned i = 0; i < enum_def->identlist.size; ++i)
  {
    print_indent(INDENT_WIDTH);
    printf("%s,\n", enum_def->identlist.data[i]);
  }
  printf("} %s\n", ident);
}

void print_array(char *ident, struct array_def *array_def)
{
  printf("typedef ");
  printf("%s %s", ident, array_def->elt_type);
  for (unsigned i = 0; i < array_def->dims.size; ++i)
  {
    printf("[%d]", array_def->dims.data[i]);
  }
  printf("\n");
}

void print_record(struct record_def *record_def)
{
  printf("typedef struct\n{\n");
  print_var_decl(record_def->var_decl, INDENT_WIDTH);
  printf("} %s;\n", record_def->ident);
}

void print_pointer(char *ident, struct pointer_def *pointer_def)
{
  printf("typedef %s *%s;\n", pointer_def->pointed_type_ident, ident);
}

void print_type_decl(struct type_decl *type_decl)
{
  switch (type_decl->type_def->type_type)
  {
    case enum_type:
      print_enum(type_decl->ident, type_decl->type_def->def.enum_def);
      break;
    case array_type:
      print_array(type_decl->ident, type_decl->type_def->def.array_def);
      break;
    case struct_type:
      print_record(type_decl->type_def->def.record_def);
      break;
    case pointer_type:
      print_pointer(type_decl->ident, type_decl->type_def->def.pointer_def);
      break;
    default:
      printf("type not handled yet (in print_type_decl)\n");
  }
}

void print_type_decls(typedecllist_t type_decls)
{
  for (unsigned i = 0; i < type_decls.size; ++i)
  {
    print_type_decl(type_decls.data[i]);
  }
}

void print_prog(struct prog *prog)
{
  print_type_decls(prog->algo->declarations->type_decls);
  print_var_decl(prog->entry_point->var_decl, 0);
  print_algo(prog->algo);
  printf("int main(void)\n{\n");
  print_instructions(prog->entry_point->instructions, INDENT_WIDTH);
  printf("}\n");
}

void print_lp_decl(struct local_param *local_param)
{
  unsigned i = 0;
  for (; i + 1 < local_param->param.size; ++i)
  {
    for (unsigned k = 0; k < list_nth(local_param->param, i)->var_idents.size; ++k)
    {
      printf("%s %s, ", list_nth(local_param->param, i)->type_ident, 
          list_nth(list_nth(local_param->param, i)->var_idents, k));
    }
  }
  unsigned k = 0;
  for (; k + 1 < list_nth(local_param->param, i)->var_idents.size; ++k)
  {
    printf("%s %s, ", list_nth(local_param->param, i)->type_ident, 
        list_nth(list_nth(local_param->param, i)->var_idents, k));
  }
  printf("%s %s", list_nth(local_param->param, i)->type_ident, 
      list_nth(list_nth(local_param->param, i)->var_idents, k));
}

void print_gp_decl(struct global_param *global_param)
{
  unsigned i = 0;
  for (; i + 1 < global_param->param.size; ++i)
  {
    for (unsigned k = 0; k < global_param->param.data[i]->var_idents.size; ++k)
    {
      printf("%s *%s, ", global_param->param.data[i]->type_ident, 
          global_param->param.data[i]->var_idents.data[k]);
    }
  }
  unsigned k = 0;
  for (; k + 1 < global_param->param.data[i]->var_idents.size; ++k)
  {
    printf("%s *%s, ", global_param->param.data[i]->type_ident, 
        global_param->param.data[i]->var_idents.data[k]);
  }
  printf("%s *%s", global_param->param.data[i]->type_ident, 
      global_param->param.data[i]->var_idents.data[k]);
}

void print_param_decl(struct param_decl *param_decl)
{
  if (!param_decl->local_param && !param_decl->global_param)
  {
    printf("void");
    return;
  }
  if (param_decl->local_first)
  {
    if (param_decl->local_param)
    {
      print_lp_decl(param_decl->local_param);
      if (param_decl->global_param)
        printf(", ");
    }
    if (param_decl->global_param)
    {
      print_gp_decl(param_decl->global_param);
    }
  }
  else
  {
    if (param_decl->global_param)
    {
      print_gp_decl(param_decl->global_param);
      if (param_decl->local_param)
        printf(", ");
    }
    if (param_decl->local_param)
    {
      print_lp_decl(param_decl->local_param);
    }
  }
}

void print_algo(struct algo *algo)
{
  if (algo->return_type)
    printf("%s ", algo->return_type);
  else
    printf("void ");
  printf("%s(", algo->ident);
  print_param_decl(algo->declarations->param_decl);
  printf(")\n{\n");
  print_var_decl(algo->declarations->var_decl, INDENT_WIDTH);
  print_instructions(algo->instructions, INDENT_WIDTH);
  printf("}\n");
}

void free_identlist(identlist_t idents)
{
  for (unsigned i = 0; i < idents.size; ++i)
    free(idents.data[i]);
  list_free(idents);
}

void free_single_var_decl(struct single_var_decl *single_var_decl)
{
  free(single_var_decl->type_ident);
  free_identlist(single_var_decl->var_idents);
  free(single_var_decl);
}

void free_param_decl(struct param_decl *param_decl)
{
  if (param_decl->local_param)
  {
    for (unsigned i = 0; i < param_decl->local_param->param.size; ++i)
      free_single_var_decl(param_decl->local_param->param.data[i]);
    list_free(param_decl->local_param->param);
    free(param_decl->local_param);
  }
  if (param_decl->global_param)
  {
    for (unsigned i = 0; i < param_decl->global_param->param.size; ++i)
      free_single_var_decl(param_decl->global_param->param.data[i]);
    list_free(param_decl->global_param->param);
    free(param_decl->global_param);
  }
  free(param_decl);
}

void free_var_decl(vardecllist_t var_decl)
{
  for (unsigned i = 0; i < var_decl.size; ++i)
    free_single_var_decl(list_nth(var_decl, i));
  list_free(var_decl);
}

void free_type_def(struct type_def *type_def)
{
  switch(type_def->type_type)
  {
    case array_type:
      free(type_def->def.array_def->elt_type);
      list_free(type_def->def.array_def->dims);
      free(type_def->def.array_def);
      break;
    case enum_type:
      free_identlist(type_def->def.enum_def->identlist);
      free(type_def->def.enum_def);
      break;
    case struct_type:
      free(type_def->def.record_def->ident);
      free_var_decl(type_def->def.record_def->var_decl);
      free(type_def->def.record_def);
      break;
    case pointer_type:
      free(type_def->def.pointer_def->pointed_type_ident);
      free(type_def->def.pointer_def);
      break;
    default:
      printf("type not handled yet (in free_type_def)");
  }
  free(type_def);
}

void free_type_decl(struct type_decl *type_decl)
{
  free(type_decl->ident);
  free_type_def(type_decl->type_def);
  free(type_decl);
}

void free_type_decls(typedecllist_t type_decls)
{
  for (unsigned i = 0; i < type_decls.size; ++i)
  {
    free_type_decl(type_decls.data[i]);
  }
  list_free(type_decls);
}

void free_decls(struct declarations *declarations)
{
  free_param_decl(declarations->param_decl);
  free_type_decls(declarations->type_decls);
  free_var_decl(declarations->var_decl);
  free(declarations);
}

void free_algo(struct algo *algo)
{
  free_instructions(algo->instructions);
  free_decls(algo->declarations);
  free(algo->ident);
  if (algo->return_type)
    free(algo->return_type);
  free(algo);
}

void print_instructions(instructionlist_t instructions, int indent)
{
  unsigned i = 0;
  for (; i + 1 < instructions.size; ++i)
    print_instruction(list_nth(instructions, i), indent);
  if (instructions.size > 0)
    print_instruction(list_nth(instructions, i), indent);
}

void print_exprlist(exprlist_t l)
{
  unsigned i = 0;
  for (; i + 1 < l.size; ++i)
  {
    print_expression(list_nth(l, i));
    printf(", ");
  }
  if (l.size > 0)
    print_expression(list_nth(l, i));
}

void print_instruction(struct instruction *i, int indent)
{
  switch (i->kind)
  {
    case assignment:
      print_indent(indent);
      print_expression(i->instr.assignment.e1);
      printf(" = ");
      print_expression(i->instr.assignment.e2);
      printf(";\n");
      break;
    case whiledo:
      print_indent(indent);
      printf("while (");
      print_expression(i->instr.whiledo.cond);
      printf(")\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.whiledo.instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      break;
    case dowhile:
      print_indent(indent);
      printf("do\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.dowhile.instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("} while (");
      print_expression(i->instr.dowhile.cond);
      printf(");\n");
      break;
    case forloop:
      print_indent(indent);
      printf("for (; ");
      print_expression(i->instr.forloop.assignment->e1);
      if (i->instr.forloop.decreasing)
        printf(" >= ");
      else
        printf(" <= ");
      print_expression(i->instr.forloop.upto);
      if (i->instr.forloop.decreasing)
        printf("; --(");
      else
        printf("; ++(");
      print_expression(i->instr.forloop.assignment->e1);
      printf("))\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.forloop.instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      break;
    case funcall:
      print_indent(indent);
      printf("%s(", i->instr.funcall.fun_ident);
      print_exprlist(i->instr.funcall.args);
      printf(");\n");
      break;
    case ifthenelse:
      print_indent(indent);
      printf("if (");
      print_expression(i->instr.ifthenelse.cond);
      printf(")\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.ifthenelse.instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      if (i->instr.ifthenelse.elseblock.size > 0)
      {
        print_indent(indent);
        printf("else\n");
        print_indent(indent);
        printf("{\n");
        print_instructions(i->instr.ifthenelse.elseblock, indent + INDENT_WIDTH);
        print_indent(indent);
        printf("}\n");
      }
      break;
    case returnstmt:
      print_indent(indent);
      printf("return ");
      print_expression(i->instr.returnstmt.expr);
      printf(";\n");
      break;
    default:
      printf("instruction not handled yet\n");
  }
}

void free_instructions(instructionlist_t instructions)
{
  for (unsigned i = 0; i < instructions.size; ++i)
    free_instruction(list_nth(instructions, i));
  list_free(instructions);
}

void free_instruction(struct instruction *i)
{
  switch (i->kind)
  {
    case assignment:
      free_expression(i->instr.assignment.e1);
      free_expression(i->instr.assignment.e2);
      break;
    case switchcase:
      // TODO free list of instructions once lists are implemented
      break;
    case dowhile:
      free_expression(i->instr.dowhile.cond);
      free_instructions(i->instr.dowhile.instructions);
      break;
    case whiledo:
      free_expression(i->instr.whiledo.cond);
      free_instructions(i->instr.whiledo.instructions);
      break;
    case forloop:
      free_expression(i->instr.forloop.assignment->e1);
      free_expression(i->instr.forloop.assignment->e2);
      free(i->instr.forloop.assignment);
      free_instructions(i->instr.forloop.instructions);
      free_expression(i->instr.forloop.upto);
      break;
    case funcall:
      free_expressions(i->instr.funcall.args);
      free(i->instr.funcall.fun_ident);
      break;
    case ifthenelse:
      free_expression(i->instr.ifthenelse.cond);
      free_instructions(i->instr.ifthenelse.instructions);
      if (i->instr.ifthenelse.elseblock.size > 0)
        free_instructions(i->instr.ifthenelse.elseblock);
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
    case realtype:
      printf("%g", e->val.realval);
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
      for (unsigned i = 0; i < e->val.arrayexpr.indices.size; ++i)
      {
        printf("[");
        print_expression(list_nth(e->val.arrayexpr.indices, i));
        printf("]");
      }
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
      print_exprlist(e->val.funcall.args);
      printf(")");
      break;
    default:
      printf("expr not handled yet\n");
  }
}

void free_expressions(exprlist_t l)
{
  for (unsigned i = 0; i < l.size; ++i)
    free_expression(list_nth(l, i));
  list_free(l);
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
      free_expressions(e->val.arrayexpr.indices);
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

void free_prog(struct prog *prog)
{
  free_var_decl(prog->entry_point->var_decl);
  free_instructions(prog->entry_point->instructions);
  free(prog->entry_point);
  free_algo(prog->algo);
}
