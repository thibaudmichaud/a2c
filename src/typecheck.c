#include "typecheck.h"
//top level doesn't exist so name is always the algo name

bool equal_types(struct type* t1, struct type* t2)
{
    if(t1->type_kind != t2->type_kind)
        return false;

    switch(t1->type_kind)
    {
        case primary_t:
            return t1->type_val.primary == t1->type_val.primary; 
            break;
        case records_t:
            if(t1->type_val.records_type->fields.size != t2->type_val.records_type->fields.size)
                return false;
            for(unsigned i = 0; i < t1->type_val.records_type->fields.size; ++i)
            {
                struct field* field1 = list_nth(t1->type_val.records_type->fields, i);
                struct field* field2 = list_nth(t2->type_val.records_type->fields, i);
                if(!equal_types(field1->t, field2->t))
                    return false;
            }
            return true;
            break;
        case tab_t:

            break;
        case pointer_t:
            break;
        case enum_t:
            break;

    }
    return false;
}

bool check_args(struct function* f, struct funcall call, 
        fun_table_t functions, var_table_t variables, type_table_t types)
{
    if(f->arg.size != call.args.size)
        return false;
    for(unsigned i = 0; i < call.args.size; ++i)
    {
        if(!equal_types(get_expr_type((struct expr *)list_nth(call.args,i), functions, variables, types) 
                    ,list_nth(f->arg,i)->type))
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

bool check_prog(struct prog* prog)
{
    fun_table_t functions = empty_fun_table();
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        struct function* f = malloc(sizeof(struct function));
        f->ident = al->ident;
        check_algo(al, functions);
    }
    return true;
}

bool check_algo(struct algo* al, fun_table_t functions)
{
    struct function* f = malloc(sizeof(struct function));
    var_table_t variables = empty_var_table();
    type_table_t types = empty_type_table();
    for(unsigned i = 0; i < al->instructions.size; i++)
    {
        /*struct declarations* decl = al->declarations;
          struct param_decl* p_decl = decl->param_decl;
          struct local_param* loc = p_decl->local_param;
          for(unsigned int i = 0; i < loc->param.size; ++i)
          {
          struct single_var_decl* var = list_nth(loc->param,i);
          for(unsigned int j = 0; j < var->var_idents.size; ++j)
          {
          struct var_sym* sym = malloc(sizeof(struct var_sym));
          sym->ident = list_nth(var->var_idents, i);
          sym->type = int_t;
          add_var(variables, sym);
          }
          }*/

        if(!check_inst(list_nth(al->instructions, i), f, functions, variables, types) 
                && list_nth(al->instructions, i)->kind == assignment)
        {
            free(f);
            free_var_table(variables);
            free_type_table(types);
            return false;
        }
    }
    free_var_table(variables);
    free_type_table(types);
    free(f);
    return true;
}

bool check_inst(struct instruction *i, struct function* f, fun_table_t functions, var_table_t variables, type_table_t types)
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
            if(check_expr(i->instr.assignment->e1, functions, variables, types) 
                    && check_expr(i->instr.assignment->e2, functions, variables, types)){

                return get_expr_type(i->instr.assignment->e1, functions, variables, types) 
                    == get_expr_type(i->instr.assignment->e2, functions, variables, types); 
            }
            else
            {
                return false;
            }
            break;

        case ifthenelse:
            {
                struct ifthenelse* e = i->instr.ifthenelse;

                if(equal_types(get_expr_type(e->cond, functions, variables, types), (struct type*) bool_t))
                {
                    for(unsigned int i =0; i < e->instructions.size; ++i)
                    {
                        if (!check_inst(list_nth(e->instructions, i), f, functions, variables, types))
                        {
                            printf("error in if statement");
                            return false;
                        }
                    }
                    for(unsigned int i =0; i < e->elseblock.size; ++i)
                    {
                        if(!check_inst(list_nth(e->elseblock,i), f, functions, variables, types))
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
                if(equal_types(get_expr_type(e->cond, functions, variables, types), (struct type*) bool_t))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), f, functions, variables, types))
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
                if(equal_types(get_expr_type(e->cond, functions, variables, types), (struct type*)bool_t))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), f, functions, variables, types))
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
                if(check_expr((struct expr *)e->assignment, functions, variables, types))
                {
                    if(equal_types(get_expr_type(e->upto, functions, variables, types), (struct type*)bool_t))
                    {
                        for(unsigned int i = 0; i < e->instructions.size; ++i)
                        {
                            if(!check_inst(list_nth(e->instructions, i), f, functions, variables, types))
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
            return equal_types(get_expr_type(i->instr.returnstmt->expr, functions, variables, types), f->ret);
            break;
    }
    return false;

}

bool check_expr(struct expr *e, fun_table_t functions, var_table_t variables, type_table_t types)
{
    switch(e->exprtype)
    {
        case identtype:
            return(find_var(variables, e->val.ident) != NULL);
            break;

        case funcalltype:
            {
                char* ident  = e->val.funcall.fun_ident;
                return get_function(functions, ident) != NULL;
            }
            break;

        case binopexprtype:
            return equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
                    get_expr_type(e->val.binopexpr.e2, functions, variables, types));
            break;

        case unopexprtype:
            return get_expr_type(e->val.unopexpr.e, functions, variables, types);
            break;

        case arrayexprtype:
            {


            }
            break;

        case structelttype:
            break;

        case dereftype:
            return(check_expr(e->val.deref.e, functions, variables, types ));
            break;

        default:
            break;
    }
    return true;

}

struct type* get_expr_type(struct expr *e, fun_table_t functions, var_table_t variables, type_table_t types)
{

    switch(e->exprtype)
    {
        case valtype:
            switch(e->val.val->valtype)
            {




        case nulltype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = nul_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
        case chartype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = char_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
        case stringtype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = str_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
        case booltype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = bool_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
        case inttype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = int_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
        case realtype:
            {
                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = real_t;
                t->type_val.primary = p;
                return t;
            
            break;
            }
            }
            case identtype:
                    break;

                    case funcalltype :
                    {
                        struct function* f = malloc(sizeof(struct function));
                        if((f = get_function(functions, e->val.funcall.fun_ident)) != NULL 
                             && check_args(f, e->val.funcall, functions, variables, types)) 
                            return f->ret;
                    }
                    break;

                    case binopexprtype:
                    if (  equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
                            (struct type*)int_t)
                       || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
                           (struct type*)real_t)
                       || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                           (struct type*)bool_t)) 
                    {
                        if (equal_types(get_expr_type(e->val.binopexpr.e2, functions, variables, types), 
                                    get_expr_type(e->val.binopexpr.e1, functions, variables, types)))
                        {
                            return get_expr_type(e->val.binopexpr.e1, functions, variables, types);
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
                    if (  equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                            (struct type*)int_t)
                       || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                           (struct type*)real_t)
                       || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                           (struct type*)bool_t)) 
                        return get_expr_type(e->val.unopexpr.e, functions, variables, types);

                    break;

                    case arrayexprtype:
                    //something strange is happening here
                    break;

                    case structelttype:
                    break;

                    case dereftype:
                    break;
    }
    return (struct type*)str_t;
}

char* expr_type (struct expr* e)
{
    switch(e->exprtype)
    {
        case valtype:
        switch(e->val.val->valtype)
        {
            case nulltype:
                return "null";
                break;

            case chartype:
                return "char";
                break;

            case stringtype:
                return "string";

            case booltype:
                return "bool";

            case inttype:
                return "int";

            case realtype:
                return "real";
        }
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
