#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"
#include "a2c.h"

static int curline = 0;
static int curchar = 0;

char *toktypestr(enum tokentype toktype)
{
  switch (toktype)
  {
    case AND: return "et";
    case ASSIGN: return "<-";
    case COMMA: return ",";
    case DEREF: return "^";
    case DIV: return "div";
    case ENDOFFILE: return "EOF";
    case EQ: return "=";
    case FOR: return "pour";
    case FUNCTION: return "fonction";
    case GE: return ">=";
    case GT: return ">";
    case IDENTIFIER: return "identifier";
    case LE: return "<=";
    case LPAREN: return "(";
    case LSQBRACKET: return "[";
    case LT: return "<";
    case MINUS: return "-";
    case MOD: return "mod";
    case NEQ: return "<>";
    case NOT: return "non";
    case NULLKW: return "NUL";
    case NUM: return "number";
    case OR: return "ou";
    case PLUS: return "+";
    case PROCEDURE: return "procedure";
    case RPAREN: return ")";
    case RSQBRACKET: return "]";
    case SLASH: return "/";
    case STAR: return "*";
    case TYPE: return "type";
    case UMINUS: return "-";
    case WHILE: return "tant que";
    case XOR: return "oue";
    default: return "(unknown token)";
  }
}

char nextnonblank()
{
  char c;
  while (true)
  {
    switch (c = getc(fin))
    {
      case '\n':
        ++curline;
        curchar = -1;
        // Fall through.
      case ' ':
      case '\t':
        break;
      default:
        return c;
    }
    ++curchar;
  }
}

void getalnum(struct token *tok)
{
  unsigned i = -1;
  size_t len = 8;
  tok->val = malloc(len);

  do
  {
    ++i;
    ++curchar;
    tok->val[i] = getc(fin);
    if (i + 1 >= len)
      tok->val = realloc(tok->val, 2 * len);
  }
  while (isalnum(tok->val[i]));

  ungetc(tok->val[i], fin);
  --curchar;
  tok->val[i] = '\0';

  if (strcmp(tok->val, "ou") == 0)
    tok->type = OR;
  else if (strcmp(tok->val, "oue") == 0)
    tok->type = XOR;
  else if (strcmp(tok->val, "et") == 0)
    tok->type = AND;
  else if (strcmp(tok->val, "div") == 0)
    tok->type = DIV;
  else if (strcmp(tok->val, "mod") == 0)
    tok->type = MOD;
  else if (strcmp(tok->val, "non") == 0)
    tok->type = NOT;
  else
    tok->type = IDENTIFIER;
}

void getnum(struct token *tok)
{
  tok->type = NUM;
  unsigned i = -1;
  size_t len = 4;
  tok->val = malloc(len);
  do
  {
    ++i;
    ++curchar;
    tok->val[i] = getc(fin);
    if (i + 1 >= len)
      tok->val = realloc(tok->val, 2 * len);
  }
  while (isdigit(tok->val[i]));
  ungetc(tok->val[i], fin);
  --curchar;
  tok->val[i] = '\0';
}

void getop(struct token *tok)
{
  char c = getc(fin);
  ++curchar;
  tok->val = calloc(3, sizeof(char));
  tok->val[0] = c;
  char lookahead;
  switch (c)
  {
    case '+': tok->type = PLUS; break;
    case '-': tok->type = MINUS; break;
    case '*': tok->type = STAR; break;
    case '=': tok->type = EQ; break;
    case '(': tok->type = LPAREN; break;
    case ')': tok->type = RPAREN; break;
    case ',': tok->type = COMMA; break;
    case '^': tok->type = DEREF; break;
    case '[': tok->type = LSQBRACKET; break;
    case ']': tok->type = RSQBRACKET; break;
    case '<':
      ++curchar;
      lookahead = getc(fin);
      tok->val[1] = lookahead;
      switch (lookahead)
      {
        case '-': tok->type = ASSIGN; break;
        case '=': tok->type = LE; break;
        case '>': tok->type = NEQ; break;
        default:
          tok->val[1] = '\0';
          tok->type = LT;
          ungetc(lookahead, fin);
          --curchar;
          break;
      }
      break;
    case '>':
      ++curchar;
      lookahead = getc(fin);
      tok->val[1] = lookahead;
      switch (lookahead = getc(fin))
      {
        case '=': tok->type = GE; break;
        default:
          tok->type = GT;
          ungetc(lookahead, fin);
          --curchar;
          break;
      }
      break;
    default:
      tok->type = UNRECOGNIZED;
      break;
  }
}

struct token *gettok()
{
  struct token *tok = malloc(sizeof(struct token));
  tok->pos = malloc(sizeof(struct pos));
  char c = nextnonblank();
  tok->pos->line = curline;
  tok->pos->charstart = curchar;
  ungetc(c, fin);
  if (isdigit(c) || c == '.')
    getnum(tok);
  else if (isalnum(c))
    getalnum(tok);
  else if (c == EOF)
  {
    tok->type = ENDOFFILE;
    tok->val = "EOF";
  }
  else
    getop(tok);
  tok->pos->charend = curchar;
  return tok;
}
