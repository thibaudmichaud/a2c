#include <stdio.h>
#include "grammar.h"
#include "codegen.h"
#include "typecheck.h"
#include <err.h>
//top level doesn't exist so name is always the algo name

struct context {
  char* name;
  struct symbole_table table;
};

struct context* c = NULL;

/*bool equals_expr(struct expr *e1, struct expr *e2){
    return get_expr_type(e1) == get_expr_type(e2);
}

bool check_args(struct funcall* f){
  struct expr* argf = f->args;
  struct symbole* fun = malloc(sizeof(struct symbole));
  //fun = get_ident(f->fun_ident);
  int i = 0;
  //asked if this is correct
  for(;argf != NULL; argf++){
      if(!equals_expr(fun->exprlist, argf)){
        return false;
      }
      i++;
  }
return true;

}*/

bool check_inst(struct instruction *i)
{
  switch(i->kind)
  {
    case funcall:
        //exist and args are good typed
        //return get_ident((*i->instr.funcall).fun_ident) && check_args((i->instr.funcall));
      break;

    case assignment:

        if(check_expr(i->instr.assignment.e2)){
          return true;
        }
        else
        {
          printf("In instruction : ");
          print_instruction(i,0);
        }
      break;

    case ifthenelse:
      break;

    case switchcase:
      break;

    case dowhile:
      break;

    case whiledo:
      break;

    case forloop:
      break;

    case returnstmt:
       //return *get_ident(c->name)->type == get_expr_type(i->instr.returnstmt->expr);
      break;
  }
  return false;

}

bool check_prog(struct prog* prog)
{
  return check_algo(prog->algo);
}

bool check_algo(struct algo* al){
  for(unsigned i = 0; i < al->instructions.size; i++){
    if(!check_inst(list_nth(al->instructions, i)) && list_nth(al->instructions, i)->kind == assignment){
      return false;

    }
  }
  return true;
}

bool check_expr(struct expr *e)
{
  switch(e->exprtype){

    case identtype:
      //TODO implements element_symbole
      // struct elt_symbole ident = getident(e->ident);
      break;

    case funcalltype:
      //TODO get your symbole if he existe awesome we can check it else GTFO
      /*struct elt_symbole fun = getident(e->fun_ident);
      if ( fun != NULL)
      {
        //TODO
        check_args(fun, expr->val.args);
      } else {
        return "this is not what I expected"
      }*/
      break;

    case binopexprtype:
        get_expr_type(e);
        return get_expr_type(e->val.binopexpr.e1) == get_expr_type(e->val.binopexpr.e2);
      break;

    case unopexprtype:
        return get_expr_type(e->val.unopexpr.e);
      break;

    case arrayexprtype:
      break;

    case structelttype:
      break;

    case dereftype:
      break;

    default:
      break;
  }
  return true;

}

type get_expr_type(struct expr *e){
  switch(e->exprtype){

    case nulltype:
      return t_NUL;
      break;

    case chartype:
      return t_CHAR;
      break;

    case stringtype:
      return t_STR;
      break;

    case booltype:
      return t_BOOL;
      break;

    case inttype:
      return t_INT;
      break;

    case realtype:
      return t_REAL;
      break;

    case identtype:
      break;

    case funcalltype :
      break;

    case binopexprtype:
        if (   get_expr_type(e->val.binopexpr.e1) == t_INT
            || get_expr_type(e->val.binopexpr.e1) == t_REAL
            || get_expr_type(e->val.binopexpr.e1) == t_BOOL)
        {
          if (get_expr_type(e->val.binopexpr.e2) == get_expr_type(e->val.binopexpr.e1) )
          {
            return get_expr_type(e->val.binopexpr.e1);
          }
          else
          {
            printf("expression left was of type : ");
            printf("%s ", expr_type(e->val.binopexpr.e1));
            printf("and expression right was of type : ");
            printf("%s\n", expr_type(e->val.binopexpr.e2));
          }
        }
        return t_error;
      break;

    case unopexprtype:
        if(    get_expr_type(e->val.unopexpr.e) == t_INT
            || get_expr_type(e->val.unopexpr.e) == t_REAL
            || get_expr_type(e->val.unopexpr.e) == t_BOOL)
          return get_expr_type(e->val.unopexpr.e);

        return t_error;
      break;

    case arrayexprtype:
      //something strange is happening here
      break;

    case structelttype:
      break;

    case dereftype:
      break;
  }
  return t_STR;
}
char* expr_type (struct expr* e)
{
  switch(e->exprtype)
  {

    case nulltype:
      return "null";
      break;

    case chartype:
      return "char";
      break;

    case stringtype:
      return "string";
      break;

    case booltype:
      return "bool";
      break;

    case inttype:
      return "int";
      break;

    case realtype:
      return "real";
      break;

    case identtype:
      break;

    case funcalltype :
      break;

    case binopexprtype:
      break;

    case unopexprtype:
      break;

    case arrayexprtype:
      break;

    case structelttype:
      break;

    case dereftype:
      break;

    default:
      break;
  }
  return "not yet";
}
