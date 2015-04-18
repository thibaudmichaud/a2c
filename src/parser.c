#include "grammar.h"
#include "lexer.h"
#include "a2c.h"
#include "parser.h"
#include "codegen.h"
#include "get_line.h"
#include <stdarg.h>

static struct token *tok;
static struct token *lookahead = NULL;
extern char *srcfilename;
static int syntaxerror = 0;

#define EXPPRIORITY 80

struct expr *prefix(char *expect);
struct expr *infix(struct expr *left, char *expect);

int lbp(enum tokentype type)
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
}

void eat(enum tokentype expect)
{
  struct token *tok = lookahead;
  lookahead = gettok();
  if (tok->type != expect)
    error("expected %s, not %s", toktypestr(expect), tok->val);
}

struct expr *parse_rec(int rbp, char *expect)
{
  tok = lookahead;
  lookahead = gettok();
  struct expr *left = prefix(expect);
  while (left && rbp < lbp(lookahead->type))
  {
    tok = lookahead;
    lookahead = gettok();
    left = infix(left, expect);
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
      res = parse_rec(EXPPRIORITY, "expression");
      eat(RPAREN);
      return res;
    case PLUS: case MINUS: case NOT: case DEREF:
      return unopexpr(toktype, parse_rec(lbp(toktype) - 1, "expression"));
    case NUM:
      return expr_from_val(intval(atoi(tok->val)));
    default:
      error("expected %s, not '%s'", expect, tok->val);
      tok = lookahead; lookahead = gettok();
      switch (tok->type)
      {
        case ENDOFFILE: return expr_from_val(0);
        default: return prefix("expression");
      }
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
      return binopexpr(left, toktype, parse_rec(lbp(toktype), "expression"));
    case LPAREN:
      // TODO : check that lhs is an identifier.
      args = empty_exprlist();
      if (lookahead->type != COMMA && lookahead->type != RPAREN)
        list_push_back(args, parse_rec(EXPPRIORITY, "expression"));
      while (lookahead->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, parse_rec(EXPPRIORITY, "expression"));
      }
      eat(RPAREN);
      return funcallexpr(left->val.ident, args);
    case LSQBRACKET:
      args = empty_exprlist();
      list_push_back(args, parse_rec(EXPPRIORITY, "expression"));
      while (lookahead->type == COMMA)
      {
        eat(COMMA);
        list_push_back(args, parse_rec(EXPPRIORITY, "expression"));
      }
      eat(RSQBRACKET);
      return arrayexpr(left, args);
    default:
      error("expected %s, not %s", expect, tok->val);
      tok = lookahead; lookahead = gettok();
      switch (tok->type)
      {
        case ENDOFFILE: return expr_from_val(0);
        default: return infix(left, "expression");
      }
  }
}

struct expr *parse(void)
{
  lookahead = gettok();
  struct expr *e = parse_rec(0, "expression");
  return syntaxerror ? NULL : e;
}
