%debug
%error-verbose
%locations
%define api.pure full

%code top
{
#include <stdlib.h>
#include "grammar.h"
#include <stdio.h>

extern struct algo *algorithm;
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
  struct block *instructions;
  struct algo *algo;
  char *str;
};

%type <expression> exp
%type <algo> algo
%type <instructions> instructions
%type <instruction> instruction
%type <instruction> assign

/* ################## */
/* TOKEN DECLARATION */
/* ################# */

%token ALGORITHM "algorithme"
%token PROCEDURE "procedure"
%token START "debut"
%token STOP "fin"
%token <str> IDENT

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
%token _EOF 0


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

algo:
 "algorithme" "procedure" IDENT
 "debut"
   instructions
 "fin" "algorithme" "procedure" IDENT
 { algorithm = algo($3, *($5)); }

instructions:
  { $$ = malloc(sizeof(struct block)); list_init(($$)->list); }
| instructions instruction { list_push_back(($1)->list, $2); $$ = $1; }

instruction:
 assign            { $$ = $1; }

assign:
 exp "<-" exp    { $$ = assign($1, $3); }

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
| IDENT   { $$ = identexpr($1); }
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
