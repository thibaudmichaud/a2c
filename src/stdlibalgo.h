#ifndef STDLIBALGO_H_
#define STDLIBALGO_H_

# include "a2c.h"

enum e_types
{
  E_TYPE_INT  =   0 ,
  E_TYPE_CHAR =   1 ,
  E_TYPE_REAL       ,
  E_TYPE_BOOLEAN    ,
  E_TYPE_STRING
};

static char *types_str[2][6] = 
  { {"entier", "caractere", "reel", "booleen", "chaine"},
    {"int", "char", "real", "boolean", "string"} };

/* types */
#  define TYPE_INT     types_str[current_lang][E_TYPE_INT]
#  define TYPE_CHAR    types_str[current_lang][E_TYPE_CHAR]
#  define TYPE_REAL    types_str[current_lang][E_TYPE_REAL]
#  define TYPE_BOOLEAN types_str[current_lang][E_TYPE_BOOLEAN]
#  define TYPE_STRING  types_str[current_lang][E_TYPE_STRING]

void fill_std_types(struct symtable *syms);
void fill_std_fun(struct symtable *syms);

#endif
