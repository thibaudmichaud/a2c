#include <stdarg.h>
#include "typecheck.h"
#include "get_line.h"
#include "parser.h"
#include "lexer.h"
#include "a2c.h"
#include <string.h>
#include <assert.h>
#include "error.h"
#include <stdbool.h>

struct type *t_bool, *t_int, *t_str, *t_reel, *t_char;

bool equal_types(char *tname1, char *tname2, struct symtable *syms)
{
  struct type *t1 = find_type(syms->types, tname1);
  struct type *t2 = find_type(syms->types, tname2);
  return ((t1 && t1->type_kind == pointer_t && strcmp(tname2, "nul") == 0)
      || (t2 && t2->type_kind == pointer_t && strcmp(tname1, "nul") == 0)
      || strcmp(tname1, tname2) == 0);
}

bool equal_dims(intlist_t dim1, intlist_t dim2)
{
  if (dim1.size != dim2.size)
    return false;
  for (unsigned i = 0; i < dim1.size; ++i)
    if (dim1.data[i] != dim2.data[i])
      return false;
  return true;
}

bool equal_identlist(identlist_t idents1, identlist_t idents2)
{
  if (idents1.size != idents2.size) return false;
  for (unsigned i = 0; i < idents1.size; ++i)
    if (strcmp(idents1.data[i], idents2.data[i]) != 0)
      return false;
  return true;
}

char *algo_to_c_type(char *ident)
{
    if (strcmp(ident, "entier") == 0)
        return "int";
    else if (strcmp(ident, "caractere") == 0)
        return "char";
    else if (strcmp(ident, "chaine") == 0)
        return "char *";
    else if (strcmp(ident, "reel") == 0)
        return "double";
    else if (strcmp(ident, "booleen") == 0)
        return "int";
    // If it is none of the primary types, then it is a user-defined type
    // wich will have the same name in the c file as in the algo file.
    return ident;
}

struct symtable *empty_symtable(void)
{
    struct symtable *syms = malloc(sizeof(struct symtable));
    syms->functions = empty_fun_table();
    syms->variables = empty_var_table();
    syms->types = empty_type_table();
    return syms;
}

void free_symtable(struct symtable *syms)
{
    free_fun_table(syms->functions);
    free_var_table(syms->variables);
    free_type_table(syms->types);
    free(syms);
}

void fill_std_syms(struct symtable *syms)
{
    t_bool = malloc(sizeof(struct type));
    t_bool->type_kind = primary_t;
    t_bool->name = strdup("booleen");
    t_bool->type_val.primary = bool_t;
    add_type(syms->types, t_bool);

    t_int = malloc(sizeof(struct type));
    t_int->type_kind = primary_t;
    t_int->name = strdup("entier");
    t_int->type_val.primary = int_t;
    add_type(syms->types, t_int);

    t_str = malloc(sizeof(struct type));
    t_str->type_kind = primary_t;
    t_str->name = strdup("chaine");
    t_str->type_val.primary = str_t;
    add_type(syms->types, t_str);

    t_reel = malloc(sizeof(struct type));
    t_reel->type_kind = primary_t;
    t_reel->name = strdup("reel");
    t_reel->type_val.primary = real_t;
    add_type(syms->types,t_reel);

    t_char = malloc(sizeof(struct type));
    t_char->type_kind = primary_t;
    t_char->name = strdup("caractere");
    t_char->type_val.primary = char_t;
    add_type(syms->types, t_char);
}

args_t get_args(struct param_decl *params, struct symtable *syms)
{
  args_t args;
  list_init(args);
  for (unsigned i = 0; i < params->local_param.size; ++i)
  {
    for (unsigned j = 0; j < params->local_param.data[i]->var_idents.size; ++j)
    {
      struct argument *arg = malloc(sizeof(struct argument));
      arg->global = 0;
      arg->type = find_type(syms->types, params->local_param.data[i]->type_ident);
      list_push_back(args, arg);
    }
  }
  for (unsigned i = 0; i < params->global_param.size; ++i)
  {
    for (unsigned j = 0; j < params->global_param.data[i]->var_idents.size; ++j)
    {
      struct argument *arg = malloc(sizeof(struct argument));
      arg->global = 0;
      arg->type = find_type(syms->types, params->global_param.data[i]->type_ident);
      list_push_back(args, arg);
    }
  }
  return args;
}

bool add_variables(struct symtable *syms, vardecllist_t var_decls)
{
    bool correct = true;
    for (unsigned i = 0; i < var_decls.size; ++i)
    {
      for(unsigned j = 0; j < var_decls.data[i]->var_idents.size; ++j)
      {
        struct var_sym* sym = malloc(sizeof(struct var_sym));
        sym->ident = list_nth(var_decls.data[i]->var_idents, j);
        struct type *s = find_type(syms->types, var_decls.data[i]->type_ident);
        if (s)
        {
          sym->type = s;
          add_var(syms->variables, sym);
        }
        else
        {
          error(var_decls.data[i]->pos, "unknown type %s", var_decls.data[i]->type_ident);
          free(sym);
          correct = false;
        }
      }
    }
    return correct;
}

bool add_types(struct symtable *syms, typedecllist_t typelist)
{
  bool correct = true;
  for(unsigned i = 0; i < typelist.size; ++i)
  {
    struct type_decl* type_decl = list_nth(typelist, i);
    struct type_def*  type_def  = type_decl->type_def;
    struct type* type = malloc(sizeof(struct type));
    type->name = strdup(type_decl->ident);
    switch(type_def->type_type)
    {
      case enum_type:
        {
          type->type_kind = enum_t;
          struct enum_type* _enum = malloc(sizeof(struct enum_type));
          _enum->idents = type_def->def.enum_def->identlist;
          type->type_val.enum_type = _enum;
          add_type(syms->types, type);
        }
        break;
      case array_type:
        {
          type->type_kind = array_t;

          struct array* array = malloc(sizeof(struct array));
          if((array->type = find_type(syms->types, type_def->def.array_def->elt_type))
              == NULL)
          {
            error(type_def->pos, "Unknown type");
            correct = false;
          }
          array->dims = type_def->def.array_def->dims;
          type->type_val.array_type = array;
          add_type(syms->types, type);
        }
        break;
      case struct_type:
        {
          type->type_kind = records_t;

          struct records* record = malloc(sizeof(struct records));
          fieldlist_t fields;
          list_init(fields);

          vardecllist_t varlist = type_def->def.record_def->var_decl;

          for(unsigned int i = 0 ; i < varlist.size; ++i)
          {
            struct single_var_decl* var = list_nth(varlist, i);

            for(unsigned int j = 0; j < var->var_idents.size; ++j)
            {
              if (!find_type(syms->types, var->type_ident))
              {
                error(var->pos, "type %s doesn't exist", var->type_ident);
                correct = false;
              }
              else
              {
                struct field* field = malloc(sizeof(struct field));
                field->ident = strdup(list_nth(var->var_idents,j));
                field->type = strdup(var->type_ident);
                list_push_back(fields, field);
              }
            }
          }
          record->fields = fields;
          type->type_val.records_type = record;
          add_type(syms->types, type);
        }
        break;
      case pointer_type:
        {
          type->type_kind = pointer_type;
          type->type_val.pointer_type = malloc(sizeof(struct pointer));
          char *pointed_type = 
            type_decl->type_def->def.pointer_def->pointed_type_ident;;
          bool pointed_type_exists = false;
          if (strcmp(pointed_type, "entier") == 0
              || strcmp(pointed_type, "booleen") == 0
              || strcmp(pointed_type, "chaine") == 0
              || strcmp(pointed_type, "reel") == 0
              || strcmp(pointed_type, "caractere") == 0)
            pointed_type_exists = true;
          for(unsigned i = 0; i < typelist.size && !pointed_type_exists; ++i)
          {
            if (strcmp(typelist.data[i]->ident, pointed_type) == 0)
            {
              pointed_type_exists = true;
              break;
            }
          }
          if (pointed_type_exists)
          {
            type->type_val.pointer_type->type = strdup(pointed_type);
            add_type(syms->types, type);
          }
          else
          {
            error(type_decl->pos, "pointed type does not exist");
            correct = false;
          }
        }
        break;
    }
  }
  return correct;
}

bool check_prog(struct prog* prog)
{
    bool correct = true;
    struct symtable *syms = empty_symtable();
    fill_std_syms(syms);
    correct = correct && add_types(syms, prog->entry_point->type_decls);
    correct = correct && add_variables(syms, prog->entry_point->var_decl);
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        struct function* f = malloc(sizeof(struct function));
        f->ident = al->ident;
        f->ret = find_type(syms->types, al->return_type);
        f->arg = get_args(al->declarations->param_decl, syms);
        add_function(syms->functions, f);
    }
    for (unsigned i = 0; i < prog->entry_point->instructions.size; ++i)
      if (!check_inst(
            prog->entry_point->instructions.data[i],
            find_type(syms->types, "entier"), syms))
          correct = false;
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        if (!check_algo(al, syms))
            correct = false;
    }
    free_symtable(syms);
    return correct;
}

bool check_algo(struct algo* al, struct symtable *syms)
{
    bool correct = true;
    struct function* f = malloc(sizeof(struct function));
    f->ret = find_type(syms->types, al->return_type);
    struct declarations* decl = al->declarations;

    if(decl != NULL)
    {
        struct param_decl* p_decl = decl->param_decl;
        vardecllist_t loc = p_decl->local_param;
        vardecllist_t glo = p_decl->global_param;
        vardecllist_t vars = decl->var_decl;
        typedecllist_t typelist = decl->type_decls;
        constdecllist_t consts = decl->const_decls;
        if(loc.size > 0)
        {
          correct = correct && add_variables(syms, loc);
        }
        if(glo.size > 0)
        {
            for(unsigned int i = 0; i < glo.size; i++)
                correct = correct && add_variables(syms, glo);
        }
        correct = correct && add_types(syms, typelist);
        correct = correct && add_variables(syms, vars);
        for(unsigned i =0; i < consts.size; ++i)
        {
            struct const_decl* cons = list_nth(consts, i);
            struct var_sym* sym = malloc(sizeof(struct var_sym));
            switch(cons->val->valtype)
            {
                case nulltype:
                    sym->type = malloc(sizeof(struct pointer));
                    sym->type->type_kind = pointer_t;
                    sym->type->type_val.pointer_type->nul = true;
                    break;
                case chartype:
                    sym->type = find_type(syms->types, "caractere");
                    break;
                case stringtype:
                    sym->type = find_type(syms->types, "chaine");
                    break;
                case inttype:
                    sym->type = find_type(syms->types, "entier");
                    break;
                case realtype:
                    sym->type = find_type(syms->types, "reel");
                    break;
                case booltype:
                    sym->type = find_type(syms->types, "booleen");
                    break;


            }
            sym->ident = cons->ident;
            add_var(syms->variables, sym);
        }
    }
    for(unsigned i = 0; i < al->instructions.size; i++)
      if(!check_inst(list_nth(al->instructions, i), f->ret, syms))
        correct = false;

    free(f);
    return correct;
}

bool check_assignment(struct assignment *assignment, struct symtable *syms)
{
    char *t1 = check_expr(assignment->e1, syms);
    char *t2 = check_expr(assignment->e2, syms);
    if (!t1 || !t2)
      return false;
    if (!equal_types(t1, t2, syms))
    {
      error(assignment->pos, "incompatible types: %s and %s.", t1, t2);
      return false;
    }
    return true;
}

struct type *check_funcall(struct funcall *f, struct symtable *syms)
{
  if (strcmp(f->fun_ident, "ecrire") == 0)
  {
    for (unsigned i = 0; i < f->args.size; ++i)
    {
      if (!check_expr(f->args.data[i], syms))
        return NULL;
    }
    // Return a non-zero value to indicate success.
    // It is a procedure so this function should be called from check_inst,
    // which doesn't use the return value as a type but just as a bool.
    return (void *)1;
  }
  struct function *proto = get_function(syms->functions, f->fun_ident);
  if (!proto)
  {
    error(f->pos, "implicit declaration of function %s", f->fun_ident);
    return NULL;
  }
  else if (proto->arg.size != f->args.size)
  {
    error(f->pos, "function %s expects %d arguments but %d were given",
        f->fun_ident, proto->arg.size, f->args.size);
    return NULL;
  }
  else
  {
    bool ok = true;
    for (unsigned i = 0; i < proto->arg.size; ++i)
    {
      char *argtype = check_expr(f->args.data[i], syms);
      if (!argtype)
        return NULL;
      if (!equal_types(proto->arg.data[i]->type->name, argtype, syms))
      {
        error(f->pos,
            "wrong type for argument %d in function %s", i + 1, f->fun_ident);
        ok = false;
      }
    }
    if (ok)
      // If it is a procedure the return type is 0 which would be interpreted as
      // an error, instead 1 is returned. Since it is a procedure it means it is
      // called by check_inst (rather than check_expr) and the return value will
      // only be used as a boolean.
      return proto->ret ? proto->ret : (void *)1;
  }
  return NULL;
}

bool check_inst(struct instruction *i, struct type *ret, struct symtable *syms)
{
    switch(i->kind)
    {
        case funcall:
            return check_funcall(i->instr.funcall, syms);

        case assignment:
            return check_assignment(i->instr.assignment, syms);

        case ifthenelse:
            {
                struct ifthenelse* e = i->instr.ifthenelse;

                char *t1 = check_expr(e->cond, syms);
                if (!t1)
                  return false;
                if(strcmp(t1, "booleen") == 0)
                {
                    for(unsigned int i =0; i < e->instructions.size; ++i)
                    {
                        if (!check_inst(list_nth(e->instructions, i), ret, syms))
                        {
                            return false;
                        }
                    }
                    for(unsigned int i =0; i < e->elseblock.size; ++i)
                    {
                        if(!check_inst(list_nth(e->elseblock,i), ret, syms))
                        {
                            return false;
                        }
                    }
                    return true;
                }

                error(e->cond->pos, "condition should be a boolean");
                return false;
            }
            break;

        case switchcase:
            {
              char *t1 = check_expr(i->instr.switchcase->cond, syms);
              if(!t1)
                return false;
              for(unsigned int l = 0; l < i->instr.switchcase->caseblocklist.size; ++l)
              {
                for( unsigned int j = 0; j < i->instr.switchcase->caseblocklist.data[l]->exprlist.size; ++j)
                {
                  char *t2 =
                    check_expr(i->instr.switchcase->caseblocklist.data[l]->exprlist.data[j],
                        syms);
                  if(!t2)
                    return false;
                  if (!equal_types(t1, t2, syms))
                  {
                    error(i->instr.switchcase->cond->pos,
                        "different types between switch and case\n");
                    return false;
                  }
                }
                for(int unsigned k = 0;
                    k < i->instr.switchcase->caseblocklist.data[l]->instructions.size; ++k)
                {
                  if(!check_inst(i->instr.switchcase->caseblocklist.data[l]->instructions.data[k], ret, syms))
                    return false;
                }

              }
              for(unsigned int l = 0; l < i->instr.switchcase->otherwiseblock.size; ++l)
              {
                if(!check_inst(i->instr.switchcase->otherwiseblock.data[l], ret, syms))
                  return false;

              }
              return true;
              break;
            }

        case dowhile:
            {
                struct dowhile* e = i->instr.dowhile;
                char *t = check_expr(e->cond, syms);
                if(strcmp(t, "booleen") == 0)
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), ret, syms))
                            return false;
                    }
                    return true;
                }
                error(e->cond->pos, "condition should be a boolean");
                return false;
            }
            break;

        case whiledo:
            {
                struct whiledo* e = i->instr.whiledo;
                if(strcmp(check_expr(e->cond, syms), "booleen") == 0)
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                        if(!check_inst(list_nth(e->instructions,i), ret, syms))
                            return false;
                    return true;
                }
                error(e->cond->pos, "condition should be a boolean");
                return false;
            }
            break;

        case forloop:
            {
                struct forloop* e = i->instr.forloop;
                char *upto_type = check_expr(e->upto, syms);
                if(upto_type && check_assignment(e->assignment, syms))
                {
                    if (strcmp(upto_type, "entier") != 0)
                    {
                      error(e->upto->pos, "expected int expression");
                      return false;
                    }
                    else if (strcmp(e->assignment->e1->type, "entier") != 0)
                    {
                      error(e->assignment->e1->pos, "expected int expression");
                      return false;
                    }
                    else if (strcmp(e->assignment->e2->type, "entier") != 0)
                    {
                      error(e->assignment->e2->pos, "expected int expression");
                      return false;
                    }
                    else
                    {
                        for(unsigned int i = 0; i < e->instructions.size; ++i)
                            if(!check_inst(list_nth(e->instructions, i), ret, syms))
                                return false;
                        return true;
                    }
                }
                return false;
            }
            break;

        case returnstmt:
            {
              char *t = check_expr(i->instr.returnstmt->expr, syms);
              if (!t)
                return false;
              if(equal_types(t, ret->name, syms))
                return true;
              error(i->instr.returnstmt->expr->pos, "expected type %s, not %s", ret->name, t);
              return false;
            }
    }
    return false;

}

char *check_expr(struct expr *e, struct symtable *syms)
{
    e->type = NULL;
    switch(e->exprtype)
    {
        case valtype:
          switch(e->val.val->valtype)
          {
            case nulltype:
              e->type = strdup("nul");
              break;
            case chartype:
              e->type = strdup("caractere");
              break;
            case stringtype:
              e->type = strdup("chaine");
              break;
            case booltype:
              e->type = strdup("booleen");
              break;
            case inttype:
              e->type = strdup("entier");
              break;
            case realtype:
              e->type = strdup("reel");
              break;
          }
          break;
        case identtype:
          {
            struct var_sym *var = find_var(syms->variables, e->val.ident);
            if (var)
              e->type = strdup(var->type->name);
            else
              error(e->pos, "Use of undeclared variable %s", e->val.ident);
          }
          break;
        case funcalltype:
          {
            struct type *rettype = check_funcall(&e->val.funcall, syms);
            if (rettype)
              e->type = strdup(rettype->name);
            break;
          }
        case binopexprtype:
            {
              char *t1 = check_expr(e->val.binopexpr.e1, syms);
              char *t2 = check_expr(e->val.binopexpr.e2, syms);
              if (!t1 || !t2)
                break;
              if (equal_types(t1, t2, syms))
              {
                if(e->val.binopexpr.op == EQ 
                    || e->val.binopexpr.op == LT
                    || e->val.binopexpr.op == GT
                    || e->val.binopexpr.op == LE
                    || e->val.binopexpr.op == GE)
                {
                  e->type = strdup(t_bool->name);
                }
                else
                {
                  e->type = strdup(t1);
                }
              }
              else
                error(e->pos, "incompatible types: %s and %s.", t1, t2);
              break;
            }

        case unopexprtype:
            {
              char *t1 = check_expr(e->val.unopexpr.e, syms);
              if(t1)
                e->type = strdup(t1);
              break;
            }

        case arrayexprtype:
            {
              char *tname = check_expr(e->val.arrayexpr.e1, syms);
              struct type *t = find_type(syms->types, tname);
              if (e->val.arrayexpr.indices.size != t->type_val.array_type->dims.size)
              {
                error(e->pos, "array has %d dimensions, not %d",
                    t->type_val.array_type->dims.size,
                    e->val.arrayexpr.indices.size);
              }
              else
              {
                for (unsigned i = 0; i < e->val.arrayexpr.indices.size; ++i)
                {
                  char *tidx = check_expr(e->val.arrayexpr.indices.data[i], syms);
                  if (strcmp(tidx, "entier") != 0)
                    error(e->val.arrayexpr.indices.data[i]->pos, "index should be an integer");
                }
                e->type = strdup(t->type_val.array_type->type->name);
              }
            }
            break;

        case structelttype:
            {
              char *record = check_expr(e->val.structelt.record, syms);
              if (record)
              {
                struct type *t = find_type(syms->types, record);
                fieldlist_t fields = t->type_val.records_type->fields;
                unsigned i = 0;
                for (; i < fields.size; ++i)
                {
                  if (equal_types(fields.data[i]->ident, e->val.structelt.field, syms))
                  {
                    e->type = strdup(fields.data[i]->type);
                    break;
                  }
                }
                if (i >= fields.size)
                  error(e->val.structelt.record->pos,
                      "field %s doesn't exist in this record type",
                      e->val.structelt.field);
              }
              break;
            }

        case dereftype:
            {
              char *tname = check_expr(e->val.deref.e, syms);
              if (tname)
              {
                struct type *t = find_type(syms->types, tname);
                if (t->type_kind != pointer_t)
                  error(e->pos, "expression is not a pointer, cannot be dereferenced");
                else
                  e->type = strdup(t->type_val.pointer_type->type);
              }
              break;
            }
    }
    return e->type;
}
