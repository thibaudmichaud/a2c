#include "typecheck.h"
#include "stdlibalgo.h"

void fill_std_types(struct symtable *syms)
{
  struct type *t_bool = malloc(sizeof(struct type));
  t_bool->type_kind = primary_t;
  t_bool->name = strdup(TYPE_BOOLEAN);
  t_bool->type_val.primary = bool_t;
  add_type(syms->types, t_bool);

  struct type *t_int = malloc(sizeof(struct type));
  t_int->type_kind = primary_t;
  t_int->name = strdup(TYPE_INT);
  t_int->type_val.primary = int_t;
  add_type(syms->types, t_int);

  struct type *t_str = malloc(sizeof(struct type));
  t_str->type_kind = primary_t;
  t_str->name = strdup(TYPE_STRING);
  t_str->type_val.primary = str_t;
  add_type(syms->types, t_str);

  struct type *t_reel = malloc(sizeof(struct type));
  t_reel->type_kind = primary_t;
  t_reel->name = strdup(TYPE_REAL);
  t_reel->type_val.primary = real_t;
  add_type(syms->types,t_reel);

  struct type *t_char = malloc(sizeof(struct type));
  t_char->type_kind = primary_t;
  t_char->name = strdup(TYPE_CHAR);
  t_char->type_val.primary = char_t;
  add_type(syms->types, t_char);

  struct type *t = malloc(sizeof(struct type));
  t->name = strdup("t_node_pile");
  t->type_kind = records_t;
  t->type_val.records_type = malloc(sizeof(struct records));
  list_init(t->type_val.records_type->fields);
  struct field *f = malloc(sizeof(struct field));
  f->type = strdup(TYPE_INT);
  f->ident = strdup("sommet");
  list_push_back(t->type_val.records_type->fields, f);
  f = malloc(sizeof(struct field));
  f->type = strdup("t_pile");
  f->ident = strdup("suiv");
  list_push_back(t->type_val.records_type->fields, f);
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_pile");
  t->type_kind = pointer_t;
  t->type_val.pointer_type = malloc(sizeof(struct pointer));
  t->type_val.pointer_type->type = strdup("t_node_pile");
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_file");
  t->type_kind = pointer_t;
  t->type_val.pointer_type = malloc(sizeof(struct pointer));
  t->type_val.pointer_type->type = strdup("t_node_file");
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_vect_entiers");
  t->type_kind = array_t;
  t->type_val.array_type = malloc(sizeof(struct array));
  t->type_val.array_type->type = find_type(syms->types, TYPE_INT);
  list_init(t->type_val.array_type->dims);
  list_push_back(t->type_val.array_type->dims, expr_from_val(intval(1000)));
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_vect_booleens");
  t->type_kind = array_t;
  t->type_val.array_type = malloc(sizeof(struct array));
  t->type_val.array_type->type = find_type(syms->types, TYPE_INT);
  list_init(t->type_val.array_type->dims);
  list_push_back(t->type_val.array_type->dims, expr_from_val(intval(1000)));
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_noeud_bin");
  t->type_kind = records_t;
  t->type_val.records_type = malloc(sizeof(struct records));
  list_init(t->type_val.records_type->fields);
  f = malloc(sizeof(struct field));
  f->type = strdup(TYPE_INT);
  f->ident = strdup("cle");
  list_push_back(t->type_val.records_type->fields, f);
  f = malloc(sizeof(struct field));
  f->type = strdup("t_arbre_bin");
  f->ident = strdup("fg");
  list_push_back(t->type_val.records_type->fields, f);
  f = malloc(sizeof(struct field));
  f->type = strdup("t_arbre_bin");
  f->ident = strdup("fd");
  list_push_back(t->type_val.records_type->fields, f);
  add_type(syms->types, t);
  
  t = malloc(sizeof(struct type));
  t->name = strdup("t_arbre_bin");
  t->type_kind = pointer_t;
  t->type_val.pointer_type = malloc(sizeof(struct pointer));
  t->type_val.pointer_type->type = strdup("t_noeud_bin");
  add_type(syms->types, t);
}

void fill_std_fun(struct symtable *syms)
{
  struct function *f = malloc(sizeof(struct function));
  f->ident = "empiler";
  f->ret = NULL;
  list_init(f->arg);
  struct argument *arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = true;
  list_push_back(f->arg, arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, TYPE_INT);
  arg->global = false;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "pile_vide";
  f->ret = find_type(syms->types, "t_pile");
  list_init(f->arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "depiler";
  f->ret = find_type(syms->types, TYPE_INT);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "vide_pile";
  f->ret = find_type(syms->types, TYPE_INT);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "est_vide";
  f->ret = find_type(syms->types, TYPE_BOOLEAN);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = false;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "enfiler";
  f->ret = NULL;
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_file");
  arg->global = true;
  list_push_back(f->arg, arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, TYPE_INT);
  arg->global = false;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "file_vide";
  f->ret = find_type(syms->types, "t_file");
  list_init(f->arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "defiler";
  f->ret = find_type(syms->types, TYPE_INT);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_file");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "vide_file";
  f->ret = find_type(syms->types, TYPE_INT);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_file");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

  f = malloc(sizeof(struct function));
  f->ident = "est_file_vide";
  f->ret = find_type(syms->types, TYPE_BOOLEAN);
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_file");
  arg->global = false;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);

}
