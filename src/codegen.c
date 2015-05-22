#include <stdio.h>
#include "grammar.h"
#include "codegen.h"
#include "parser.h"
#include "lexer.h"
#include "typecheck.h"
#include "funtable.h"
#include <assert.h>

#define INDENT_WIDTH 2

void print_indent(int indent)
{
  for (int i = 0; i < indent; ++i)
    printf(" ");
}

char *getopstr(enum tokentype op)
{
  switch (op)
  {
    case PLUS:
      return "+";
    case MINUS:
      return "-";
    case STAR:
    case DEREF:
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
    case NOT:
      return "!";
    case XOR:
      return "!=";
    default:
      return NULL;
  }
}

void print_type_format(char *t)
{
  if (strcmp(t, "caractere") == 0)
      printf("%%c");
  else if (strcmp(t, "booleen") == 0
      || strcmp(t, "entier") == 0)
      printf("%%d");
  else if (strcmp(t, "chaine") == 0)
      printf("%%s");
  else if (strcmp(t, "reel") == 0)
      printf("%%g");
  else if (strcmp(t, "nul") == 0)
      printf("%%p");
  else
    printf("\nUndefined behaviour: printing non-primitive data type\n");
}

void print_write_fun(struct funcall *f)
{
  assert(strcmp(f->fun_ident, "ecrire") == 0);
  printf("printf(\"");
  for (unsigned i = 0; i < f->args.size - 1; ++i)
  {
    print_type_format(f->args.data[i]->e->type);
    printf(" ");
  }
  print_type_format(f->args.data[f->args.size - 1]->e->type);
  printf("\\n\", ");
  print_arglist(f->args);
  printf(");\n");
}

void print_val(struct val *val)
{
  switch (val->valtype)
  {
    case inttype:
      printf("%d", val->val.intval);
      break;
    case realtype:
      printf("%g", val->val.realval);
      break;
    case stringtype:
      printf("\"%s\"", val->val.stringval);
      break;
    case booltype:
      printf("%d", val->val.boolval == true);
      break;
    case chartype:
      printf("'%c'", val->val.charval);
      break;
    case nulltype:
      printf("NULL");
      break;
  }
}

void print_single_var_decl(struct single_var_decl *single_var_decl, int indent)
{
  print_indent(indent);
  printf("%s ", algo_to_c_type(single_var_decl->type_ident));
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
  printf("} %s;\n", ident);
}

void print_array(char *ident, struct array_def *array_def)
{
  printf("typedef ");
  printf("%s %s", algo_to_c_type(array_def->elt_type), ident);
  for (unsigned i = 0; i < array_def->dims.size; ++i)
  {
    printf("[%d]", array_def->dims.data[i]);
  }
  printf(";\n");
}

void print_record(char *ident, struct record_def *record_def)
{
  printf("typedef struct %s\n{\n", ident);
  print_var_decl(record_def->var_decl, INDENT_WIDTH);
  printf("} %s;\n", ident);
}

void print_pointer(char *ident, struct pointer_def *pointer_def)
{
  printf("typedef %s *%s;\n", algo_to_c_type(pointer_def->pointed_type_ident),
      ident);
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
      print_record(type_decl->ident, type_decl->type_def->def.record_def);
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
    if (type_decls.data[i]->type_def->type_type == struct_type)
    {
      printf("struct %s;\n", type_decls.data[i]->ident);
      printf("typedef struct %s %s;\n",
          type_decls.data[i]->ident,
          type_decls.data[i]->ident);
    }
  }
  for (unsigned i = 0; i < type_decls.size; ++i)
  {
    print_type_decl(type_decls.data[i]);
  }
}

void print_prog(struct prog *prog)
{
  printf("#include <stdio.h>\n#include <stdlib.h>\n");
  for (unsigned i = 0; i < prog->algos.size; ++i)
    print_type_decls(prog->algos.data[i]->declarations->type_decls);
  print_type_decls(prog->entry_point->type_decls);
  print_var_decl(prog->entry_point->var_decl, 0);
  for (unsigned i = 0; i < prog->algos.size; ++i)
    print_algo(prog->algos.data[i]);
  printf("int main(void)\n{\n");
  print_instructions(prog->entry_point->instructions, INDENT_WIDTH);
  printf("}\n");
}

void print_lp_decl(vardecllist_t local_param)
{
  unsigned i = 0;
  for (; i + 1 < local_param.size; ++i)
  {
    for (unsigned k = 0; k < list_nth(local_param, i)->var_idents.size; ++k)
    {
      printf("%s %s, ",
          algo_to_c_type(list_nth(local_param, i)->type_ident),
          local_param.data[i]->var_idents.data[k]);
    }
  }
  if (i < local_param.size)
  {
    unsigned k = 0;
    for (; k + 1 < list_nth(local_param, i)->var_idents.size; ++k)
    {
      printf("%s %s, ",
          algo_to_c_type(list_nth(local_param, i)->type_ident),
          list_nth(list_nth(local_param, i)->var_idents, k));
    }
    printf("%s %s",
        algo_to_c_type(list_nth(local_param, i)->type_ident),
        list_nth(list_nth(local_param, i)->var_idents, k));
  }
}

void print_gp_decl(vardecllist_t global_param)
{
  unsigned i = 0;
  for (; i + 1 < global_param.size; ++i)
  {
    for (unsigned k = 0; k < global_param.data[i]->var_idents.size; ++k)
    {
      printf("%s *%s, ",
          algo_to_c_type(global_param.data[i]->type_ident),
          global_param.data[i]->var_idents.data[k]);
    }
  }
  if (i < global_param.size)
  {
    unsigned k = 0;
    for (; k + 1 < global_param.data[i]->var_idents.size; ++k)
    {
      printf("%s *%s, ", algo_to_c_type(global_param.data[i]->type_ident),
          global_param.data[i]->var_idents.data[k]);
    }
    printf("%s *%s",
        algo_to_c_type(global_param.data[i]->type_ident),
        global_param.data[i]->var_idents.data[k]);
  }
}

void print_param_decl(struct param_decl *param_decl)
{
  if (param_decl->local_param.size == 0 && param_decl->global_param.size == 0)
  {
    printf("void");
    return;
  }
  if (param_decl->local_first)
  {
    print_lp_decl(param_decl->local_param);
    if (param_decl->global_param.size > 0)
      printf(", ");
    print_gp_decl(param_decl->global_param);
  }
  else
  {
    print_gp_decl(param_decl->global_param);
    if (param_decl->local_param.size > 0)
      printf(", ");
    print_lp_decl(param_decl->local_param);
  }
}

void print_const_decl(constdecllist_t const_decls, int indent)
{
  for (unsigned i = 0; i < const_decls.size; ++i)
  {
    print_indent(indent);
    printf("const %s %s = ", algo_to_c_type(const_decls.data[i]->type),
        const_decls.data[i]->ident);
    print_val(const_decls.data[i]->val);
    printf(";\n");
  }
}

void print_algo(struct algo *algo)
{
  if (algo->return_type)
    printf("%s ", algo_to_c_type(algo->return_type));
  else
    printf("void ");
  printf("%s(", algo->ident);
  print_param_decl(algo->declarations->param_decl);
  printf(")\n{\n");
  print_const_decl(algo->declarations->const_decls, INDENT_WIDTH);
  print_var_decl(algo->declarations->var_decl, INDENT_WIDTH);
  print_instructions(algo->instructions, INDENT_WIDTH);
  printf("}\n");
}

void free_val(struct val *val)
{
  switch (val->valtype)
  {
    case stringtype:
      free(val->val.stringval);
      break;
    default:
      break;
  }
  free(val);
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
  for (unsigned i = 0; i < param_decl->local_param.size; ++i)
    free_single_var_decl(param_decl->local_param.data[i]);
  list_free(param_decl->local_param);
  for (unsigned i = 0; i < param_decl->global_param.size; ++i)
    free_single_var_decl(param_decl->global_param.data[i]);
  list_free(param_decl->global_param);
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

void free_const_decl(constdecllist_t const_decls)
{
  for (unsigned i = 0; i < const_decls.size; ++i)
  {
    free(const_decls.data[i]->type);
    free(const_decls.data[i]->ident);
    free_val(const_decls.data[i]->val);
    free(const_decls.data[i]);
  }
  list_free(const_decls);
}

void free_decls(struct declarations *declarations)
{
  free_param_decl(declarations->param_decl);
  free_const_decl(declarations->const_decls);
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

void print_arg(struct arg *arg)
{
    if (arg->global)
    {
      printf("&(");
      print_expression(arg->e);
      printf(")");
    }
    else
      print_expression(arg->e);
}

void print_arglist(arglist_t l)
{
  unsigned i = 0;
  for (; i + 1 < l.size; ++i)
  {
    print_arg(l.data[i]);
    printf(", ");
  }
  if (l.size > 0)
    print_arg(l.data[i]);
}

void print_exprlist(exprlist_t l)
{
  unsigned i = 0;
  for (; i + 1 < l.size; ++i)
  {
    print_expression(l.data[i]);
    printf(", ");
  }
  if (l.size > 0)
    print_expression(list_nth(l, i));
}

void print_assignment(struct assignment *a)
{
  print_expression(a->e1);
  printf(" = ");
  print_expression(a->e2);
}

void print_case(struct caseblock *c, int indent)
{
  for(size_t i = 0; i<c->exprlist.size;i++)
  {
    print_indent(indent);
    printf("case ");
    print_expression(c->exprlist.data[i]);
    printf(":\n");
  }
  print_instructions(c->instructions, indent + INDENT_WIDTH);
  print_indent(indent + INDENT_WIDTH);
  printf("break;\n");
}

void print_caselist(caseblocklist_t caselist, int indent)
{
  unsigned i = 0;
  for (; i + 1 < caselist.size; ++i)
    print_case(list_nth(caselist, i), indent);
  if (caselist.size > 0)
    print_case(list_nth(caselist, i), indent);
}

void print_funcall(struct funcall *f, int indent)
{
  print_indent(indent);
  if (strcmp(f->fun_ident, "ecrire") == 0)
    print_write_fun(f);
  else
  {
    printf("%s(", f->fun_ident);
    print_arglist(f->args);
    printf(");\n");
  }
}

void print_instruction(struct instruction *i, int indent)
{
  switch (i->kind)
  {
    case assignment:
      print_indent(indent);
      print_assignment(i->instr.assignment);
      printf(";\n");
      break;
    case whiledo:
      print_indent(indent);
      printf("while (");
      print_expression(i->instr.whiledo->cond);
      printf(")\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.whiledo->instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      break;
    case dowhile:
      print_indent(indent);
      printf("do\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.dowhile->instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("} while (");
      print_expression(i->instr.dowhile->cond);
      printf(");\n");
      break;
    case forloop:
      print_indent(indent);
      printf("for (");
      print_assignment(i->instr.forloop->assignment);
      printf("; ");
      print_expression(i->instr.forloop->assignment->e1);
      if (i->instr.forloop->decreasing)
        printf(" >= ");
      else
        printf(" <= ");
      print_expression(i->instr.forloop->upto);
      if (i->instr.forloop->decreasing)
        printf("; --(");
      else
        printf("; ++(");
      print_expression(i->instr.forloop->assignment->e1);
      printf("))\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.forloop->instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      break;
    case funcall:
      print_funcall(i->instr.funcall, indent);
      break;
    case ifthenelse:
      print_indent(indent);
      printf("if (");
      print_expression(i->instr.ifthenelse->cond);
      printf(")\n");
      print_indent(indent);
      printf("{\n");
      print_instructions(i->instr.ifthenelse->instructions, indent + INDENT_WIDTH);
      print_indent(indent);
      printf("}\n");
      if (i->instr.ifthenelse->elseblock.size > 0)
      {
        print_indent(indent);
        printf("else\n");
        print_indent(indent);
        printf("{\n");
        print_instructions(i->instr.ifthenelse->elseblock, indent + INDENT_WIDTH);
        print_indent(indent);
        printf("}\n");
      }
      break;
    case returnstmt:
      print_indent(indent);
      printf("return ");
      if (i->instr.returnstmt->expr)
        print_expression(i->instr.returnstmt->expr);
      printf(";\n");
      break;
    default:
      printf("instruction not handled yet\n");
    case switchcase:
      print_indent(indent);
      printf("switch (");
      print_expression(i->instr.switchcase->cond);
      printf(")\n");
      print_indent(indent);
      printf("{\n");
      print_caselist(i->instr.switchcase->caseblocklist, indent+ INDENT_WIDTH);
      if(i->instr.switchcase->otherwiseblock.size > 0)
      {
        print_indent(indent + INDENT_WIDTH);
        printf("default:\n");
        print_instructions(i->instr.switchcase->otherwiseblock, indent + 2 * INDENT_WIDTH);
        print_indent(indent + 2 * INDENT_WIDTH);
        printf("break;\n");
      }
      print_indent(indent);
      printf("}\n");
      break;
  }
}

void free_instructions(instructionlist_t instructions)
{
  for (unsigned i = 0; i < instructions.size; ++i)
    if (list_nth(instructions, i))
      free_instruction(list_nth(instructions, i));
  list_free(instructions);
}

void free_caseblock(struct caseblock *caseblock)
{
  free_expressions(caseblock->exprlist);
  free_instructions(caseblock->instructions);
  free(caseblock);
}

void free_caseblocklist(caseblocklist_t caseblocklist)
{
  for (unsigned i = 0; i < caseblocklist.size; ++i)
    free_caseblock(caseblocklist.data[i]);
  list_free(caseblocklist);
}

void free_args(arglist_t args)
{
  for (unsigned i = 0; i < args.size; ++i)
  {
    free_expression(args.data[i]->e);
    free(args.data[i]);
  }
  list_free(args);
}

void free_instruction(struct instruction *i)
{
  switch (i->kind)
  {
    case assignment:
      free_expression(i->instr.assignment->e1);
      free_expression(i->instr.assignment->e2);
      free(i->instr.assignment);
      break;
    case switchcase:
      free_caseblocklist(i->instr.switchcase->caseblocklist);
      free_instructions(i->instr.switchcase->otherwiseblock);
      free_expression(i->instr.switchcase->cond);
      free(i->instr.switchcase);
      break;
    case dowhile:
      free_expression(i->instr.dowhile->cond);
      free_instructions(i->instr.dowhile->instructions);
      free(i->instr.dowhile);
      break;
    case whiledo:
      free_expression(i->instr.whiledo->cond);
      free_instructions(i->instr.whiledo->instructions);
      free(i->instr.whiledo);
      break;
    case forloop:
      free_expression(i->instr.forloop->assignment->e1);
      free_expression(i->instr.forloop->assignment->e2);
      free(i->instr.forloop->assignment);
      free_instructions(i->instr.forloop->instructions);
      free_expression(i->instr.forloop->upto);
      free(i->instr.forloop);
      break;
    case funcall:
      free_args(i->instr.funcall->args);
      free(i->instr.funcall->fun_ident);
      free(i->instr.funcall);
      break;
    case ifthenelse:
      free_expression(i->instr.ifthenelse->cond);
      free_instructions(i->instr.ifthenelse->instructions);
      free_instructions(i->instr.ifthenelse->elseblock);
      free(i->instr.ifthenelse);
      break;
    case returnstmt:
      free_expression(i->instr.returnstmt->expr);
      free(i->instr.returnstmt);
      break;
  }
  free(i);
}

void print_expression(struct expr *e)
{
  switch (e->exprtype)
  {
    case valtype:
      print_val(e->val.val);
      break;
    case binopexprtype:
      printf("(");
      print_expression(e->val.binopexpr.e1);
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
        printf(" - 1]");
      }
      break;
    case identtype:
      if (e->argref)
        printf("*(%s)", e->val.ident);
      else
        printf("%s", e->val.ident);
      break;
    case dereftype:
      printf("*");
      print_expression(e->val.deref.e);
      break;
    case funcalltype:
      printf("%s(", e->val.funcall.fun_ident);
      print_arglist(e->val.funcall.args);
      printf(")");
      break;
    case structelttype:
      printf("(");
      print_expression(e->val.structelt.record);
      printf(").%s", e->val.structelt.field);
      break;
    default:
      printf("expr not handled yet in print_expression\n");
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
    case valtype:
      free_val(e->val.val);
      break;
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
    case dereftype:
      free_expression(e->val.deref.e);
      break;
    case funcalltype:
      free_args(e->val.funcall.args);
      free(e->val.funcall.fun_ident);
      break;
    case structelttype:
      free_expression(e->val.structelt.record);
      free(e->val.structelt.field);
    default:
      break;
  }
  if (e->type)
    free(e->type);
  free(e);
}

void free_prog(struct prog *prog)
{
  free_var_decl(prog->entry_point->var_decl);
  free_type_decls(prog->entry_point->type_decls);
  free_instructions(prog->entry_point->instructions);
  free(prog->entry_point);
  for (unsigned i = 0; i < prog->algos.size; ++i)
    free_algo(prog->algos.data[i]);
  list_free(prog->algos);
  free(prog);
}
