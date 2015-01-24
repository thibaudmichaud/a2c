%{
#include <stdio.h>
#define YY_NO_INPUT
%}

%option noyywrap
%option nounput
%option yylineno

%%

[ \n\t]                 ;
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
"<-"                    printf("operator: %s\n", yytext);
"^"                     printf("operator: %s\n", yytext);
"."                     printf("operator: %s\n", yytext);
","                     printf("operator: %s\n", yytext);
":"                     printf("operator: %s\n", yytext);
"/*"                    printf("COMMENT_BEGIN\n");
"*/"                    printf("COMMENT_END\n");
"<"                     printf("operator: %s\n", yytext);
">"                     printf("operator: %s\n", yytext);
"<="                    printf("operator: %s\n", yytext);
">="                    printf("operator: %s\n", yytext);
"<>"                    printf("operator: %s\n", yytext);
"="                     printf("operator: %s\n", yytext);
"+"                     printf("operator: %s\n", yytext);
"-"                     printf("operator: %s\n", yytext);
"*"                     printf("operator: %s\n", yytext);
"/"                     printf("operator: %s\n", yytext);
"("                     printf("LPAREN");
")"                     printf("RPAREN");
"["                     printf("LBRACKET");
"]"                     printf("RBRACKET");
[a-zA-Z_][a-zA-Z0-9_]+  printf("identifier: %s\n", yytext);
[0-9]+                  printf("number: %s\n", yytext);
.                       ;

%%

int main(void)
{
  yylex();
}
