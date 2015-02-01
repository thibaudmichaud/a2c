%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "grammar.h"

#define SAVE_INT (yylval->expression = intexpr(strtol(yytext, NULL, 10)))
#define SAVE_REAL (yylval->expression = realexpr(strtof(yytext, NULL)))
#define yyterminate() return END

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
"algorithme"            printf("keyword: %s\n", yytext);
"div"                   printf("keyword: %s\n", yytext);
"globaux"               printf("keyword: %s\n", yytext);
"parametres"            printf("keyword: %s\n", yytext);
"tant"                  printf("keyword: %s\n", yytext);
"que"                   printf("keyword: %s\n", yytext);
"alors"                 printf("keyword: %s\n", yytext);
"enregistrement"        printf("keyword: %s\n", yytext);
"jusqu'a"               printf("keyword: %s\n", yytext);
"procedure"             printf("keyword: %s\n", yytext);
"types"                 printf("keyword: %s\n", yytext);
"autrement"             printf("keyword: %s\n", yytext);
"et"                    printf("keyword: %s\n", yytext);
"locaux"                printf("keyword: %s\n", yytext);
"pour"                  printf("keyword: %s\n", yytext);
"variables"             printf("keyword: %s\n", yytext);
"constantes"            printf("keyword: %s\n", yytext);
"faire"                 printf("keyword: %s\n", yytext);
"mod"                   printf("keyword: %s\n", yytext);
"selon"                 printf("keyword: %s\n", yytext);
"debut"                 printf("keyword: %s\n", yytext);
"fin"                   printf("keyword: %s\n", yytext);
"non"                   printf("keyword: %s\n", yytext);
"si"                    printf("keyword: %s\n", yytext);
"decroissant"           printf("keyword: %s\n", yytext);
"fonction"              printf("keyword: %s\n", yytext);
"ou"                    printf("keyword: %s\n", yytext);
"sinon"                 printf("keyword: %s\n", yytext);
"/*"                    BEGIN(COMMENT);
<COMMENT>"*/"           BEGIN(INITIAL);
<COMMENT>(.|\n)         ;
<COMMENT><<EOF>>        printf("Unexpected file end (unterminated comment).\n");
"<-"                    printf("operator: %s\n", yytext);
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
"*"                     return MUL;
"/"                     return DIV;
"("                     return LPAREN;
")"                     return RPAREN;
"["                     printf("LBRACKET\n");
"]"                     printf("RBRACKET\n");
\"[^"]*\"               printf("STRING\n");
[a-zA-Z_][a-zA-Z0-9_]*  printf("identifier: %s\n", yytext);
[0-9]+                  SAVE_INT; return INT;
[0-9]+(\.[0-9]+)?       SAVE_REAL; return REAL;
.                       ;

%%
