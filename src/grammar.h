#ifndef _GRAMMAR_H
# define _GRAMMAR_H

#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"

/*--------------*/
/* instructions */
/*--------------*/

struct funcall
{
  char *fun_ident;
  struct expr *args;
};

struct elseblock
{
  struct instruction *instructions;
};

struct ifthenelse
{
  struct expr *cond;
  struct instruction *instructions;
  struct elseblock *elseblock;
};

struct assignment
{
  char *ident;
  struct expr *expression;
};

struct caseblock
{
  struct expr *exprlist;
  struct instruction *instructions;
};

struct switchcase
{
  struct expr *cond;
  struct caseblock *caselist;
};

struct dowhile
{
  struct instruction *instructions;
  struct expr *cond;
};

struct whiledo
{
  struct expr *cond;
  struct instruction *instructions;
};

struct forloop
{
  struct assignment *assignment;
  struct expr *cond;
  bool decroissant;
  struct instruction *instructions;
};

struct returnstmt
{
  struct expr *expr;
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
    struct funcall *funcall;
    struct assignment *assignment;
    struct ifthenelse *ifthenelse;
    struct switchcase *switchcase;
    struct dowhile *dowhile;
    struct whiledo *whiledo;
    struct forloop *forloop;
    struct returnstmt *returnstmt;
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
    struct funcall funcallval;
    struct binopexpr binopexpr;
    struct unopexpr unopexpr;
    struct arrayexpr arrayexpr;
    struct structelt structelt;
    struct deref deref;
  } val;
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

#endif
