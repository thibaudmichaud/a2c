#ifndef _GRAMMAR_H
# define _GRAMMAR_H

#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "data_struct/list/list.h"
#include <stdio.h>

/*-------------*/
/* expressions */
/*-------------*/

struct prog
{
  struct algo *algo;
  struct entry_point *entry_point;
};

struct entry_point
{
  struct var_decl *var_decl;
  struct block *instructions;
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

struct funcall
{
  char *fun_ident;
  struct exprlist *args;
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
    booltype,
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
    bool boolval;
    struct funcall funcall;
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
  struct expr *upto;
  bool decreasing;
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
  list_tpl(struct single_var_decl *) decls;
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
  struct identlist *var_idents;
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
  struct param_decl *param_decl;
  struct const_decl *const_decl;
  struct type_decl *type;
  struct var_decl *var_decl;
};

struct algo
{
  char *ident;
  struct declarations *declarations;
  struct block *instructions;
};

/*------*/
/* Misc */
/*------*/

struct identlist
{
  list_tpl(char *) list;
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
struct expr *stringexpr(char *str)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = stringtype;
  e->val.stringval = str;
  return e;
}

static inline
struct expr *funcallexpr(char *ident, struct exprlist *e1)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = funcalltype;
  e->val.funcall.fun_ident = ident;
  e->val.funcall.args = e1;
  return e;
}

static inline
struct expr *derefexpr(struct expr *e1)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = dereftype;
  e->val.deref.e = e1;
  return e;
}

static inline
struct expr *arrayexpr(struct expr *e1, struct expr *e2)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = arrayexprtype;
  e->val.arrayexpr.e1 = e1;
  e->val.arrayexpr.e2 = e2;
  return e;
}

static inline
struct assignment *assign(struct expr *e1, struct expr *e2)
{
  struct assignment *a = malloc(sizeof(struct assignment));
  a->e1 = e1;
  a->e2 = e2;
  return a;
}

static inline
struct algo *algo(char *ident, struct declarations *declarations, struct block *instructions)
{
  struct algo *a = malloc(sizeof(struct algo));
  a->ident = ident;
  a->instructions = instructions;
  a->declarations = declarations;
  return a;
}

static inline
struct exprlist *empty_exprlist(void)
{
  struct exprlist *e = malloc(sizeof(struct exprlist));
  list_init(e->list);
  return e;
}

static inline
struct instruction *whileblock(struct expr *cond, struct block *b)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = whiledo;
  i->instr.whiledo.cond = cond;
  i->instr.whiledo.instructions = b;
  return i;
}

static inline
struct instruction *dowhileblock(struct block *b, struct expr *cond)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = dowhile;
  i->instr.dowhile.cond = cond;
  i->instr.dowhile.instructions = b;
  return i;
}

static inline
struct expr *boolexpr(bool b)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = booltype;
  e->val.boolval = b;
  return e;
}

static inline
struct identlist *empty_identlist(void)
{
  struct identlist *i = malloc(sizeof(struct identlist));
  list_init(i->list);
  return i;
}

static inline
struct single_var_decl *single_var_decl(char *typeid, struct identlist *idents)
{
  struct single_var_decl *s = malloc(sizeof(struct single_var_decl));
  s->type_ident = typeid;
  s->var_idents = idents;
  return s;
}

static inline
struct var_decl *empty_var_decl(void)
{
  struct var_decl *v = malloc(sizeof(struct var_decl));
  list_init(v->decls);
  return v;
}

static inline
struct instruction *forblock(struct assignment *assignment, struct expr *upto, bool decreasing,
    struct block *instructions)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = forloop;
  i->instr.forloop.assignment = malloc(sizeof(struct assignment));
  i->instr.forloop.assignment = assignment;
  i->instr.forloop.upto = upto;
  i->instr.forloop.instructions = instructions;
  i->instr.forloop.decreasing = decreasing;
  return i;
}

static inline
struct instruction *assigninstr(struct assignment *a)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = assignment;
  i->instr.assignment = *a;
  free(a);
  return i;
}

static inline
struct entry_point *make_entry_point(struct var_decl *var_decl, struct block *instructions)
{
  struct entry_point *e = malloc(sizeof(struct entry_point));
  e->var_decl = var_decl;
  e->instructions = instructions;
  return e;
}

static inline
struct prog *make_prog(struct algo *algo, struct entry_point *entry_point)
{
  struct prog *p = malloc(sizeof(struct prog));
  p->algo = algo;
  p->entry_point = entry_point;
  return p;
}

#endif
