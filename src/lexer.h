#ifndef A2C_LEXER_H
# define A2C_LEXER_H

#include <stdio.h>

enum tokentype {
    AND,
    ASSIGN,
    COMMA,
    DEREF,
    DIV,
    ENDOFFILE,
    EQ,
    FOR,
    FUNCTION,
    GE,
    GT,
    IDENTIFIER,
    LE,
    LPAREN,
    LSQBRACKET,
    LT,
    MINUS,
    MOD,
    NEQ,
    NOT,
    NULLKW,
    NUM,
    OR,
    PLUS,
    PROCEDURE,
    RPAREN,
    RSQBRACKET,
    SLASH,
    STAR,
    TYPE,
    UMINUS,
    UNRECOGNIZED,
    WHILE,
    XOR,
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
void ungettok(void);

#endif
