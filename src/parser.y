%debug
%error-verbose
%define api.pure full

%destructor {; free($$); } <str>

%code top
{
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern struct prog *prog;
extern FILE *yyin;
extern int syntax_error;
int yylineno;
char *srcfilename;
}


%code requires
{
#include "grammar.h"
extern FILE *yyin;
}

%code provides
{
  #define YY_DECL enum yytokentype yylex(YYSTYPE *yylval)
  YY_DECL;
  void yyerror (const char* msg);
  char* get_line(FILE *f, int line);
}

%union
{
  struct expr *expression;
  struct instruction *instruction;
  instructionlist_t instructions;
  caseblocklist_t caseblocklist;
  struct algo *algo;
  exprlist_t exprlist;
  identlist_t identlist;
  struct single_var_decl *single_var_decl;
  vardecllist_t var_decl;
  struct declarations *decls;
  struct assignment *assignment;
  struct entry_point *entry_point;
  struct prog *prog;
  struct local_param *lp;
  struct global_param *gp;
  struct param_decl *param_decl;
  struct const_decl *const_decl;
  struct type_decl *type_decl;
  struct type_def *type_def;
  struct enum_def *enum_def;
  struct val *val;
  constdecllist_t const_decls;
  typedecllist_t type_decls;
  intlist_t intlist;
  algolist_t algolist;
  char *str;
  int integer;
};

%type <algo> algo
%type <algo> fun
%type <algo> proc
%type <algolist> algolist
%type <assignment> assign
%type <const_decls> const_decls
%type <const_decls> const_decl_list
%type <const_decl> const_decl
%type <caseblocklist> caseblocklist
%type <decls> decls
%type <entry_point> entry_point
%type <expression> exp
%type <exprlist> explist nonempty_explist
%type <var_decl> gp_decl
%type <identlist> identlist
%type <instruction> instruction
%type <instructions> instructions
%type <intlist> dims
%type <var_decl> lp_decl
%type <param_decl> param_decl
%type <prog> prog
%type <single_var_decl> single_var_decl
%type <type_decl> type_decl
%type <type_decls> type_decl_list
%type <type_decls> type_decls
%type <type_def> array_def
%type <type_def> enum_def
%type <type_def> pointer_def
%type <type_def> record_def
%type <type_def> type_def
%type <val> val
%type <var_decl> var_decl var_decl2

/* ################# */
/* TOKEN DECLARATION */
/* ################# */

%token ALGORITHM "algorithme"
%token PROCEDURE "procedure"
%token FUNCTION "fonction"
%token RETURN "retourne"
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
%token PARAMETERS "parametres"
%token LOCAL "locaux"
%token GLOBAL "globaux"
%token <str> IDENT
%token <str> STRING
%token VARIABLES "variables"
%token TYPES "types"
%token CROSS "x"
%token RECORD "enregistrement"
%token <val> CHAR
%token CONST "constantes"
%token OTHERWISE

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
%token <integer> INT
%token <val> REAL
%token TRUE
%token FALSE

/* priority */
%right ASSIGN
%precedence IDENT WHILE ":"
%left "=" "<>"
%left  "<" ">" "<=" ">="
%left PLUS MINUS OR XOR
%left STAR SLASH DIV AND MOD
%right NO "^"
%precedence "[" "]" "(" ")"

%%

prog:
 algolist
 entry_point _EOL { prog = make_prog($1, $2); }

entry_point:
 var_decl
 "debut" _EOL
 instructions
 "fin" { $$ = make_entry_point($1, $4); }

algolist:
  { $$ = empty_algolist(); }
| algolist algo _EOL { $$ = $1; list_push_back($$, $2); }

algo: 
    proc | fun

proc:
 "algorithme" "procedure" IDENT _EOL
 decls
 "debut" _EOL
   instructions
 "fin" "algorithme" "procedure" IDENT
 { $$ = algo($3, NULL, $5, $8); free($12); }
/* NOTE: $12 will be needed for the semantic analysis phase but for now the
free is here to prevent valgrind from reporting the error */

fun:
 "algorithme" "fonction" IDENT ":" IDENT _EOL
 decls
 "debut" _EOL
   instructions
 "fin" "algorithme" "fonction" IDENT
 { $$ = algo($3, $5, $7, $10); free($14); }

decls:
 param_decl type_decls var_decl { $$ = make_declarations($1, empty_constdecllist(), $2, $3); }
| param_decl var_decl type_decls { $$ = make_declarations($1, empty_constdecllist(), $3, $2); }
| param_decl var_decl { $$ = make_declarations($1, empty_constdecllist(), empty_typedecllist(), $2); }
| param_decl type_decls { $$ = make_declarations($1, empty_constdecllist(), $2, empty_vardecllist()); }
| param_decl { $$ = make_declarations($1, empty_constdecllist(), empty_typedecllist(), empty_vardecllist()); }
| param_decl const_decls type_decls var_decl { $$ = make_declarations($1, $2, $3, $4); }
| param_decl type_decls const_decls var_decl { $$ = make_declarations($1, $3, $2, $4); }
| param_decl type_decls var_decl const_decls { $$ = make_declarations($1, $4, $2, $3); }
| param_decl const_decls var_decl type_decls { $$ = make_declarations($1, $2, $4, $3); }
| param_decl var_decl const_decls type_decls { $$ = make_declarations($1, $3, $4, $2); }
| param_decl var_decl type_decls const_decls { $$ = make_declarations($1, $4, $3, $2); }
| param_decl const_decls var_decl { $$ = make_declarations($1, $2, empty_typedecllist(), $3); }
| param_decl var_decl const_decls { $$ = make_declarations($1, $3, empty_typedecllist(), $2); }
| param_decl const_decls type_decls { $$ = make_declarations($1, $2, $3, empty_vardecllist()); }
| param_decl type_decls const_decls { $$ = make_declarations($1, $3, $2, empty_vardecllist()); }
| param_decl const_decls { $$ = make_declarations($1, $2, empty_typedecllist(), empty_vardecllist()); }

const_decls:
  "constantes" _EOL const_decl_list { $$ = $3; }

const_decl_list:
 const_decl { $$ = empty_constdecllist(); list_push_back($$, $1); }
| const_decl_list const_decl { $$ = $1; list_push_back($$, $2); }

const_decl:
 IDENT IDENT "=" val _EOL { $$ = make_constdecl($1, $2, $4); }

type_decls:
 "types" _EOL type_decl_list { $$ = $3; }

type_decl_list:
 type_decl { $$ = empty_typedecllist(); list_push_back($$, $1); }
| type_decl_list type_decl { $$ = $1; list_push_back($$, $2); }

type_decl:
 IDENT "=" type_def _EOL { $$ = make_type_decl($1, $3); }

type_def:
 enum_def { $$ = $1; }
| array_def { $$ = $1; }
| record_def { $$ = $1; }
| pointer_def { $$ = $1; }

enum_def:
 "(" identlist ")" { $$ = make_enum_def($2); }

array_def:
 dims IDENT { $$ = make_array_def($1, $2); }

record_def:
 "enregistrement" _EOL var_decl2 "fin" "enregistrement" IDENT { $$ = make_record($3, $6); }

pointer_def:
 "^" IDENT { $$ = make_pointer_def($2); }

dims:
 INT { $$ = empty_intlist(); list_push_back($$, $1); }
| dims "x" INT { $$ = $1; list_push_back($$, $3); }

param_decl:
 lp_decl gp_decl { $$ = make_param_decl(true, $1, $2); }
| gp_decl lp_decl { $$ = make_param_decl(false, $2, $1); }
| lp_decl { $$ = make_param_decl(true, $1, empty_vardecllist()); }
| gp_decl { $$ = make_param_decl(true, empty_vardecllist(), $1); }
| { $$ = make_param_decl(true, empty_vardecllist(), empty_vardecllist()); }

lp_decl:
 "parametres" "locaux" _EOL
 var_decl2 { $$ = $4; }

gp_decl:
 "parametres" "globaux" _EOL
 var_decl2 { $$ = $4; }

var_decl:
 "variables" _EOL
 var_decl2 { $$ = $3; }

var_decl2:
 { $$ = empty_var_decl(); }
| var_decl2 single_var_decl { $$ = $1; list_push_back($$, $2); }

single_var_decl:
 IDENT identlist _EOL { $$ = single_var_decl($1, $2); }

caseblocklist:
 nonempty_explist ":" instructions { $$ = empty_caseblocklist(); list_push_front($$,make_block($1,$3));}
| nonempty_explist ":" instructions caseblocklist {$$ = $4; list_push_front($$, make_block($1,$3));}

identlist:
IDENT { $$ = empty_identlist(); list_push_back($$, $1); }
| identlist "," IDENT { $$ = $1; list_push_back($$, $3); }

instructions: { list_init($$); }
| instructions error  
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
| IDENT "(" explist ")" _EOL { $$ = funcallinstr($1, $3); $$->lineno = yylineno; }
| IF exp THEN _EOL
    instructions
  END IF _EOL { $$ = ifthenelseblock($2, $5, empty_instructionlist()); }
| IF exp THEN _EOL
    instructions
  ELSE _EOL
    instructions
  END IF _EOL { $$ = ifthenelseblock($2, $5, $8); }
| SWITCH exp DO _EOL
    caseblocklist
  END SWITCH _EOL { $$ = switchblock($2, $5, empty_instructionlist());}
| SWITCH exp DO _EOL
    caseblocklist
  OTHERWISE instructions
  END SWITCH _EOL { $$ = switchblock($2, $5, $7);}
| "retourne" exp _EOL { $$ = return_stmt($2); }
| "retourne" _EOL { $$ = return_stmt(NULL); }

assign:
 exp "<-" exp { $$ = assign($1, $3); }

exp:
 val { $$ = expr_from_val($1); $$->lineno = yylineno; }
| IDENT   { $$ = identexpr($1); $$->lineno = yylineno; }
| exp "+" exp  { $$ = binopexpr($1, PLUS, $3); $$->lineno = $1->lineno; }
| exp "-" exp  { $$ = binopexpr($1, MINUS, $3); $$->lineno = $1->lineno; }
| exp "ou" exp  { $$ = binopexpr($1, OR, $3); $$->lineno = $1->lineno; }
| exp "oue" exp  { $$ = binopexpr($1, XOR, $3); $$->lineno = $1->lineno; }
| exp "*" exp  { $$ = binopexpr($1, STAR, $3); $$->lineno = $1->lineno; }
| exp "/" exp  { $$ = binopexpr($1, SLASH, $3); $$->lineno = $1->lineno; }
| exp "div" exp  { $$ = binopexpr($1, DIV, $3); $$->lineno = $1->lineno; }
| exp "et" exp  { $$ = binopexpr($1, AND, $3); $$->lineno = $1->lineno; }
| exp "mod" exp  { $$ = binopexpr($1, MOD, $3); $$->lineno = $1->lineno; }
| exp "=" exp  { $$ = binopexpr($1, EQ, $3); $$->lineno = $1->lineno; }
| exp "<=" exp  { $$ = binopexpr($1, LE, $3); $$->lineno = $1->lineno; }
| exp ">=" exp  { $$ = binopexpr($1, GE, $3); $$->lineno = $1->lineno; }
| exp "<" exp  { $$ = binopexpr($1, LT, $3); $$->lineno = $1->lineno; }
| exp ">" exp  { $$ = binopexpr($1, GT, $3); $$->lineno = $1->lineno; }
| exp "<>" exp  { $$ = binopexpr($1, NEQ, $3); $$->lineno = $1->lineno; }
| "(" exp ")"  { $$ = $2; $$->lineno = $2->lineno; }
| "non" exp  { $$ = unopexpr(NO, $2); $$->lineno = $2->lineno; }
| "+" exp      { $$ = $2; $$->lineno = $2->lineno; }
| "-" exp      { $$ = unopexpr(MINUS, $2); $$->lineno = $2->lineno; }
| IDENT "(" explist ")" { $$ = funcallexpr($1, $3); $$->lineno = yylineno; }
| "^" exp { $$ = derefexpr($2); $$->lineno = $2->lineno; }
| exp "[" nonempty_explist "]" { $$ = arrayexpr($1, $3); $$->lineno = $1->lineno; }

explist:
          { $$ = empty_exprlist(); }
| nonempty_explist { $$ = $1; }

val:
 INT     { $$ = intval($1); }
| REAL    { $$ = $1; }
| STRING { $$ = strval($1); }
| TRUE { $$ = boolval(true); }
| FALSE { $$ = boolval(false); }
| CHAR { $$ = $1; }

nonempty_explist:
 exp     { $$ = empty_exprlist(); list_push_back($$, $1); }
| explist "," exp { $$ = $1; list_push_back($$, $3); }

%%

void
yyerror (const char* msg)
{
  syntax_error = 1;
  fprintf (stderr, "%s:%d: %s\n", srcfilename ,yylineno, msg);
}



