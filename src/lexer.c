#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"
#include "a2c.h"
#include <assert.h>
#include "error.h"

static int curline = 1;
static int curchar = 0;

char nextnonblank()
{
  char c;
  while (true)
  {
    switch (c = getc(fin))
    {
      case ' ':
      case '\t':
        break;
      case '\n':
        curchar = 0;
        do ++curline; while ((c = getc(fin)) == '\n');
        ungetc(c, fin);
        return '\n';
      default:
        return c;
    }
    ++curchar;
  }
}

char *getalnumstr(void)
{
  unsigned i = -1;
  size_t len = 8;
  char *str = malloc(len);

  do
  {
    ++i;
    str[i] = getc(fin);
    if (i + 1 >= len)
    {
      len *= 2;
      str = realloc(str, 2 * len);
    }
  }
  while (isalnum(str[i]) || str[i] == '\'' || str[i] == '_');
  curchar += i;
  ungetc(str[i], fin);
  str[i] = '\0';
  return str;
}

void getalnum(struct token *tok)
{
  tok->val = getalnumstr();

  if (strcmp(tok->val, "algorithme") == 0)
    tok->type = ALGORITHM;
  else if (strcmp(tok->val, "debut") == 0)
    tok->type = BEGIN;
  else if (strcmp(tok->val, "types") == 0)
    tok->type = TYPES;
  else if (strcmp(tok->val, "constantes") == 0)
    tok->type = CONST;
  else if (strcmp(tok->val, "ou") == 0)
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
  else if (strcmp(tok->val, "tant") == 0)
    tok->type = WHILE;
  else if (strcmp(tok->val, "que") == 0)
    tok->type = SO;
  else if (strcmp(tok->val, "faire") == 0)
    tok->type = DO;
  else if (strcmp(tok->val, "fin") == 0)
    tok->type = END;
  else if (strcmp(tok->val, "retourne") == 0)
    tok->type = RETURN;
  else if (strcmp(tok->val, "pour") == 0)
    tok->type = FOR;
  else if (strcmp(tok->val, "jusqu'a") == 0)
    tok->type = UNTIL;
  else if (strcmp(tok->val, "decroissant") == 0)
    tok->type = DECREASING;
  else if (strcmp(tok->val, "si") == 0)
    tok->type = IF;
  else if (strcmp(tok->val, "sinon") == 0)
    tok->type = ELSE;
  else if (strcmp(tok->val, "alors") == 0)
    tok->type = THEN;
  else if (strcmp(tok->val, "selon") == 0)
    tok->type = SWITCH;
  else if (strcmp(tok->val, "autrement") == 0)
    tok->type = OTHERWISE;
  else if (strcmp(tok->val, "procedure") == 0)
    tok->type = PROCEDURE;
  else if (strcmp(tok->val, "variables") == 0)
    tok->type = VARIABLES;
  else if (strcmp(tok->val, "fonction") == 0)
    tok->type = FUNCTION;
  else if (strcmp(tok->val, "vrai") == 0)
    tok->type = TRUE;
  else if (strcmp(tok->val, "faux") == 0)
    tok->type = FALSE;
  else if (strcmp(tok->val, "parametres") == 0)
    tok->type = PARAM;
  else if (strcmp(tok->val, "locaux") == 0)
    tok->type = LOCAL;
  else if (strcmp(tok->val, "globaux") == 0)
    tok->type = GLOBAL;
  else if (strcmp(tok->val, "enregistrement") == 0)
    tok->type = RECORD;
  else if (strcmp(tok->val, "x") == 0)
    tok->type = X;
  else
    tok->type = IDENTIFIER;
}

void getnum(struct token *tok)
{
  tok->type = INT;
  unsigned i = -1;
  size_t len = 4;
  tok->val = malloc(len);
  do
  {
    ++i;
    ++curchar;
    tok->val[i] = getc(fin);
    if (tok->val[i] == '.')
      tok->type = REAL;
    if (i + 1 >= len)
      tok->val = realloc(tok->val, 2 * len);
  }
  while (isdigit(tok->val[i]) || tok->val[i] == '.');
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
    case ':': tok->type = COLON; break;
    case '.': tok->type = DOT; break;
    case '/':
      ++curchar;
      lookahead = getc(fin);
      tok->val[1] = lookahead;
      if (lookahead == '*')
      {
        lookahead = getc(fin);
        ++curchar;
        do
        {
          while (lookahead != '*')
          {
            lookahead = getc(fin);
            if (lookahead == '\n')
            {
              curchar = 0;
              ++curline;
            }
            ++curchar;
          }
          lookahead = getc(fin);
          ++curchar;
        } while (lookahead != '/');
        *tok = *gettok();
        break;
      }
      else
      {
        tok->val[1] = '\0';
        tok->type = SLASH;
        ungetc(lookahead, fin);
        --curchar;
        break;
      }
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

void getstring(struct token *tok)
{
  tok->type = STRING;
  ++curchar;
  char c = getc(fin);
  assert(c == '"');
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
  while (tok->val[i] != '"');
  tok->val[i] = '\0';
}

struct token *gettok()
{
  struct token *tok = malloc(sizeof(struct token));
  tok->pos = malloc(sizeof(struct pos));
  char c = nextnonblank();
  tok->pos->line = curline;
  tok->pos->charstart = curchar;
  if (c != '\n')
    ungetc(c, fin);
  if (isdigit(c))
    getnum(tok);
  else if (isalnum(c))
    getalnum(tok);
  else if (c == '\'')
  {
    getc(fin);
    c = getc(fin);
    tok->type = CHAR;
    tok->val = malloc(3);
    tok->val[0] = tok->val[2] = '\'';
    tok->val[1] = c;
    c = getc(fin);
    if (c != '\'')
    {
      tok->pos->len = 2;
      error(*tok->pos, "missing ' to end character");
    }
  }
  else if (c == '\"')
    getstring(tok);
  else if (c == EOF)
  {
    tok->type = ENDOFFILE;
    tok->val = "EOF";
  }
  else if (c == '\n')
  {
    tok->type = EOL;
    tok->val = "EOL";
  }
  else
    getop(tok);
  tok->pos->len = curchar - tok->pos->charstart;
  return tok;
}

void freetok(struct token *tok)
{
  if (tok->type != EOL && tok->type != ENDOFFILE)
    free(tok->val);
  free(tok->pos);
  free(tok);
}

char *describe(enum tokentype toktype)
{
  switch (toktype)
  {
    case ALGORITHM: return "algorithme";
    case AND: return "et";
    case ASSIGN: return "<-";
    case BEGIN: return "debut";
    case CHAR: return "char";
    case COLON: return ":";
    case COMMA: return ",";
    case CONST: return "constantes";
    case DECREASING: return "decroissant";
    case DEREF: return "";
    case DIV: return "div";
    case DO: return "faire";
    case ELSE: return "sinon";
    case END: return "fin";
    case ENDOFFILE: return "EOF";
    case EOL: return "EOL";
    case EQ: return "=";
    case FALSE: return "faux";
    case FOR: return "pour";
    case FUNCTION: return "fonction";
    case GE: return ">=";
    case GLOBAL: return "globaux";
    case GT: return ">";
    case IDENTIFIER: return "identifier";
    case IF: return "si";
    case INT: return "int";
    case LE: return "<";
    case LOCAL: return "locaux";
    case LPAREN: return "(";
    case LSQBRACKET: return "[";
    case LT: return "<";
    case MINUS: return "-";
    case MOD: return "mod";
    case NEQ: return "<>";
    case NOT: return "non";
    case NULLKW: return "NUL";
    case OR: return "ou";
    case OTHERWISE: return "autrement";
    case PARAM: return "parametres";
    case PLUS: return "+";
    case PROCEDURE: return "procedure";
    case REAL: return "real";
    case RECORD: return "enregistrement";
    case RETURN: return "retourne";
    case RPAREN: return ")";
    case RSQBRACKET: return "]";
    case SLASH: return "/";
    case SO: return "que";
    case STAR: return "*";
    case STRING: return "string";
    case SWITCH: return "selon";
    case THEN: return "alors";
    case TRUE: return "vrai";
    case TYPES: return "types";
    case UMINUS: return "-";
    case UNTIL: return "jusqu'a";
    case VARIABLES: return "variables";
    case WHILE: return "tant";
    case X: return "x";
    case XOR: return "oue";
    default: return "";
  }
}
