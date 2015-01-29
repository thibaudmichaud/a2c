%debug
%error-verbose
%locations

%code top
{
#include <stdlib.h>
#include "grammar.h"

struct expr *expression;

}

%code provides
{

int yylex();
void yyerror (const char* msg);

}

%token PLUS "+" MINUS "-"
       MUL "*" DIV "/"
       LPAREN "(" RPAREN ")"
       EOL "\n"
%token END 0

%union
{
  struct expr *expression;
};

%token <expression> INT
%token <expression> REAL
%type <expression> exp

%left PLUS MINUS
%left MUL DIV

%%

input:
 %empty
| input line
;

line:
 exp END { expression = $1; }
| error "\n"
;

exp:
 exp "+" exp  { $$ = binopexpr($1, PLUS, $3); }
| exp "-" exp  { $$ = binopexpr($1, MINUS, $3); }
| exp "*" exp  { $$ = binopexpr($1, MUL, $3); }
| exp "/" exp  { $$ = binopexpr($1, DIV, $3); }
| INT     { $$ = $1; }
| REAL    { $$ = $1; }
| "(" exp ")"  { $$ = $2; }
| "+" exp      { $$ = $2; }
| "-" exp      { $$ = unopexpr(MINUS, $2); }
;

%%

void
yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}

int main(void)
{
  expression = malloc(sizeof(struct expr));
  yyparse();
}
