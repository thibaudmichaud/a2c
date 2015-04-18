#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"
#include "a2c.h"

static int curline = 0;
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
    ++curchar;
    str[i] = getc(fin);
    if (i + 1 >= len)
      str = realloc(str, 2 * len);
  }
  while (isalnum(str[i]) || str[i] == '\'');
  ungetc(str[i], fin);
  --curchar;
  str[i] = '\0';
  return str;
}

void getalnum(struct token *tok)
{
  tok->val = getalnumstr();

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
    case ':': tok->type = COLON; break;
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
  if (c != '\n')
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
  else if (c == '\n')
  {
    tok->type = EOL;
    tok->val = "EOL";
  }
  else
    getop(tok);
  tok->pos->charend = curchar;
  return tok;
}
