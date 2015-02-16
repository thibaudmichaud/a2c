#include <stdio.h>
#include "grammar.h"
#include "typecheck.h"
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
        return check_expr(i->instr.assignment.e2);
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

bool check_algo(struct algo* al){
  for(unsigned i = 0; i < al->instructions.list.size; ++i){
    if(!check_inst(list_nth(al->instructions.list, i))){
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
        return get_expr_type(e->val.binopexpr.e1) == get_expr_type(e->val.binopexpr.e2);
      break;
    
    case unopexprtype:
        return get_expr_type(e->val.unopexpr.e);
      break;
    
    case arrayexprtype:
        return false;
      break;
    
    case structelttype:
        return false;
      break;
    
    case dereftype:
        return false;
      break;
    
    default:
      break;
  }
  return false;

}

types get_expr_type(struct expr *e){

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
    
    case inttype:
      return t_INT;
      break;
    
    case realtype:
      return t_REAL;
      break;
    
    case identtype:
        //return get_ident(e->val.ident) != NULL ? *get_ident(e->val.ident)->type : t_error;
      break;
    
    case funcalltype :
       // return get_ident(e->val.funcallval.fun_ident) != NULL ? 
         // *get_ident(e->val.funcallval.fun_ident)->type : t_error;
      break;
    
    case binopexprtype:
        printf("binop spot");
        if (get_expr_type(e->val.binopexpr.e1) == t_INT || get_expr_type(e->val.binopexpr.e1) == t_REAL)
          return get_expr_type(e->val.binopexpr.e2) == get_expr_type(e->val.binopexpr.e1) ? 
                  get_expr_type(e->val.binopexpr.e1) : t_error;
        
        return t_error;
        //error checker
      break;
   
    case unopexprtype:
        if(get_expr_type(e->val.unopexpr.e) == t_INT || get_expr_type(e->val.unopexpr.e) == t_REAL)
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
  return t_INT;
}
