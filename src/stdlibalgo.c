#include "typecheck.h"

void fill_std_types(struct symtable *syms)
{
  struct type *t_bool = malloc(sizeof(struct type));
  t_bool->type_kind = primary_t;
  t_bool->name = strdup("booleen");
  t_bool->type_val.primary = bool_t;
  add_type(syms->types, t_bool);

  struct type *t_int = malloc(sizeof(struct type));
  t_int->type_kind = primary_t;
  t_int->name = strdup("entier");
  t_int->type_val.primary = int_t;
  add_type(syms->types, t_int);

  struct type *t_str = malloc(sizeof(struct type));
  t_str->type_kind = primary_t;
  t_str->name = strdup("chaine");
  t_str->type_val.primary = str_t;
  add_type(syms->types, t_str);

  struct type *t_reel = malloc(sizeof(struct type));
  t_reel->type_kind = primary_t;
  t_reel->name = strdup("reel");
  t_reel->type_val.primary = real_t;
  add_type(syms->types,t_reel);

  struct type *t_char = malloc(sizeof(struct type));
  t_char->type_kind = primary_t;
  t_char->name = strdup("caractere");
  t_char->type_val.primary = char_t;
  add_type(syms->types, t_char);

  struct type *t = malloc(sizeof(struct type));
  t->name = strdup("t_pile");
  t->type_kind = records_t;
  t->type_val.records_type = malloc(sizeof(struct records));
  list_init(t->type_val.records_type->fields);
  struct field *f = malloc(sizeof(struct field));
  f->type = strdup("entier");
  f->ident = strdup("data");
  list_push_back(t->type_val.records_type->fields, f);
  f = malloc(sizeof(struct field));
  f->type = strdup("t_pile");
  f->ident = strdup("prev");
  list_push_back(t->type_val.records_type->fields, f);
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_vect_entiers");
  t->type_kind = array_t;
  t->type_val.array_type = malloc(sizeof(struct array));
  t->type_val.array_type->type = find_type(syms->types, "entier");
  list_init(t->type_val.array_type->dims);
  list_push_back(t->type_val.array_type->dims, 1000);
  add_type(syms->types, t);

  t = malloc(sizeof(struct type));
  t->name = strdup("t_vect_booleens");
  t->type_kind = array_t;
  t->type_val.array_type = malloc(sizeof(struct array));
  t->type_val.array_type->type = find_type(syms->types, "entier");
  list_init(t->type_val.array_type->dims);
  list_push_back(t->type_val.array_type->dims, 1000);
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
  arg->type = find_type(syms->types, "entier");
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
  f->ret = find_type(syms->types, "entier");
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);


  f = malloc(sizeof(struct function));
  f->ident = "vider";
  f->ret = find_type(syms->types, "entier");
  list_init(f->arg);
  arg = malloc(sizeof(struct argument));
  arg->type = find_type(syms->types, "t_pile");
  arg->global = true;
  list_push_back(f->arg, arg);
  add_function(syms->functions, f);
}
