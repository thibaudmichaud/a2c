#ifndef A2C_LEXER_H
# define A2C_LEXER_H

#include <stdio.h>

enum tokentype {
  ALGORITHM,
  AND,
  ASSIGN,
  BEGIN,
  CHAR,
  COLON,
  COMMA,
  CONST,
  DECREASING,
  DEREF,
  DIV,
  DO,
  DOT,
  ELSE,
  END,
  ENDOFFILE,
  EOL,
  EQ,
  FALSE,
  FOR,
  FUNCTION,
  GE,
  GLOBAL,
  GT,
  IDENTIFIER,
  IF,
  INT,
  LE,
  LOCAL,
  LPAREN,
  LSQBRACKET,
  LT,
  MINUS,
  MOD,
  NEQ,
  NOT,
  NULLKW,
  OR,
  OTHERWISE,
  PARAM,
  PLUS,
  PROCEDURE,
  REAL,
  RECORD,
  RETURN,
  RPAREN,
  RSQBRACKET,
  SLASH,
  SO,
  STAR,
  STRING,
  SWITCH,
  THEN,
  TRUE,
  TYPES,
  UMINUS,
  UNRECOGNIZED,
  UNTIL,
  VARIABLES,
  WHILE,
  X,
  XOR
};

struct pos
{
  unsigned line;
  unsigned charstart;
  unsigned len;
};

struct token {
    enum tokentype type;
    char *val;
    struct pos *pos;
};

char *describe(enum tokentype toktype);
struct token *gettok(void);
void freetok(struct token *tok);

#endif
