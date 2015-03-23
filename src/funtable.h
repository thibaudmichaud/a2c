#ifndef _FUNTABLE_H
#define _FUNTABLE_H

#include <string.h>

#include "data_struct/hashtable/hashtable.h"
#include "typechecking/typecheck.h"

typedef enum
{
    global,
    local,
}parametre;

struct argument
{
    type type;
    parametre par;
};

typedef list_tpl(struct argument *) args;

struct function
{
    char* ident;
    type ret;
    args arg;
};

typedef ht_tpl(struct function*) funtable;
funtable init(void);
void add_function(funtable fun, struct function* f);
int find_function(funtable table, char* ident);
void delete_function(funtable table, struct function* f);
struct function* get_function(funtable table, char *ident);

#endif
