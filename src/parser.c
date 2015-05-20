#include "grammar.h"
#include "lexer.h"
#include "a2c.h"
#include "parser.h"
#include "codegen.h"
#include "get_line.h"
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "error.h"

static struct token *tok;
static struct token *lookahead[2] = { NULL };
extern char *srcfilename;

struct expr *prefix(char *expect);
struct expr *infix(struct expr *left, char *expect);

void next(void)
{
  if (tok)
    freetok(tok);
  tok = lookahead[0];
  lookahead[0] = lookahead[1];
  lookahead[1] = gettok();
}

int lbp(int type)
{
  switch (type)
  {
    case LPAREN: case LSQBRACKET:
      return 50;
    case NOT: case UMINUS: case DEREF: case DOT:
      return 40;
    case STAR: case SLASH:
      return 30;
    case PLUS: case MINUS: case OR: case XOR: case AND:
      return 20;
    case LT: case LE: case GT: case GE: case NEQ: case EQ:
      return 10;
    default:
      return 0;
  }
}

static void syntaxerror(char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  error(*tok->pos, msg, args);
  exit(1);
}

void eat(enum tokentype expect)
{
  next();
  if (tok->type != expect)
    syntaxerror("expected %s, not %s", describe(expect), tok->val);
}

/*
 * A simple Top-Down Operator Precedence parser (a.k.a. Pratt parser) to parse
 * expressions.
 * Inspired by this article:
 * http://effbot.org/zone/simple-top-down-parsing.htm
 * original paper:
 * http://hall.org.ua/halls/wizzard/pdf/Vaughan.Pratt.TDOP.pdf
 * The function names used in the original paper, "nud" and "led", have been
 * replaced by more explicit ones: "prefix" and "infix".
 */

/*-------------*
 | EXPRESSIONS | 
 *-------------*/

struct expr *expression(int rbp)
{
  next();
  struct expr *left = prefix("expression");
  struct pos pos;
  left->pos = *tok->pos;
  while (rbp < lbp(lookahead[0]->type))
  {
    next();
    pos = left->pos;
    left = infix(left, "expression");
    left->pos.line = pos.line;
    left->pos.charstart = pos.charstart;
    left->pos.len = tok->pos->charstart + tok->pos->len - pos.charstart;
  }
  return left;
}

struct expr *prefix(char *expect)
{
  struct expr *res = NULL;
  int toktype = tok->type;
  switch (toktype)
  {
    case IDENTIFIER:
      return identexpr(strdup(tok->val));
    case LPAREN:
      res = expression(0);
      eat(RPAREN);
      return res;
    case PLUS: case MINUS: case NOT: case DEREF:
      return unopexpr(toktype, expression(lbp(toktype) - 1));
    case REAL:
      return expr_from_val(realval(atof(tok->val)));
    case INT:
      return expr_from_val(intval(atoi(tok->val)));
    case STRING:
      return expr_from_val(strval(strdup(tok->val)));
    case TRUE:
      return expr_from_val(boolval(1));
    case FALSE:
      return expr_from_val(boolval(0));
    default:
      syntaxerror("expected %s, not %s", expect, tok->val);
      return NULL;
  }
}

struct expr *infix(struct expr *left, char *expect)
{
  exprlist_t args;
  int toktype = tok->type;
  char *ident;
  switch (toktype)
  {
    case PLUS: case MINUS: case STAR: case SLASH: case OR: case XOR:
    case LT: case LE: case GT: case GE: case NEQ: case EQ: case AND:
      return binopexpr(left, toktype, expression(lbp(toktype)));
    case LPAREN:
      if (left->exprtype != identtype)
        syntaxerror("calling non-callable expression");
      args = empty_exprlist();
      if (lookahead[0]->type != COMMA && lookahead[0]->type != RPAREN)
        list_push_back(args, expression(0));
      while (lookahead[0]->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, expression(0));
      }
      eat(RPAREN);
      ident = strdup(left->val.ident);
      left->type = NULL; // To prevent segfaults when freeing the expr
      free_expression(left);
      return funcallexpr(ident, args);
    case LSQBRACKET:
      args = empty_exprlist();
      list_push_back(args, expression(0));
      while (lookahead[0]->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, expression(0));
      }
      eat(RSQBRACKET);
      return arrayexpr(left, args);
    case DOT:
      eat(IDENTIFIER);
      char *ident = strdup(tok->val);
      return record_access(left, ident);
    default:
      syntaxerror("expected %s, not %s", expect, tok->val);
      next();
      switch (tok->type)
      {
        case ENDOFFILE: return expr_from_val(0);
        default: return infix(left, "expression");
      }
  }
}

struct expr *parse_expression(void)
{
  return expression(0);
}

/*
 * LL(2) parser using the above TDOP parser for expressions and a few hacks to
 * handle non-LL(2) grammar structures.
 */

/*--------------*
 | INSTRUCTIONS |
 * -------------*/

struct instruction *parse_instruction(void);

instructionlist_t parse_block(void)
{
  instructionlist_t block = empty_instructionlist();
  while (lookahead[0]->type != END && lookahead[0]->type != ENDOFFILE
      && lookahead[0]->type != ELSE && lookahead[1]->type != COMMA
      && lookahead[1]->type != COLON && lookahead[0]->type != OTHERWISE)
    list_push_back(block, parse_instruction());
  return block;
}

struct instruction *parse_while(void)
{
  eat(WHILE); eat(SO);
  struct expr *cond = parse_expression();
  eat(DO); eat(EOL);
  instructionlist_t block = parse_block();
  eat(END); eat(WHILE); eat(SO); eat(EOL);
  return whileblock(cond, block);
}

struct instruction *parse_do(void)
{
  struct expr *cond;
  instructionlist_t block = empty_instructionlist();
  instructionlist_t subblock;
  eat(DO); eat(EOL);
  while (true)
  {
    if (lookahead[0]->type != WHILE)
      list_push_back(block, parse_instruction());
    else // while could close the do..while or start a while..do
    {
      eat(WHILE); eat(SO);
      cond = parse_expression();
      if (lookahead[0]->type == EOL) // closing the do..while
      {
        eat(EOL);
        return dowhileblock(block, cond);
      }
      else // starting a while..do
      {
        eat(DO); eat(EOL);
        subblock = parse_block();
        eat(END); eat(WHILE); eat(SO); eat(EOL);
        list_push_back(block, whileblock(cond, subblock));
      }
    }
  }
}

struct assignment *parse_assignment(struct expr *lhs)
{
  struct pos pos = *tok->pos;
  eat(ASSIGN);
  struct expr *rhs = parse_expression();
  return assign(lhs, rhs, pos);
}

struct instruction *parse_assignment_instr(struct expr *lhs)
{
  struct assignment *a = parse_assignment(lhs);
  eat(EOL);
  return assigninstr(a);
}

struct instruction *parse_for(void)
{
  int decreasing = 0;
  eat(FOR);
  struct expr *lhs = parse_expression();
  struct assignment *a = parse_assignment(lhs);
  eat(UNTIL);
  struct expr *until = parse_expression();
  if (lookahead[0]->type == DECREASING)
  {
    decreasing = 1;
    eat(DECREASING);
  }
  eat(DO); eat(EOL);
  instructionlist_t block = parse_block();
  eat(END); eat(FOR); eat(EOL);
  return forblock(a, until, decreasing, block);
}

struct instruction *parse_if(void)
{
  instructionlist_t elseblock;
  eat(IF);
  struct expr *cond = parse_expression();
  eat(THEN); eat(EOL);
  instructionlist_t block = parse_block();
  if (lookahead[0]->type == ELSE)
  {
    eat(ELSE); eat(EOL);
    elseblock = parse_block();
  }
  else elseblock = empty_instructionlist();
  eat(END); eat(IF); eat(EOL);
  return ifthenelseblock(cond, block, elseblock);
}

exprlist_t parse_expressionlist(void)
{
  exprlist_t exprlist = empty_exprlist();
  list_push_back(exprlist, parse_expression());
  while (lookahead[0]->type == COMMA)
  {
    eat(COMMA);
    list_push_back(exprlist, parse_expression());
  } 
  return exprlist;
}

struct caseblock *parse_caseblock(void)
{
  exprlist_t exprlist = parse_expressionlist();
  eat(COLON);
  instructionlist_t block = parse_block();
  return caseblock(exprlist, block);
}

struct instruction *parse_switch(void)
{
  instructionlist_t otherwiseblock;
  eat(SWITCH);
  struct expr *cond = parse_expression();
  eat(DO); eat(EOL);
  caseblocklist_t caselist = empty_caseblocklist();
  while (lookahead[0]->type != END && lookahead[0]->type != OTHERWISE)
    list_push_back(caselist, parse_caseblock());
  if (lookahead[0]->type == OTHERWISE)
  {
    eat(OTHERWISE);
    otherwiseblock = parse_block();
  } else otherwiseblock = empty_instructionlist();
  eat(END); eat(SWITCH); eat(EOL);
  return switchblock(cond, caselist, otherwiseblock);
}

struct instruction *parse_instruction(void)
{
  struct expr *expr;
  struct instruction *res;
  switch (lookahead[0]->type)
  {
    case WHILE: return parse_while();
    case DO: return parse_do();
    case IDENTIFIER:
       expr = parse_expression();
       switch(lookahead[0]->type)
       {
         case ASSIGN: return parse_assignment_instr(expr);
         case EOL:
           if (expr->exprtype != funcalltype)
           {
             if (expr->exprtype == binopexprtype
                 && expr->val.binopexpr.op == EQ)
               error(expr->pos, "unexpected =, did you mean <- ?");
             else
               syntaxerror("expected instruction, not expression");
           }
           eat(EOL);
           res = funcallinstr(expr->val.funcall.fun_ident,
               expr->val.funcall.args, expr->pos);
           free(expr);
           return res;
         default:
           next();
           syntaxerror("unexpected %s", tok->val);
           return parse_instruction();
       }
    case RETURN:
       eat(RETURN);
       if (lookahead[0]->type == EOL)
       {
         eat(EOL);
         return return_stmt(NULL);
       }
       else
       {
         expr = parse_expression(); eat(EOL);
         return return_stmt(expr);
       }
    case FOR: return parse_for();
    case IF: return parse_if();
    case SWITCH: return parse_switch();
    case ENDOFFILE: return NULL;
    default:
       next();
       syntaxerror("expected instruction, not %s", tok->val);
       return parse_instruction();
  }
}

/*--------------*
 | DECLARATIONS |
 *--------------*/

struct single_var_decl *parse_vardecl(void)
{
  struct pos pos = *tok->pos;
  identlist_t identlist = empty_identlist();
  eat(IDENTIFIER);
  char *type = strdup(tok->val);
  eat(IDENTIFIER);
  while (lookahead[0]->type == COMMA)
  {
    list_push_back(identlist, strdup(tok->val));
    eat(COMMA); eat(IDENTIFIER);
  }
  list_push_back(identlist, strdup(tok->val));
  eat(EOL);
  return single_var_decl(type, identlist, pos);
}

vardecllist_t parse_vardecls()
{
  vardecllist_t vardecllist = empty_vardecllist();
  while (lookahead[0]->type != BEGIN && lookahead[0]->type != TYPES
      && lookahead[0]->type != CONST && lookahead[0]->type != VARIABLES
      && lookahead[0]->type != PARAM && lookahead[0]->type != END)
    list_push_back(vardecllist, parse_vardecl());
  return vardecllist;
}

struct val *parse_val(void)
{
  next();
  switch (tok->type)
  {
    case INT: return intval(atoi(tok->val));
    case REAL: return realval(atof(tok->val));
    case STRING: return strval(tok->val);
    case TRUE: return boolval(true);
    case FALSE: return boolval(false);
    case CHAR: return charval(tok->val[0]);
    default:
      syntaxerror("expected a value, not %s", tok->val);
      return intval(0);
  }
}

struct const_decl *parse_constdecl(void)
{
  struct const_decl *constdecl = malloc(sizeof(struct const_decl));
  eat(IDENTIFIER);
  constdecl->type = strdup(tok->val);
  eat(IDENTIFIER);
  constdecl->ident = strdup(tok->val);
  eat(EQ);
  constdecl->val = parse_val();
  eat(EOL);
  return constdecl;
}

constdecllist_t parse_constdecls(void)
{
  eat(CONST); eat(EOL);
  constdecllist_t constdecllist = empty_constdecllist();
  while (lookahead[0]->type != BEGIN && lookahead[0]->type != TYPES
      && lookahead[0]->type != CONST && lookahead[0]->type != VARIABLES
      && lookahead[0]->type != PARAM)
    list_push_back(constdecllist, parse_constdecl());
  return constdecllist;
}

struct type_def *parse_enum_def(void)
{
  eat(LPAREN);
  identlist_t identlist = empty_identlist();
  eat(IDENTIFIER);
  list_push_back(identlist, strdup(tok->val));
  while (lookahead[0]->type == COMMA)
  {
    eat(COMMA);
    eat(IDENTIFIER);
    list_push_back(identlist, strdup(tok->val));
  }
  eat(RPAREN); eat(EOL);
  return make_enum_def(identlist);
}

struct type_def *parse_record_def(void)
{
  eat(RECORD); eat(EOL);
  vardecllist_t vardecl = parse_vardecls();
  eat(END); eat(RECORD); eat(IDENTIFIER); eat(EOL);
  return make_record(vardecl);
}

struct type_def *parse_array_def(void)
{
  struct pos pos = *tok->pos;
  intlist_t dims = empty_intlist();
  eat(INT);
  list_push_back(dims, atoi(tok->val));
  while (lookahead[0]->type == X)
  {
    eat(X);
    eat(INT);
    list_push_back(dims, atoi(tok->val));
  }
  eat(IDENTIFIER);
  char *type = strdup(tok->val);
  eat(EOL);
  return make_array_def(dims, type, pos);
}

struct type_def *parse_pointer_def(void)
{
  eat(DEREF);
  eat(IDENTIFIER);
  char *type = strdup(tok->val);
  eat(EOL);
  return make_pointer_def(type);
}

struct type_decl *parse_typedecl(void)
{
  eat(IDENTIFIER);
  char *ident = strdup(tok->val);
  struct type_def *type_def = NULL;
  eat(EQ);
  switch (lookahead[0]->type)
  {
    case LPAREN: type_def = parse_enum_def(); break;
    case RECORD: type_def = parse_record_def(); break;
    case INT:    type_def = parse_array_def(); break;
    case DEREF:  type_def = parse_pointer_def(); break;
    default: syntaxerror("expected type definition, not %s", tok->val);
  }
  return make_type_decl(ident, type_def);
}

typedecllist_t parse_typedecls(void)
{
  eat(TYPES); eat(EOL);
  typedecllist_t typedecllist = empty_typedecllist();
  while (lookahead[0]->type != BEGIN && lookahead[0]->type != TYPES
      && lookahead[0]->type != CONST && lookahead[0]->type != VARIABLES
      && lookahead[0]->type != PARAM)
    list_push_back(typedecllist, parse_typedecl());
  return typedecllist;
}

/*------------*
 | ALGORITHMS |
 *------------*/

vardecllist_t parse_lp(void)
{
  eat(LOCAL); eat(EOL);
  return parse_vardecls();
}

vardecllist_t parse_gp(void)
{
  eat(GLOBAL); eat(EOL);
  vardecllist_t gp = parse_vardecls();
  return gp;
}

struct declarations *parse_decls(void)
{
  struct declarations *declarations = calloc(1, sizeof(struct declarations));
  vardecllist_t lp;
  vardecllist_t gp;
  int local_first = 1;
  if (lookahead[0]->type == PARAM)
  {
    eat(PARAM);
    if (lookahead[0]->type == LOCAL)
    {
      lp = parse_lp();
      if (lookahead[0]->type == PARAM)
      {
        eat(PARAM);
        gp = parse_gp();
      }
      else
        list_init(gp);
    }
    else if (lookahead[0]->type == GLOBAL)
    {
      local_first = 0;
      gp = parse_gp();
      if (lookahead[0]->type == PARAM)
      {
        eat(PARAM);
        lp = parse_lp();
      }
      else list_init(lp);
    }
    else syntaxerror("Expected \"local\" or \"global\"");
  }
  else
  {
    list_init(lp);
    list_init(gp);
  }
  if (lookahead[0]->type == VARIABLES)
  {
    eat(VARIABLES); eat(EOL);
    declarations->var_decl = parse_vardecls();
    if (lookahead[0]->type == CONST)
    {
      declarations->const_decls = parse_constdecls();
      if (lookahead[0]->type == TYPES)
        declarations->type_decls = parse_typedecls();
    }
    else if (lookahead[0]->type == TYPES)
    {
      declarations->type_decls = parse_typedecls();
      if (lookahead[0]->type == CONST)
        declarations->const_decls = parse_constdecls();
    }
  }
  else if (lookahead[0]->type == CONST)
  {
    declarations->const_decls = parse_constdecls();
    if (lookahead[0]->type == VARIABLES)
    {
      eat(VARIABLES); eat(EOL);
      declarations->var_decl = parse_vardecls();
      if (lookahead[0]->type == TYPES)
        declarations->type_decls = parse_typedecls();
    }
    else if (lookahead[0]->type == TYPES)
    {
      declarations->type_decls = parse_typedecls();
      if (lookahead[0]->type == TYPES)
      {
        eat(VARIABLES); eat(EOL);
        declarations->var_decl = parse_vardecls();
      }
    }
  }
  else if (lookahead[0]->type == TYPES)
  {
    declarations->type_decls = parse_typedecls();
    if (lookahead[0]->type == CONST)
    {
      declarations->const_decls = parse_constdecls();
      if (lookahead[0]->type == VARIABLES)
      {
        eat(VARIABLES); eat(EOL);
        declarations->var_decl = parse_vardecls();
      }
    }
    else if (lookahead[0]->type == VARIABLES)
    {
      eat(VARIABLES); eat(EOL);
      declarations->var_decl = parse_vardecls();
      if (lookahead[0]->type == CONST)
        declarations->const_decls = parse_constdecls();
    }
  }
  declarations->param_decl = make_param_decl(local_first, lp, gp);
  return declarations;
}

struct algo *parse_function()
{
  eat(FUNCTION);
  eat(IDENTIFIER);
  char *ident = strdup(tok->val);
  eat(COLON); eat(IDENTIFIER);
  char *returntype = strdup(tok->val);
  eat(EOL);
  struct declarations *decls = parse_decls();
  eat(BEGIN); eat(EOL);
  instructionlist_t block = parse_block();
  eat(END); eat(ALGORITHM); eat(FUNCTION); eat(IDENTIFIER);
  if (lookahead[0]->type == EOL) eat(EOL);
  return algo(ident, returntype, decls, block);
}

struct algo *parse_procedure()
{
  eat(PROCEDURE);
  eat(IDENTIFIER);
  char *ident = strdup(tok->val);
  eat(EOL);
  struct declarations *decls = parse_decls();
  eat(BEGIN); eat(EOL);
  instructionlist_t block = parse_block();
  eat(END); eat(ALGORITHM); eat(PROCEDURE); eat(IDENTIFIER);
  if (lookahead[0]->type == EOL) eat(EOL);
  return algo(ident, NULL, decls, block);
}

struct algo *parse_algo(void)
{
  eat(ALGORITHM);
  if (lookahead[0]->type == PROCEDURE)
    return parse_procedure();
  else
    return parse_function();
}

algolist_t parse_algolist(void)
{
  algolist_t algolist = empty_algolist();
  while (lookahead[0]->type == ALGORITHM)
  {
    list_push_back(algolist, parse_algo());
  }
  return algolist;
}

struct prog *parse_prog(void)
{
  algolist_t algolist = parse_algolist();
  vardecllist_t globvar;
  if (lookahead[0]->type == VARIABLES)
  {
    eat(VARIABLES); eat(EOL);
    globvar = parse_vardecls();
  }
  else
    globvar = empty_vardecllist();
  eat(BEGIN); eat(EOL);
  instructionlist_t instrs = parse_block();
  eat(END); eat(EOL);
  struct entry_point *entrypoint = make_entry_point(globvar, instrs);
  return make_prog(algolist, entrypoint);
}

/*--------------------*
 | PARSER ENTRY POINT |
 * -------------------*/

struct prog *parse(void)
{
  lookahead[0] = gettok(); lookahead[1] = gettok();
  struct prog *prog = parse_prog();
  freetok(tok); freetok(lookahead[0]); freetok(lookahead[1]);
  return prog;
}
