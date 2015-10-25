#pragma once

/* types */
# ifdef LANG_FR
#   define TYPE_INT     "entier"
#   define TYPE_CHAR    "caractere"
#   define TYPE_REAL    "reel"
#   define TYPE_BOOLEAN "booleen"
#   define TYPE_STRING  "chaine"
# else
#   define TYPE_INT     "int"
#   define TYPE_CHAR    "char"
#   define TYPE_REAL    "real"
#   define TYPE_BOOLEAN "boolean"
#   define TYPE_STRING  "string"
# endif

void fill_std_types(struct symtable *syms);
void fill_std_fun(struct symtable *syms);
