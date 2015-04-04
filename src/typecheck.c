#include "typecheck.h"
//top level doesn't exist so name is always the algo name

funtable functions;
var_table_t variables;
type_table_t types;

/*bool equals_expr(struct expr *e1, struct expr *e2){
    return get_expr_type(e1) == get_expr_type(e2);
}
*/
bool check_args(struct function* f, struct funcall call){
    if(f->arg.size != call.args.size)
        return false;
    for(unsigned i = 0; i < call.args.size; ++i)
    {
        if(get_expr_type((struct expr *)list_nth(call.args,i)) != (unsigned)(list_nth(f->arg,i))->type)
            return false;
    }
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

bool check_inst(struct instruction *i, struct function* f)
{
  switch(i->kind)
  {
    case funcall:
        {
            struct function* f = malloc(sizeof(struct function));
            f->ident = i->instr.funcall->fun_ident;
            if (get_function(functions, f->ident) != NULL)
            {
                free(f);
                return true;
            }
            else
            {
                printf("implicit declaration of function %s",f->ident);
                free(f);
                return false;
            }
        }
      break;

    case assignment:
        if(check_expr(i->instr.assignment->e1) && check_expr(i->instr.assignment->e2)){
            return get_expr_type(i->instr.assignment->e1) == get_expr_type(i->instr.assignment->e2); 
        }
        else
        {
            return false;
        }
      break;

    case ifthenelse:
      {
        struct ifthenelse* e = i->instr.ifthenelse;
        
        if(get_expr_type(e->cond) == bool_t){
          for(unsigned int i =0; i < e->instructions.size; ++i)
          {
            if (!check_inst(list_nth(e->instructions, i), f))
            {
              printf("error in if statement");
              return false;
            }
          }
          for(unsigned int i =0; i < e->elseblock.size; ++i)
          {
            if(!check_inst(list_nth(e->elseblock,i), f))
            {
              printf("error in else statement");
              return false;
            }
          }

          return true;
        }
        
        printf("error in condition statement");
        
        return false;
      }
      break;

    case switchcase:
      //TODO: implement switchcase
      break;

    case dowhile:
      {
        struct dowhile* e = i->instr.dowhile;
        if(get_expr_type(e->cond) == bool_t)
        {
          for(unsigned int i = 0; i < e->instructions.size; ++i)
          {
            if(!check_inst(list_nth(e->instructions,i), f))
            {
              printf("error in instruction switch");
              return false;
            }
          }
          return true;
        }
        printf("error in condition of switch");
        return false;
      } 
      break;

    case whiledo:
      {
        struct whiledo* e = i->instr.whiledo;
        if(get_expr_type(e->cond) == bool_t)
        {
          for(unsigned int i = 0; i < e->instructions.size; ++i)
          {
            if(!check_inst(list_nth(e->instructions,i), f))
            {
              printf("error in instruction switch");
              return false;
            }
          }
          return true;
        }
        printf("error in condition of switch");
        return false;
      } 
      break;

    case forloop:
      {
        struct forloop* e = i->instr.forloop;
        if(check_expr((struct expr *)e->assignment))
        {
          if(get_expr_type(e->upto) == bool_t)
          {
            for(unsigned int i = 0; i < e->instructions.size; ++i)
            {
              if(!check_inst(list_nth(e->instructions, i), f))
              {
                printf("error in for loop");
                return false;
              }
            }
            return true;
          }
          printf("error in up to");
          return false;
        }
        printf("error in assignement");
        return false;
      }
      break;
    
    case returnstmt:
      return (unsigned)get_expr_type(i->instr.returnstmt->expr) == (unsigned)f->ret;
      break;
  }
  return false;

}

bool check_prog(struct prog* prog)
{
  return check_algo(prog->algo);
}

bool check_algo(struct algo* al){
    struct function* f = malloc(sizeof(struct function));
    for(unsigned i = 0; i < al->instructions.size; i++){
        if(!check_inst(list_nth(al->instructions, i), f) && list_nth(al->instructions, i)->kind == assignment){
            return false;
        }
    }
    return true;
}

bool check_expr(struct expr *e)
{
  switch(e->exprtype)
  {
    case identtype:
        return(find_var(variables, e->val.ident) != NULL);
      break;

    case funcalltype:
        {
            struct function* f = malloc(sizeof(struct function));
            f->ident = e->val.funcall.fun_ident;
            return (find_function(functions,f->ident));
        }
      break;

    case binopexprtype:
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
      return(check_expr(e->val.deref.e));
      break;

    default:
      break;
  }
  return true;

}

type get_expr_type(struct expr *e){
  switch(e->exprtype){

    case nulltype:
      return nul_t;
      break;

    case chartype:
      return char_t;
      break;

    case stringtype:
      return str_t;
      break;

    case booltype:
      return bool_t;
      break;

    case inttype:
      return int_t;
      break;

    case realtype:
      return real_t;
      break;

    case identtype:
      break;

    case funcalltype :
      {
        struct function* f = malloc(sizeof(struct function));
        if((f = get_function(functions, e->val.funcall.fun_ident)) != NULL 
                  && check_args(f, e->val.funcall))
        { 
              return f->ret;
        }
      }
      break;

    case binopexprtype:
        if (   get_expr_type(e->val.binopexpr.e1) == int_t
            || get_expr_type(e->val.binopexpr.e1) == real_t
            || get_expr_type(e->val.binopexpr.e1) == bool_t) 
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
      break;

    case unopexprtype:
        if(    get_expr_type(e->val.unopexpr.e) == int_t 
            || get_expr_type(e->val.unopexpr.e) == real_t
            || get_expr_type(e->val.unopexpr.e) == bool_t)
          return get_expr_type(e->val.unopexpr.e);
        
      break;

    case arrayexprtype:
      //something strange is happening here
      break;

    case structelttype:
      break;

    case dereftype:
      break;
  }
  return str_t;
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
