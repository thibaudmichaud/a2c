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
static int curchar = 1;

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
        curchar = 1;
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

# define KEYWORDS_LEN       33

static char* keywords[2][KEYWORDS_LEN] = {
  {"algorithme","debut","types","constantes","ou","oue","et","div","mod","non",
    "tant","que","faire","fin","retourne","pour","jusqu'a","decroissant","si","sinon","alors","selon",
    "autrement","procedure","variables","fonction","vrai","faux","parametres","locaux","globaux",
    "enregistrement","NUL"
  },
  {"algorithm","begin","types","constants","or","xor","and","div","mod","not",
    "while","","do","end","return","for","to","downto","if","else","then","caseof",
    "default","procedure","variables","function","true","false","parameters","local","global",
    "","NUL"}};

void getalnum(struct token *tok)
{
  tok->val = getalnumstr();

  if (strcmp(tok->val, keywords[current_lang][0]) == 0)
    tok->type = ALGORITHM;
  else if (strcmp(tok->val, keywords[current_lang][1]) == 0)
    tok->type = BEGIN;
  else if (strcmp(tok->val, keywords[current_lang][2]) == 0)
    tok->type = TYPES;
  else if (strcmp(tok->val, keywords[current_lang][3]) == 0)
    tok->type = CONST;
  else if (strcmp(tok->val, keywords[current_lang][4]) == 0)
    tok->type = OR;
  else if (strcmp(tok->val, keywords[current_lang][5]) == 0)
    tok->type = XOR;
  else if (strcmp(tok->val, keywords[current_lang][6]) == 0)
    tok->type = AND;
  else if (strcmp(tok->val, keywords[current_lang][7]) == 0)
    tok->type = DIV;
  else if (strcmp(tok->val, keywords[current_lang][8]) == 0)
    tok->type = MOD;
  else if (strcmp(tok->val, keywords[current_lang][9]) == 0)
    tok->type = NOT;
  else if (strcmp(tok->val, keywords[current_lang][10]) == 0)
    tok->type = WHILE;
  else if (strcmp(tok->val, keywords[current_lang][11]) == 0)
    tok->type = SO;
  else if (strcmp(tok->val, keywords[current_lang][12]) == 0)
    tok->type = DO;
  else if (strcmp(tok->val, keywords[current_lang][13]) == 0)
    tok->type = END;
  else if (strcmp(tok->val, keywords[current_lang][14]) == 0)
    tok->type = RETURN;
  else if (strcmp(tok->val, keywords[current_lang][15]) == 0)
    tok->type = FOR;
  else if (strcmp(tok->val, keywords[current_lang][16]) == 0)
    tok->type = UNTIL;
  else if (strcmp(tok->val, keywords[current_lang][17]) == 0)
    tok->type = DECREASING;
  else if (strcmp(tok->val, keywords[current_lang][18]) == 0)
    tok->type = IF;
  else if (strcmp(tok->val, keywords[current_lang][19]) == 0)
    tok->type = ELSE;
  else if (strcmp(tok->val, keywords[current_lang][20]) == 0)
    tok->type = THEN;
  else if (strcmp(tok->val, keywords[current_lang][21]) == 0)
    tok->type = SWITCH;
  else if (strcmp(tok->val, keywords[current_lang][22]) == 0)
    tok->type = OTHERWISE;
  else if (strcmp(tok->val, keywords[current_lang][23]) == 0)
    tok->type = PROCEDURE;
  else if (strcmp(tok->val, keywords[current_lang][24]) == 0)
    tok->type = VARIABLES;
  else if (strcmp(tok->val, keywords[current_lang][25]) == 0)
    tok->type = FUNCTION;
  else if (strcmp(tok->val, keywords[current_lang][26]) == 0)
    tok->type = TRUE;
  else if (strcmp(tok->val, keywords[current_lang][27]) == 0)
    tok->type = FALSE;
  else if (strcmp(tok->val, keywords[current_lang][28]) == 0)
    tok->type = PARAM;
  else if (strcmp(tok->val, keywords[current_lang][29]) == 0)
    tok->type = LOCAL;
  else if (strcmp(tok->val, keywords[current_lang][30]) == 0)
    tok->type = GLOBAL;
  else if (strcmp(tok->val, keywords[current_lang][31]) == 0)
    tok->type = RECORD;
  else if (strcmp(tok->val, keywords[current_lang][32]) == 0)
    tok->type = NULLKW;
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
              curchar = 1;
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
    case ALGORITHM: return keywords[current_lang][0];
    case AND: return keywords[current_lang][6];
    case ASSIGN: return "<-";
    case BEGIN: return keywords[current_lang][2];
    case CHAR: return "char";
    case COLON: return ":";
    case COMMA: return ",";
    case CONST: return keywords[current_lang][4];
    case DECREASING: return keywords[current_lang][18];
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
