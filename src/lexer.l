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

"\n"                    yylloc->first_line++;
"algorithme"            return ALGORITHM;
"alors"                 printf("keyword: %s\n", yytext);
"autrement"             printf("keyword: %s\n", yytext);
"constantes"            printf("keyword: %s\n", yytext);
"debut"                 return START;
"decroissant"           return DECREASING;
"div"                   return DIV;
"enregistrement"        printf("keyword: %s\n", yytext);
"et"                    return AND;
"faire"                 return DO;
"faux"                  return FALSE;
"fin"                   return END;
"fonction"              printf("keyword: %s\n", yytext);
"globaux"               printf("keyword: %s\n", yytext);
"jusqu'a"               return UPTO;
"locaux"                printf("keyword: %s\n", yytext);
"mod"                   return MOD;
"non"                   return NO;
"ou"                    return OR;
"oue"                   return XOR;
"parametres"            printf("keyword: %s\n", yytext);
"pour"                  return FOR;
"procedure"             return PROCEDURE;
"selon"                 printf("keyword: %s\n", yytext);
"si"                    printf("keyword: %s\n", yytext);
"sinon"                 printf("keyword: %s\n", yytext);
"tant"                  return ASLONG;
"que"                   return AS;
"types"                 printf("keyword: %s\n", yytext);
"variables"             printf("keyword: %s\n", yytext);
"vrai"                  return TRUE;
"tantque"               return WHILE;
"/*"                    BEGIN(COMMENT);
<COMMENT>"*/"           BEGIN(INITIAL);
<COMMENT>(.|\n)         ;
<COMMENT><<EOF>>        printf("Unexpected file end (unterminated comment).\n");
"<-"                    return ASSIGN;
"^"                     return DEREF;
"."                     printf("operator: %s\n", yytext);
","                     return COMMA;
":"                     printf("operator: %s\n", yytext);
"<"                     printf("operator: %s\n", yytext);
">"                     printf("operator: %s\n", yytext);
"<="                    printf("operator: %s\n", yytext);
">="                    printf("operator: %s\n", yytext);
"<>"                    return NEQ;
"="                     return EQ;
"+"                     return PLUS;
"-"                     return MINUS;
"*"                     return STAR;
"/"                     return SLASH;
"("                     return LPAREN;
")"                     return RPAREN;
"["                     return LBRACKET;
"]"                     return RBRACKET;
\"[^"]*\"               SAVE_STRING; return STRING;
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_STRING; return IDENT;
[0-9]+                  SAVE_INT; return INT;
[0-9]+(\.[0-9]+)?       SAVE_REAL; return REAL;
.                       ;

%%
