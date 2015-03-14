#ifndef _GRAMMAR_H
# define _GRAMMAR_H

#include <stdlib.h>
#include <stdbool.h>
#include "data_struct/list/list.h"
#include <stdio.h>

/*-------------*/
/* expressions */
/*-------------*/

typedef list_tpl(char *) identlist_t;
typedef list_tpl(struct expr *) exprlist_t;
typedef list_tpl(struct caseblock *) caselist_t;
typedef list_tpl(struct instruction *) instructionlist_t;
typedef list_tpl(struct single_var_decl *) vardecllist_t;

struct prog
{
  struct algo *algo;
  struct entry_point *entry_point;
};

struct entry_point
{
  vardecllist_t var_decl;
  instructionlist_t instructions;
};

struct binopexpr
{
  struct expr *e1;
  int op; // should use tokens defined in parser.h
  struct expr *e2;
};


struct arrayexpr
{
  struct expr *e1;
  exprlist_t indices;
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
  exprlist_t args;
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

struct ifthenelse
{
  struct expr *cond;
  instructionlist_t instructions;
  instructionlist_t elseblock;
};

struct assignment
{
  // e1 <- e2
  struct expr *e1;
  struct expr *e2;
};

struct caseblock
{
  exprlist_t exprlist;
  struct instruction *instructions;
};

struct switchcase
{
  struct expr *cond;
  caselist_t caselist;
};

struct dowhile
{
  instructionlist_t instructions;
  struct expr *cond;
};

struct whiledo
{
  struct expr *cond;
  instructionlist_t instructions;
};

struct forloop
{
  struct assignment *assignment;
  struct expr *upto;
  bool decreasing;
  instructionlist_t instructions;
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

struct struct_def
{
  char *ident;
  vardecllist_t var_decl;
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
  identlist_t var_idents;
};

struct global_param
{
  vardecllist_t var_decl;
};

struct local_param
{
  vardecllist_t var_decl;
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
  vardecllist_t var_decl;
};

struct algo
{
  char *ident;
  char *return_type;
  struct declarations *declarations;
  instructionlist_t instructions;
};

/*------*/
/* Misc */
/*------*/

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
struct expr *realexpr(double i)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = realtype;
  e->val.realval = i;
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
struct expr *funcallexpr(char *ident, exprlist_t e1)
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
struct expr *arrayexpr(struct expr *e1, exprlist_t indices)
{
  struct expr *e = malloc(sizeof(struct expr));
  e->exprtype = arrayexprtype;
  e->val.arrayexpr.e1 = e1;
  e->val.arrayexpr.indices = indices;
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
struct algo *algo(char *ident, char *return_type, struct declarations *declarations,
    instructionlist_t instructions)
{
  struct algo *a = malloc(sizeof(struct algo));
  a->ident = ident;
  a->return_type = return_type;
  a->instructions = instructions;
  a->declarations = declarations;
  return a;
}

static inline
exprlist_t empty_exprlist(void)
{
  exprlist_t l;
  list_init(l);
  return l;
}

static inline
struct instruction *whileblock(struct expr *cond, instructionlist_t b)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = whiledo;
  i->instr.whiledo.cond = cond;
  i->instr.whiledo.instructions = b;
  return i;
}

static inline
struct instruction *dowhileblock(instructionlist_t b, struct expr *cond)
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
identlist_t empty_identlist(void)
{
  identlist_t l;
  list_init(l);
  return l;
}

static inline
struct single_var_decl *single_var_decl(char *typeid, identlist_t idents)
{
  struct single_var_decl *s = malloc(sizeof(struct single_var_decl));
  s->type_ident = typeid;
  s->var_idents = idents;
  return s;
}

static inline
vardecllist_t empty_var_decl(void)
{
  vardecllist_t v;
  list_init(v);
  return v;
}

static inline
struct instruction *forblock(struct assignment *assignment, struct expr *upto, bool decreasing,
    instructionlist_t instructions)
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
struct entry_point *make_entry_point(vardecllist_t var_decl, instructionlist_t instructions)
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

static inline
struct instruction *funcallinstr(char *ident, exprlist_t args)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = funcall;
  i->instr.funcall.fun_ident = ident;
  i->instr.funcall.args = args;
  return i;
}

static inline
struct instruction *ifthenelseblock(struct expr *cond, instructionlist_t instructions, instructionlist_t elseblock)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = ifthenelse;
  i->instr.ifthenelse.cond = cond;
  i->instr.ifthenelse.instructions = instructions;
  i->instr.ifthenelse.elseblock = elseblock;
  return i;
}

static inline
struct declarations *make_declarations(struct param_decl *param_decl,
    struct const_decl *const_decl,
    struct type_decl *type_decl,
    vardecllist_t var_decl)
{
  struct declarations *d = malloc(sizeof(struct declarations));
  d->param_decl = param_decl;
  d->const_decl = const_decl;
  d->type = type_decl;
  d->var_decl = var_decl;
  return d;
}

static inline
instructionlist_t empty_instructionlist(void)
{
  instructionlist_t i;
  list_init(i);
  return i;
}

static inline
struct instruction *return_stmt(struct expr *e)
{
  struct instruction *i = malloc(sizeof(struct instruction));
  i->kind = returnstmt;
  i->instr.returnstmt.expr = e;
  return i;
}

#endif
