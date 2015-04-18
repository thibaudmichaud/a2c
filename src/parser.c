#include "grammar.h"
#include "lexer.h"
#include "a2c.h"
#include "parser.h"
#include "codegen.h"
#include "get_line.h"
#include <stdarg.h>
#include <string.h>

static struct token *tok;
static struct token *lookahead[2] = { NULL };
extern char *srcfilename;
static int syntaxerror = 0;

#define EXPR -1
#define INSTR -2

struct expr *prefix(char *expect);
struct expr *infix(struct expr *left, char *expect);

void next(void)
{
  tok = lookahead[0];
  lookahead[0] = lookahead[1];
  lookahead[1] = gettok();
}

int lbp(int type)
{
  switch (type)
  {
    case LPAREN: case LSQBRACKET:
      return 130;
    case NOT: case UMINUS: case DEREF:
      return 120;
    case STAR: case SLASH:
      return 110;
    case PLUS: case MINUS: case OR: case XOR: case AND:
      return 100;
    case LT: case LE: case GT: case GE: case NEQ: case EQ:
      return 90;
    case EXPR:
      return 80;
    default:
      return 0;
  }
}

void error(char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  syntaxerror = 1;
  fprintf(stderr, "%s:%d:%d: error: ",
      srcfilename, tok->pos->line, tok->pos->charstart);
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
  fprintf(stderr, "%s", get_line(fin, tok->pos->line));
  for (unsigned i = 0; i < tok->pos->charstart; ++i)
    fprintf(stderr, " ");
  printf("^\n");
  exit(1);
}

char *toktypestr(enum tokentype toktype)
{
  switch (toktype)
  {
    case ASSIGN: return "<-";
    case ENDOFFILE: return "EOF";
    case EOL: return "EOL";
    case FUNCTION: return "fonction";
    case PROCEDURE: return "procedure";
    case RETURN: return "retourne";
    case RPAREN: return ")";
    case RSQBRACKET: return "]";
    case UNTIL: return "jusqu'a";
    case FOR: return "pour";
    case END: return "fin";
    default: return "something else";
  }
}

void eat(enum tokentype expect)
{
  next();
  if (tok->type != expect)
    error("expected %s, not %s", toktypestr(expect), tok->val);
}

/*
 * A simple Top-Down Operator Precedence parser (a.k.a. Pratt parser) to parse
 * expressions.
 * Inspired by this article:
 * http://effbot.org/zone/simple-top-down-parsing.htm
 * The standard function names "nud" and "led" have been replaced by more
 * explicit names: "prefix" and "infix".
 */

/*-------------*
 | EXPRESSIONS | 
 *-------------*/

struct expr *expression(int rbp)
{
  next();
  struct expr *left = prefix("expression");
  while (!syntaxerror && rbp < lbp(lookahead[0]->type))
  {
    next();
    left = infix(left, "expression");
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
      return identexpr(tok->val);
    case LPAREN:
      res = expression(lbp(EXPR));
      eat(RPAREN);
      return res;
    case PLUS: case MINUS: case NOT: case DEREF:
      return unopexpr(toktype, expression(lbp(toktype) - 1));
    case NUM:
      return expr_from_val(intval(atoi(tok->val)));
    default:
      error("expected %s, not '%s'", expect, tok->val);
      return NULL;
  }
}

struct expr *infix(struct expr *left, char *expect)
{
  exprlist_t args;
  int toktype = tok->type;
  switch (toktype)
  {
    case PLUS: case MINUS: case STAR: case SLASH: case OR: case XOR:
    case LT: case LE: case GT: case GE: case NEQ: case EQ: case AND:
      return binopexpr(left, toktype, expression(lbp(toktype)));
    case LPAREN:
      // TODO : check that lhs is an identifier.
      args = empty_exprlist();
      if (lookahead[0]->type != COMMA && lookahead[0]->type != RPAREN)
        list_push_back(args, expression(lbp(EXPR)));
      while (lookahead[0]->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, expression(lbp(EXPR)));
      }
      eat(RPAREN);
      return funcallexpr(left->val.ident, args);
    case LSQBRACKET:
      args = empty_exprlist();
      list_push_back(args, expression(lbp(EXPR)));
      while (lookahead[0]->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, expression(lbp(EXPR)));
      }
      eat(RSQBRACKET);
      return arrayexpr(left, args);
    default:
      error("expected %s, not %s", expect, tok->val);
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
  return expression(lbp(EXPR));
}

/*
 * LL(1) parser using the above TDOP parser for expressions and a few hacks to
 * handle non-LL(1) grammar structures.
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
      && lookahead[1]->type != COLON)
    list_push_back(block, parse_instruction());
  return block;
}

struct instruction *parse_while(void)
{
  struct expr *cond;
  instructionlist_t block;
  eat(WHILE); eat(SO);
  cond = parse_expression();
  eat(DO); eat(EOL);
  block = parse_block();
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
  eat(ASSIGN);
  struct expr *rhs = parse_expression();
  return assign(lhs, rhs);
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
    eat(OTHERWISE); eat(EOL);
    otherwiseblock = parse_block();
  } else otherwiseblock = empty_instructionlist();
  eat(END); eat(SWITCH); eat(EOL);
  return switchblock(cond, caselist, otherwiseblock);
}

struct instruction *parse_instruction(void)
{
  struct expr *expr;
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
           eat(EOL);
           return funcallinstr(expr->val.funcall.fun_ident,
               expr->val.funcall.args);
         default:
           error("unexpected %s", tok->val);
           next();
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
       error("expected instruction, not %s", tok->val);
       return parse_instruction();
  }
}

/*--------------------*
 * PARSER ENTRY POINT |
 * -------------------*/

instructionlist_t parse(void)
{
  lookahead[0] = gettok(); lookahead[1] = gettok();
  instructionlist_t block = parse_block();
  if (!syntaxerror)
    return block;
  else return empty_instructionlist();
}
