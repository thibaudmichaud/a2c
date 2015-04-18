#ifndef A2C_LEXER_H
# define A2C_LEXER_H

#include <stdio.h>

enum tokentype {
    AND,
    ASSIGN,
    COLON,
    COMMA,
    DECREASING,
    DEREF,
    DIV,
    DO,
    END,
    ENDOFFILE,
    ELSE,
    EOL,
    EQ,
    FOR,
    FUNCTION,
    GE,
    GT,
    IDENTIFIER,
    IF,
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
    OTHERWISE,
    PLUS,
    PROCEDURE,
    RETURN,
    RPAREN,
    RSQBRACKET,
    SLASH,
    STAR,
    SWITCH,
    THEN,
    TYPE,
    UMINUS,
    UNRECOGNIZED,
    UNTIL,
    WHILE,
    SO,
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
void ungettok(void);

#endif
