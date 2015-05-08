#include <stdarg.h>
#include "typecheck.h"
#include "get_line.h"
#include "parser.h"
#include "a2c.h"
#include "lexer.h"

void error(unsigned line, unsigned charstart, unsigned len, char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "%s:%d:%d: error: ",
      srcfilename, line, charstart);
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
  char *linestr = get_line(fin, line);
  fprintf(stderr, "%s", linestr);
  free(linestr);
  for (unsigned i = 0; i < charstart; ++i)
    fprintf(stderr, " ");
  for (unsigned i = 0; i < len; ++i)
    fprintf(stderr, "^");
  fprintf(stderr, "\n");
}

struct type* char_to_type(char* ident_type)
{
    struct type* t = malloc(sizeof( struct type));
    if(ident_type == NULL)
    {
        t->type_kind = primary_t;
        t->type_val.primary = nul_t;
    }
    else if(strcmp(ident_type, "entier") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary  = int_t;
    }
    else if(strcmp(ident_type, "reel") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = real_t;
    }
    else if(strcmp(ident_type, "boolen") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = bool_t;
    }
    else if(strcmp(ident_type, "caractere") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = char_t;
    }
    else if(strcmp(ident_type, "chaine") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = str_t;
    }

    return t;

}

bool equal_dims(intlist_t dim1, intlist_t dim2)
{
  if (dim1.size != dim2.size)
    return false;
  for (unsigned i = 0; i < dim1.size; ++i)
    if (dim1.data[i] != dim2.data[i])
      return false;
  return true;
}

bool equal_identlist(identlist_t idents1, identlist_t idents2)
{
  if (idents1.size != idents2.size) return false;
  for (unsigned i = 0; i < idents1.size; ++i)
    if (strcmp(idents1.data[i], idents2.data[i]) != 0)
      return false;
  return true;
}

bool equal_types(struct type* t1, struct type* t2)
{

    if(t1->type_kind != t2->type_kind)
        return false;

    switch(t1->type_kind)
    {
        case primary_t:
            return t1->type_val.primary == t2->type_val.primary;
            break;
        case records_t:
            if(t1->type_val.records_type->fields.size != t2->type_val.records_type->fields.size)
                return false;
            for(unsigned i = 0; i < t1->type_val.records_type->fields.size; ++i)
            {
                struct field* field1 = list_nth(t1->type_val.records_type->fields, i);
                struct field* field2 = list_nth(t2->type_val.records_type->fields, i);

                if(!equal_types(field1->type, field2->type))
                    return false;
            }
            return true;
            break;
        case array_t:
            return equal_dims(t1->type_val.array_type->dims,
                t2->type_val.array_type->dims)
              && equal_types(t1->type_val.array_type->type,
                  t2->type_val.array_type->type);
            break;
        case pointer_t:
            return equal_types(t1->type_val.pointer_type->type,
                t2->type_val.pointer_type->type);
            break;
        case enum_t:
            return equal_identlist(t1->type_val.enum_type->idents,
                t2->type_val.enum_type->idents);
            break;

    }
    return true;
}

bool check_args(struct function* f, struct funcall call,
        fun_table_t* functions, var_table_t* variables, type_table_t* types)
{
    if(f->arg != NULL)
    {
        if(f->arg->size != call.args.size)
            return false;
        for(unsigned i = 0; i < call.args.size; ++i)
        {
            if(!equal_types(get_expr_type((struct expr *)list_nth(call.args,i), functions,
                            variables, types), list_nth(*f->arg,i)->type))
                return false;
        }
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
    bool correct = true;
    fun_table_t* functions = empty_fun_table();
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        struct function* f = malloc(sizeof(struct function));
        f->ident = al->ident;
        f->ret = char_to_type(al->return_type);
        f->arg = NULL;
        add_function(functions,f);
    }
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        if (!check_algo(al, functions))
            correct = false;
    }
    free_fun_table(functions);
    free(functions);
    return correct;
}

bool add_header_variables(var_table_t* variables,
    type_table_t* types, struct single_var_decl* var)
{
    bool noerr = true;
    for(unsigned int j = 0; j < var->var_idents.size; ++j)
    {
        struct var_sym* sym = malloc(sizeof(struct var_sym));
        sym->ident = list_nth(var->var_idents, j);
        struct type_sym *s = find_type(types, var->type_ident);
        if (s)
        {
          sym->type = s->type;
          add_var(variables, sym);
        }
        else
        {
          // TODO get the right line/char
          error(1, 1, 1, "unknown type %s", var->type_ident);
          free(sym);
          noerr = false;
        }
    }
    return noerr;
}

bool check_algo(struct algo* al, fun_table_t* functions)
{
    bool noerr = true;
    struct function* f = malloc(sizeof(struct function));
    f->ret = char_to_type(al->return_type);
    var_table_t* variables = empty_var_table();
    type_table_t* types = empty_type_table();
    struct declarations* decl = al->declarations;

    struct type* t_bool_ = malloc(sizeof(struct type));
    t_bool_->type_kind = primary_t;
    primary_type p1 = bool_t;
    t_bool_->type_val.primary = p1;
    struct type_sym* type_bool = malloc(sizeof(struct type));
    type_bool->ident = "booleen";
    type_bool->type = t_bool_;
    add_type(types,type_bool);

    struct type* t_int_ = malloc(sizeof(struct type));
    t_int_->type_kind = primary_t;
    primary_type p2 = int_t;
    t_int_->type_val.primary = p2;
    struct type_sym* type_int = malloc(sizeof(struct type));
    type_int->ident = "entier";
    type_int->type = t_int_;
    add_type(types,type_int);

    struct type* t_str_ = malloc(sizeof(struct type));
    t_str_->type_kind = primary_t;
    t_str_->type_val.primary = str_t;
    struct type_sym* type_string = malloc(sizeof(struct type));
    type_string->ident = "chaine";
    type_string->type = t_str_;
    add_type(types,type_string);

    struct type* t_reel_ = malloc(sizeof(struct type));
    t_reel_->type_kind = primary_t;
    primary_type p3 = bool_t;
    t_reel_->type_val.primary = p3;
    struct type_sym* type_reel = malloc(sizeof(struct type));
    type_reel->ident = "reel";
    type_reel->type = t_reel_;
    add_type(types,type_reel);

    struct type* t_char_ = malloc(sizeof(struct type));
    t_char_->type_kind = primary_t;
    primary_type p4 = char_t;
    t_char_->type_val.primary = p4;
    struct type_sym* type_char = malloc(sizeof(struct type));
    type_char->ident = "caractere";
    type_char->type = t_reel_;
    add_type(types,type_char);

    if(decl != NULL)
    {
        struct param_decl* p_decl = decl->param_decl;
        struct local_param* loc = p_decl->local_param;
        struct global_param* glo = p_decl->global_param;
        vardecllist_t vars = decl->var_decl;
        typedecllist_t typelist = decl->type_decls;
        constdecllist_t consts = decl->const_decls;
        if(loc != NULL)
        {
            for(unsigned int i = 0; i < loc->param.size; i++)
            {
                struct single_var_decl* var = list_nth(loc->param,i);
                noerr = noerr && add_header_variables(variables, types, var);
            }
        }
        if(glo != NULL)
        {
            for(unsigned int i = 0; i < glo->param.size; i++)
            {
                struct single_var_decl* var = list_nth(glo->param,i);
                noerr = noerr && add_header_variables(variables, types, var);
            }
        }
        for(unsigned i = 0; i < typelist.size; ++i)
        {
            struct type_decl* type_decl = list_nth(typelist, i);
            struct type_def*  type_def  = type_decl->type_def;
            struct type* type = malloc(sizeof(struct type));
            struct type_sym* sym = malloc(sizeof(struct type_sym));
            switch(type_def->type_type)
            {
                case enum_type:
                    {
                        type->type_kind = enum_t;

                        struct enum_type* _enum = malloc(sizeof(struct enum_type));
                        _enum->idents = type_def->def.enum_def->identlist;

                        type->type_val.enum_type = _enum;

                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case array_type:
                    {
                        type->type_kind = array_t;

                        struct array* array = malloc(sizeof(struct array));
                        if((array->type = find_type(types, type_def->def.array_def->elt_type)->type)
                                == NULL)
                        {
                            printf("unknown type\n");
                        }

                        array->dims = type_def->def.array_def->dims;

                        type->type_val.array_type = array;

                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case struct_type:
                    {
                        type->type_kind = records_t;

                        struct records* record = malloc(sizeof(struct records));
                        fieldlist_t fields;
                        list_init(fields);

                        vardecllist_t varlist = type_def->def.record_def->var_decl;

                        for(unsigned int i = 0 ; i < varlist.size; ++i)
                        {
                            struct single_var_decl* var = list_nth(varlist, i);

                            for(unsigned int j = 0; j < var->var_idents.size; ++j)
                            {
                                struct field* field = malloc(sizeof(struct field));
                                field->ident = list_nth(var->var_idents,i);
                                field->type = find_type(types, var->type_ident)->type;

                                list_push_back(fields, field);
                            }
                        }

                        record->fields = fields;

                        type->type_val.records_type = record;

                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case pointer_type:
                    {
                      type->type_kind = pointer_type;
                      type->type_val.pointer_type = malloc(sizeof(struct pointer));
                      type->type_val.pointer_type->type = find_type(types,
                        type_decl->type_def->def.pointer_def->pointed_type_ident)->type;
                      sym->type = type;
                      sym->ident = type_decl->ident;
                      add_type(types, sym);
                    }
                    break;

            }
        }
        for(unsigned i =0; i < vars.size; ++i)
        {
            struct single_var_decl* var = list_nth(vars, i);
            noerr = noerr && add_header_variables(variables, types, var);
        }
        for(unsigned i =0; i < consts.size; ++i)
        {
            struct const_decl* cons = list_nth(consts, i);
            struct var_sym* sym = malloc(sizeof(struct var_sym));
            switch(cons->val->valtype)
            {
                case nulltype:
                    sym->type = find_type(types, "nul")->type;
                    break;
                case chartype:
                    sym->type = find_type(types, "caractere")->type;
                    break;
                case stringtype:
                    sym->type = find_type(types, "chaine")->type;
                    break;
                case inttype:
                    sym->type = find_type(types, "entier")->type;
                    break;
                case realtype:
                    sym->type = find_type(types, "reel")->type;
                    break;
                case booltype:
                    sym->type = find_type(types, "booleen")->type;
                    break;


            }
            sym->ident = cons->ident;
            add_var(variables, sym);
        }
    }
    for(unsigned i = 0; i < al->instructions.size; i++)
      if(!check_inst(list_nth(al->instructions, i), f, functions, variables, types))
        noerr = false;

    free(f->ret);
    free_var_table(variables);
    free_type_table(types);
    free(f);
    free(t_char_);
    free(t_str_);
    free(t_bool_);
    free(t_reel_);
    free(t_int_);
    free(variables);
    free(types);
    return noerr;
}

bool check_inst(struct instruction *i, struct function* fun, fun_table_t* functions,
        var_table_t* variables, type_table_t* types)
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
                    && check_expr(i->instr.assignment->e2, functions, variables, types))
            {
                struct type* t1 = get_expr_type(i->instr.assignment->e1, functions, variables, types);
                struct type* t2 = get_expr_type(i->instr.assignment->e2, functions, variables, types);
                if(!equal_types(t1,t2))
                {
                    printf("ls. %d: ", i->instr.assignment->e1->lineno);
                    printf("error in assignment\n");
                    return false;
                }
                return true;
            }
            else
            {
                return false;
            }
            break;

        case ifthenelse:
            {
                struct ifthenelse* e = i->instr.ifthenelse;

                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = bool_t;
                t->type_val.primary = p;
                struct type* t1 = get_expr_type(e->cond, functions, variables, types);
                if(equal_types(t1, t))
                {
                    for(unsigned int i =0; i < e->instructions.size; ++i)
                    {
                        if (!check_inst(list_nth(e->instructions, i), fun, functions, variables, types))
                        {
                            free(t1);
                            free(t);
                            return false;
                        }
                    }
                    for(unsigned int i =0; i < e->elseblock.size; ++i)
                    {
                        if(!check_inst(list_nth(e->elseblock,i), fun, functions, variables, types))
                        {
                            free(t);
                            return false;
                        }
                    }
                    free(t);
                    return true;
                }

                printf("error in condition statement");
                free(t);
                return false;
            }
            break;

        case switchcase:
            if(!check_expr(i->instr.switchcase->cond, functions, variables, types))
            {
                printf("error in switch expression\n");
                return false;
            }
            for(unsigned int l = 0; l < i->instr.switchcase->caseblocklist.size; ++l)
            {
                for( unsigned int j = 0; j < i->instr.switchcase->caseblocklist.data[l]->exprlist.size; ++j)
                {
                    if(!(check_expr(i->instr.switchcase->caseblocklist.data[l]->exprlist.data[j],
                                    functions,variables,types)
                                && equal_types(
                                    get_expr_type(
                                        i->instr.switchcase->caseblocklist.data[l]->exprlist.data[j],
                                        functions, variables, types),
                                    get_expr_type(i->instr.switchcase->cond, functions, variables, types))))
                    {
                        printf("error in difference type between switch and case\n");
                        return false;
                    }
                }
                for(int unsigned k = 0;
                        k < i->instr.switchcase->caseblocklist.data[l]->instructions.size; ++k)
                {
                    if(!check_inst(i->instr.switchcase->caseblocklist.data[l]->instructions.data[k], fun,
                                functions, variables,types ))
                    {
                        printf("error while checking case instruction\n");
                        return false;
                    }
                }

            }
            for(unsigned int l = 0; l < i->instr.switchcase->otherwiseblock.size; ++l)
            {
                if(!check_inst(i->instr.switchcase->otherwiseblock.data[l], fun,
                            functions, variables, types))
                {
                    printf("error while checking case instruction\n");
                    return false;
                }

            }
            return true;
            break;

        case dowhile:
            {
                struct dowhile* e = i->instr.dowhile;
                if(equal_types(get_expr_type(e->cond, functions, variables, types), 
                            find_type(types,"booleen")->type))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), fun, functions, variables, types))
                        {
                            printf("error in do while loop\n");
                            return false;
                        }
                    }
                    return true;
                }
                printf("error in condition of do while loop\n");
                return false;
            }
            break;

        case whiledo:
            {
                struct whiledo* e = i->instr.whiledo;
                if(equal_types(get_expr_type(e->cond, functions, variables, types), 
                            find_type(types,"booleen")->type))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), fun, functions, variables, types))
                        {
                            printf("error in while loop");
                            return false;
                        }
                    }
                    return true;
                }
                printf("error in condition of while loop");
                return false;
            }
            break;

        case forloop:
            {
                struct forloop* e = i->instr.forloop;
                if(check_expr((struct expr *)e->assignment, functions, variables, types))
                {
                    if(equal_types(get_expr_type(e->upto, functions, variables, types),
                                find_type(types,"entier")->type) )
                    {
                        for(unsigned int i = 0; i < e->instructions.size; ++i)
                        {
                            if(!check_inst(list_nth(e->instructions, i), fun, functions, variables, types))
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
            if(equal_types(get_expr_type(i->instr.returnstmt->expr, functions, variables, types), fun->ret))
                return true;
            printf("error in return statement\n");
            return false;
            break;
    }
    return false;

}

bool check_expr(struct expr *e, fun_table_t* functions, var_table_t* variables, type_table_t* types)
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
            if (equal_types(get_expr_type(e->val.binopexpr.e2, functions, variables, types),
                        get_expr_type(e->val.binopexpr.e1, functions, variables, types)))
            {
                if(e->val.binopexpr.op == EQ 
                    || e->val.binopexpr.op == LT
                    || e->val.binopexpr.op == GT
                    || e->val.binopexpr.op == LE
                    || e->val.binopexpr.op == GE)
                {
                    struct type* t = malloc(sizeof(struct type));
                    t->type_kind = primary_t;
                    primary_type p = bool_t;
                    t->type_val.primary = p;
                    return true;
                }
                return true;
            }
            else
            {
                printf("%s:%d: ", srcfilename, e->lineno);
                printf("incompatible types: ");
                printf("%s ", expr_type(e->val.binopexpr.e1));
                printf("and ");
                printf("%s.\n", expr_type(e->val.binopexpr.e2));
                char *error_line = get_line(fin, e->lineno);
                printf("%s", error_line);
                free(error_line);
                return false;
            }
            break;

        case unopexprtype:
            if(get_expr_type(e->val.unopexpr.e, functions, variables, types))
                return true;
            printf("%d;",e->lineno);
            return false;
            break;

        case arrayexprtype:
            {


            }
            break;

        case structelttype:
            break;

        case dereftype:
            if(check_expr(e->val.deref.e, functions, variables, types ))
                return true;
            printf("%d;",e->lineno);
            return false;
            break;

        default:
            break;
    }
    return true;

}

struct type* get_expr_type(struct expr *e, fun_table_t* functions, var_table_t* variables,
        type_table_t* types)
{

    switch(e->exprtype)
    {
        case valtype:
            switch(e->val.val->valtype)
            {
                case nulltype:
                    return find_type(types,"nul")->type;
                case chartype:
                    return find_type(types,"caractere")->type;
                case stringtype:
                    return find_type(types,"chaine")->type;
                case booltype:
                    return find_type(types,"booleen")->type;
                case inttype:
                    return find_type(types,"entier")->type;
                case realtype:
                    return find_type(types,"reel")->type;
            }
        case identtype:
            return find_var(variables, e->val.ident)->type;
            break;

        case funcalltype :
            {
                struct function* f;
                if(( f = get_function(functions, e->val.funcall.fun_ident)) != NULL
                        && check_args(f, e->val.funcall, functions, variables, types))
                    return f->ret;
            }
            break;

        case binopexprtype:
            /*if (  equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
              (struct type*)int_t)
              || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
              (struct type*)real_t)
              || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
              (struct type*)bool_t))
              {*/
            if (equal_types(get_expr_type(e->val.binopexpr.e2, functions, variables, types),
                        get_expr_type(e->val.binopexpr.e1, functions, variables, types)))
            {
                if(e->val.binopexpr.op == EQ 
                        || e->val.binopexpr.op == LT
                        || e->val.binopexpr.op == GT
                        || e->val.binopexpr.op == LE
                        || e->val.binopexpr.op == GE)
                        
                {
                    return find_type(types,"booleen")->type;
                }
                return get_expr_type(e->val.binopexpr.e1, functions, variables, types);
            }
            else
            {
                printf("expression left was of type : ");
                printf("%s ", expr_type(e->val.binopexpr.e1));
                printf("and expression right was of type : ");
                printf("%s\n", expr_type(e->val.binopexpr.e2));
            }
            //}
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
            return get_expr_type(e->val.arrayexpr.e1, functions, variables, types);
            //something strange is happening here
            break;

        case structelttype:
            break;

        case dereftype:
            break;
    }
    return NULL;
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
            return "ident";
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
