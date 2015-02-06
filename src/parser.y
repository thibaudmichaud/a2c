%debug
%error-verbose
%locations
%define api.pure full

%code top
{
#include <stdlib.h>
#include "grammar.h"
#include <stdio.h>

struct block instructions;
extern FILE *yyin;
}

%code provides
{
  #define YY_DECL enum yytokentype yylex(YYSTYPE *yylval, YYLTYPE *yylloc)
  YY_DECL;
  void yyerror (YYLTYPE* yylloc, const char* msg);
}


%union
{
  struct expr *expression;
  struct instruction *instruction;
  char *str;
};

%type <expression> exp
%type <instruction> instruction
%type <instruction> assign

/* ################## */
/* TOKEN DECLARATION */
/* ################# */

%token <expression> IDENT

/* operands */
%token PLUS "+" MINUS "-"
       STAR "*" SLASH "/"
       DIV "div"
       MOD "mod"
       LPAREN "(" RPAREN ")"
       EOL "\n"
%token AND "et" OR "ou" XOR "oue"
       NO "non"
%token ASSIGN "<-"
%token END 0


/* expressions */
%token <expression> INT
%token <expression> REAL
%token <expression> TRUE
%token <expression> FALSE

/* priority */
%left PLUS MINUS OR XOR
%left STAR SLASH DIV AND MOD
%right NO

%%

instructions: |
 instructions instruction { list_push_back(instructions.list, $2); }

instruction:
 assign            { $$ = $1; }

assign:
 IDENT "<-" exp    { $$ = assign($1, $3); }

exp:
 exp "+" exp  { $$ = binopexpr($1, PLUS, $3); }
| exp "-" exp  { $$ = binopexpr($1, MINUS, $3); }
| exp "ou" exp  { $$ = binopexpr($1, OR, $3); }
| exp "oue" exp  { $$ = binopexpr($1, XOR, $3); }
| exp "*" exp  { $$ = binopexpr($1, STAR, $3); }
| exp "/" exp  { $$ = binopexpr($1, SLASH, $3); }
| exp "div" exp  { $$ = binopexpr($1, DIV, $3); }
| exp "et" exp  { $$ = binopexpr($1, AND, $3); }
| exp "mod" exp  { $$ = binopexpr($1, MOD, $3); }
| exp "non" exp  { $$ = binopexpr($1, NO, $3); }
| INT     { $$ = $1; }
| REAL    { $$ = $1; }
| IDENT   { $$ = $1; }
| "(" exp ")"  { $$ = $2; }
| "+" exp      { $$ = $2; }
| "-" exp      { $$ = unopexpr(MINUS, $2); }

;

%%

void
yyerror (YYLTYPE *yylloc, const char* msg)
{
  YY_LOCATION_PRINT(stderr, *yylloc);
  fprintf (stderr, ": %s\n", msg);
}
