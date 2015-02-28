%debug
%error-verbose
%locations
%define api.pure full

%code top
{
#include <stdlib.h>
#include "grammar.h"
#include <stdio.h>
#include <stdbool.h>

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
  struct exprlist *exprlist;
  struct identlist *identlist;
  struct single_var_decl *single_var_decl;
  struct var_decl *var_decl;
  struct declarations *decls;
  char *str;
};

%type <expression> exp
%type <algo> algo
%type <instructions> instructions
%type <instruction> instruction
%type <instruction> assign
%type <exprlist> explist
%type <identlist> identlist
%type <single_var_decl> single_var_decl
%type <var_decl> var_decl
%type <decls> decls

/* ################## */
/* TOKEN DECLARATION */
/* ################# */

%token ALGORITHM "algorithme"
%token PROCEDURE "procedure"
%token START "debut"
%token END "fin"
%token ASLONG AS DO
%token WHILE
%token DEREF "^"
%token <str> IDENT
%token <str> STRING

/* operands */
%token PLUS "+" MINUS "-"
       STAR "*" SLASH "/"
       DIV "div"
       MOD "mod"
       EQ "=" NEQ "<>"
       LPAREN "(" RPAREN ")"
       LBRACKET "[" RBRACKET "]"
       EOL "\n"
%token AND "et" OR "ou" XOR "oue"
       NO "non"
%token ASSIGN "<-"
%token COMMA ","
%token _EOF 0


/* expressions */
%token <expression> INT
%token <expression> REAL
%token <expression> TRUE
%token <expression> FALSE

/* priority */
%left PLUS MINUS OR XOR
%left STAR SLASH DIV AND MOD
%right NO DEREF

%%

algo:
 "algorithme" "procedure" IDENT
 decls
 "debut"
   instructions
 "fin" "algorithme" "procedure" IDENT
 { algorithm = algo($3, $4, $6); free($10); }
/* NOTE: $10 will be needed for the semantic analysis phase but for now the
free is here to prevent valgrind from reporting the error */

decls:
 var_decl { $$ = malloc(sizeof(struct declarations)); $$->var_decl = $1; }

var_decl:
 { $$ = empty_var_decl(); }
| var_decl single_var_decl { $$ = $1; list_push_back($$->decls, $2); }

single_var_decl:
 IDENT identlist { $$ = single_var_decl($1, $2); }

identlist:
IDENT { $$ = empty_identlist(); list_push_back($$->list, $1); }
| identlist "," IDENT { $$ = $1; list_push_back($$->list, $3); }

instructions:
  { $$ = malloc(sizeof(struct block)); list_init(($$)->list); }
| instructions instruction { list_push_back(($1)->list, $2); $$ = $1; }

instruction:
 assign            { $$ = $1; }
| ASLONG AS exp DO instructions END ASLONG AS { $$ = whileblock($3, $5); }
| DO instructions WHILE exp { $$ = dowhileblock($2, $4); }

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
| exp "=" exp  { $$ = binopexpr($1, EQ, $3); }
| exp "<>" exp  { $$ = binopexpr($1, NEQ, $3); }
| INT     { $$ = $1; }
| REAL    { $$ = $1; }
| IDENT   { $$ = identexpr($1); }
| STRING { $$ = stringexpr($1); }
| TRUE { $$ = boolexpr(true); }
| FALSE { $$ = boolexpr(false); }
| "(" exp ")"  { $$ = $2; }
| "+" exp      { $$ = $2; }
| "-" exp      { $$ = unopexpr(MINUS, $2); }
| IDENT "(" explist ")" { $$ = funcallexpr($1, $3); }
| "^" exp { $$ = derefexpr($2); }
| exp "[" exp "]" { $$ = arrayexpr($1, $3); }

explist:
          { $$ = empty_exprlist(); }
| exp     { $$ = empty_exprlist(); list_push_back(($$)->list, $1); }
| explist "," exp { $$ = $1; list_push_back(($$)->list, $3); }
;

%%

void
yyerror (YYLTYPE *yylloc, const char* msg)
{
  YY_LOCATION_PRINT(stderr, *yylloc);
  fprintf (stderr, ": %s\n", msg);
}
