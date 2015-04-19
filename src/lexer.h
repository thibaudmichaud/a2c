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
  XOR
};

struct pos
{
  unsigned line;
  unsigned charstart;
  unsigned charend;
};

struct token {
    enum tokentype type;
    char *val;
    struct pos *pos;
};

char *toktypestr(enum tokentype toktype);
struct token *gettok(void);
void freetok(struct token *tok);

#endif
