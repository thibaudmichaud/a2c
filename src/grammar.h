#ifndef _GRAMMAR_H
# define _GRAMMAR_H

#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "data_struct/list/list.h"

/*-------------*/
/* expressions */
/*-------------*/

struct binopexpr
{
  struct expr *e1;
  int op; // should use tokens defined in parser.h
  struct expr *e2;
};


struct arrayexpr
{
  // e1[e2]
  struct expr *e1;
  struct expr *e2;
};

struct structelt
{
  // e1.ident
  struct expr *e1;
  char *ident;
};

struct deref
{
  struct expr *e;
};

enum unop
{
  UPLUS,
  UMINUS,
  NOT
};

struct unopexpr
{
  enum unop op;
  struct expr *e;
};

struct expr
{
  enum
  {
    nulltype,
    chartype,
    stringtype,
    inttype,
    realtype,
    identtype,
    funcalltype,
    binopexprtype,
    unopexprtype,
    arrayexprtype,
    structelttype,
    dereftype
  } exprtype;
  union
  {
    bool nullval;
    char charval;
    char *stringval;
    int intval;
    double realval;
    char *ident;
    // Anonymous struct to bypass double recursive definition of expr and
    // funcall.
    struct { char *fun_ident; struct expr *args; } funcall;
    struct binopexpr binopexpr;
    struct unopexpr unopexpr;
    struct arrayexpr arrayexpr;
    struct structelt structelt;
    struct deref deref;
  } val;
};

/*--------------*/
/* instructions */
/*--------------*/

struct exprlist
{
  list_tpl(struct expr *) list;
};

struct funcall
{
  char *fun_ident;
  struct exprlist exprlist;
};

struct elseblock
{
  struct block *instructions;
};

struct ifthenelse
{
  struct expr *cond;
  struct block *instructions;
  struct elseblock *elseblock;
};

struct assignment
{
  // e1 <- e2
  struct expr *e1;
  struct expr *e2;
};

struct caseblock
{
  struct expr *exprlist;
  struct instruction *instructions;
};

struct caselist
{
  list_tpl(struct caseblock *) caselist;
};

struct switchcase
{
  struct expr *cond;
  struct caselist caselist;
};

struct dowhile
{
  struct block *instructions;
  struct expr *cond;
};

struct whiledo
{
  struct expr *cond;
  struct block *instructions;
};

struct forloop
{
  struct assignment *assignment;
  struct expr *cond;
  bool decroissant;
  struct block *instructions;
};

struct returnstmt
{
  struct expr *expr;
};

struct block
{
  list_tpl(struct instruction *) list;
};

struct instruction
{
  enum
  {
    funcall,
    assignment,
    ifthenelse,
    switchcase,
    dowhile,
    whiledo,
    forloop,
    returnstmt
  } kind;
  union
  {
    struct funcall funcall;
    struct assignment assignment;
    struct ifthenelse ifthenelse;
    struct switchcase switchcase;
    struct dowhile dowhile;
    struct whiledo whiledo;
    struct forloop forloop;
    struct returnstmt returnstmt;
  } instr;
};

struct enum_def
{
  char **ident;
};

struct array_def
{
  char *array_ident;
  size_t *dim;
  char *type_ident;
};

struct var_decl
{
  struct single_var_decl *decls;
};

struct struct_def
{
  char *ident;
  struct var_decl var_decl;
};

struct pointer_def
{
  char *pointertype_ident;
  char *basetype_ident;
};

struct type_def
{
  enum
  {
    enum_type,
    array_type,
    struct_type,
    pointer_type
  } type_type;
  union
  {
    struct enum_def *enum_def;
    struct array_def *array_def;
    struct struct_def *struct_def;
    struct pointer_def *pointer_def;
  } def;
};

struct single_var_decl
{
  char *type_ident;
  char **var_ident;
};

struct global_param
{
  struct var_decl var_decl;
};

struct local_param
{
  struct var_decl var_decl;
};

struct param_decl
{
  struct global_param global_param;
  struct local_param local_param;
};

struct const_decl
{
};

struct type_decl
{
  char *ident;
  struct type_def type_def;
};

struct declarations
{
  struct param_decl param_decl;
  struct const_decl const_decl;
  struct type_decl type;
  struct var_decl var_decl;
};

struct algo
{
  char *ident;
  struct declarations declarations;
  struct instruction *instructions;
};

// Helper functions

static inline
struct expr *binopexpr(struct expr *e1, int op, struct expr *e2)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = binopexprtype;
  e->val.binopexpr.e1 = e1;
  e->val.binopexpr.e2 = e2;
  e->val.binopexpr.op = op;
  return e;
}

static inline
struct expr *intexpr(int i)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = inttype;
  e->val.intval = i;
  return e;
}

static inline
struct expr *realexpr(int i)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = inttype;
  e->val.intval = i;
  return e;
}

static inline
struct expr *unopexpr(int op, struct expr *e1)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = unopexprtype;
  e->val.unopexpr.op = op;
  e->val.unopexpr.e = e1;
  return e;
}

static inline
struct expr *identexpr(char *ident)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = identtype;
  e->val.ident = ident;
  return e;
}

static inline
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
      return "ou";
    case AND:
      return "et";
    case XOR:
      return "oue";
    case MOD:
      return "mod";
    default:
      return NULL;
  }
}

static inline
struct instruction *assign(struct expr *e1, struct expr *e2)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = assignment;
  i->instr.assignment.e1 = e1;
  i->instr.assignment.e2 = e2;
  return i;
}

#endif
