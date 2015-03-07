%debug
%error-verbose
%define api.pure full

%code top
{
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern struct prog *prog;
extern FILE *yyin;
int yylineno;
}

%code requires
{
#include "grammar.h"
}

%code provides
{
  #define YY_DECL enum yytokentype yylex(YYSTYPE *yylval)
  YY_DECL;
  void yyerror (const char* msg);
}


%union
{
  struct expr *expression;
  struct instruction *instruction;
  instructionlist_t instructions;
  struct algo *algo;
  exprlist_t exprlist;
  identlist_t identlist;
  struct single_var_decl *single_var_decl;
  vardecllist_t var_decl;
  struct declarations *decls;
  struct assignment *assignment;
  struct entry_point *entry_point;
  struct prog *prog;
  char *str;
};

%type <expression> exp
%type <algo> algo
%type <instructions> instructions
%type <instruction> instruction
%type <exprlist> explist nonempty_explist
%type <identlist> identlist
%type <single_var_decl> single_var_decl
%type <var_decl> var_decl var_decl2
%type <decls> decls
%type <assignment> assign
%type <entry_point> entry_point
%type <prog> prog

/* ################# */
/* TOKEN DECLARATION */
/* ################# */

%token ALGORITHM "algorithme"
%token PROCEDURE "procedure"
%token START "debut"
%token END "fin"
%token ASLONG AS DO
%token FOR
%token DECREASING
%token UPTO
%token IF
%token THEN
%token ELSE
%token SWITCH
%token COLON ":"
%token DEREF "^"
%token <str> IDENT
%token <str> STRING
%token VARIABLES "variables"

/* operators */
%token PLUS "+" MINUS "-"
       STAR "*" SLASH "/"
       DIV "div"
       MOD "mod"
       LT "<" LE "<="
       GT ">" GE ">="
       EQ "=" NEQ "<>"
       LPAREN "(" RPAREN ")"
       LBRACKET "[" RBRACKET "]"
       EOL "\n"
%token AND "et" OR "ou" XOR "oue"
       NO "non"
%token ASSIGN "<-"
%token COMMA ","
%token _EOL
%token _EOF 0


/* expressions */
%token <expression> INT
%token <expression> REAL
%token <expression> TRUE
%token <expression> FALSE

/* priority */
%right ASSIGN
%precedence IDENT WHILE
%left "=" "<>"
%left PLUS MINUS OR XOR
%left STAR SLASH DIV AND MOD
%right NO DEREF
%precedence "[" "]" "(" ")"

%%

prog:
 algo _EOL
 entry_point _EOL { prog = make_prog($1, $3); }

entry_point:
 var_decl
 "debut" _EOL
 instructions
 "fin" { $$ = make_entry_point($1, $4); }

algo:
 "algorithme" "procedure" IDENT _EOL
 decls
 "debut" _EOL
   instructions
 "fin" "algorithme" "procedure" IDENT
 { $$ = algo($3, $5, $8); free($12); }
/* NOTE: $12 will be needed for the semantic analysis phase but for now the
free is here to prevent valgrind from reporting the error */

decls:
 var_decl { $$ = make_declarations(NULL, NULL, NULL, $1); }

var_decl:
 "variables" _EOL
 var_decl2 { $$ = $3; }

var_decl2:
 { $$ = empty_var_decl(); }
| var_decl2 single_var_decl { $$ = $1; list_push_back($$, $2); }

single_var_decl:
 IDENT identlist _EOL { $$ = single_var_decl($1, $2); }

identlist:
IDENT { $$ = empty_identlist(); list_push_back($$, $1); }
| identlist "," IDENT { $$ = $1; list_push_back($$, $3); }

instructions:
  { list_init($$); }
| instructions instruction { list_push_back(($1), $2); $$ = $1; }

instruction:
 assign _EOL      { $$ = assigninstr($1); }
| ASLONG AS exp DO _EOL
    instructions
  END ASLONG AS _EOL { $$ = whileblock($3, $6); }
| DO _EOL
    instructions
  ASLONG AS exp _EOL { $$ = dowhileblock($3, $6); }
| FOR assign UPTO exp DO _EOL
    instructions
  END FOR _EOL { $$ = forblock($2, $4, 0, $7); }
| FOR assign UPTO exp DECREASING DO _EOL
    instructions
  END FOR _EOL { $$ = forblock($2, $4, 1, $8); }
| IDENT "(" explist ")" _EOL { $$ = funcallinstr($1, $3); }
| IF exp THEN _EOL
    instructions
  END IF _EOL { ifthenelseblock($2, $5, empty_instructionlist()); }
| IF exp THEN _EOL
    instructions
  ELSE _EOL
    instructions
  END IF _EOL { $$ = ifthenelseblock($2, $5, $8); }

assign:
 exp "<-" exp { $$ = assign($1, $3); }

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
| exp "<=" exp  { $$ = binopexpr($1, LE, $3); }
| exp ">=" exp  { $$ = binopexpr($1, GE, $3); }
| exp "<" exp  { $$ = binopexpr($1, LT, $3); }
| exp ">" exp  { $$ = binopexpr($1, GT, $3); }
| exp "<>" exp  { $$ = binopexpr($1, NEQ, $3); }
| INT     { $$ = $1; }
| REAL    { $$ = $1; }
| IDENT   { $$ = identexpr($1); }
| STRING { $$ = stringexpr($1); }
| TRUE { $$ = boolexpr(true); }
| FALSE { $$ = boolexpr(false); }
| "(" exp ")"  { $$ = $2; }
| "non" exp  { $$ = unopexpr(NO, $2); }
| "+" exp      { $$ = $2; }
| "-" exp      { $$ = unopexpr(MINUS, $2); }
| IDENT "(" explist ")" { $$ = funcallexpr($1, $3); }
| "^" exp { $$ = derefexpr($2); }
| exp "[" nonempty_explist "]" { $$ = arrayexpr($1, $3); }

explist:
          { $$ = empty_exprlist(); }
| nonempty_explist { $$ = $1; }

nonempty_explist:
 exp     { $$ = empty_exprlist(); list_push_back($$, $1); }
| explist "," exp { $$ = $1; list_push_back($$, $3); }
;

%%

void
yyerror (const char* msg)
{
  fprintf (stderr, "error near line %d: %s\n", yylineno, msg);
}
