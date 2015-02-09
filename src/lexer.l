%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "grammar.h"

#define SAVE_INT (yylval->expression = intexpr(strtol(yytext, NULL, 10)))
#define SAVE_REAL (yylval->expression = realexpr(strtof(yytext, NULL)))
#define SAVE_STRING (yylval->str = strdup(yytext))
#define yyterminate() return _EOF

%}

%option noinput
%option noyywrap
%option nounput
%option yylineno
%option header-file="lexer.h"
%x COMMENT

%%

[ \t]                   ;
"\n"                    yylloc->first_line++;
"algorithme"            return ALGORITHM;
"alors"                 printf("keyword: %s\n", yytext);
"autrement"             printf("keyword: %s\n", yytext);
"constantes"            printf("keyword: %s\n", yytext);
"debut"                 return START;
"decroissant"           printf("keyword: %s\n", yytext);
"div"                   return DIV;
"enregistrement"        printf("keyword: %s\n", yytext);
"et"                    return AND;
"faire"                 printf("keyword: %s\n", yytext);
"faux"                  return FALSE;
"fin"                   return STOP;
"fonction"              printf("keyword: %s\n", yytext);
"globaux"               printf("keyword: %s\n", yytext);
"jusqu'a"               printf("keyword: %s\n", yytext);
"locaux"                printf("keyword: %s\n", yytext);
"mod"                   return MOD;
"non"                   return NOT;
"ou"                    return OR;
"oue"                   return XOR;
"parametres"            printf("keyword: %s\n", yytext);
"pour"                  printf("keyword: %s\n", yytext);
"procedure"             return PROCEDURE;
"que"                   printf("keyword: %s\n", yytext);
"selon"                 printf("keyword: %s\n", yytext);
"si"                    printf("keyword: %s\n", yytext);
"sinon"                 printf("keyword: %s\n", yytext);
"tant"                  printf("keyword: %s\n", yytext);
"types"                 printf("keyword: %s\n", yytext);
"variables"             printf("keyword: %s\n", yytext);
"vrai"                  return TRUE;
"/*"                    BEGIN(COMMENT);
<COMMENT>"*/"           BEGIN(INITIAL);
<COMMENT>(.|\n)         ;
<COMMENT><<EOF>>        printf("Unexpected file end (unterminated comment).\n");
"<-"                    return ASSIGN;
"^"                     printf("operator: %s\n", yytext);
"."                     printf("operator: %s\n", yytext);
","                     printf("operator: %s\n", yytext);
":"                     printf("operator: %s\n", yytext);
"<"                     printf("operator: %s\n", yytext);
">"                     printf("operator: %s\n", yytext);
"<="                    printf("operator: %s\n", yytext);
">="                    printf("operator: %s\n", yytext);
"<>"                    printf("operator: %s\n", yytext);
"="                     printf("operator: %s\n", yytext);
"+"                     return PLUS;
"-"                     return MINUS;
"*"                     return STAR;
"/"                     return SLASH;
"("                     return LPAREN;
")"                     return RPAREN;
"["                     printf("LBRACKET\n");
"]"                     printf("RBRACKET\n");
\"[^"]*\"               printf("STRING\n");
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_STRING; return IDENT;
[0-9]+                  SAVE_INT; return INT;
[0-9]+(\.[0-9]+)?       SAVE_REAL; return REAL;
.                       ;

%%
