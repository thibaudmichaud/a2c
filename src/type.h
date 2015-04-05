#ifndef _TYPE_H
#define _TYPE_H

#include "data_struct/list/list.h"
#include "grammar.h"
struct field
{
    char* ident;
    struct type* type;
};

typedef list_tpl(struct field *) fieldlist_t;

struct records {
    fieldlist_t* fields;
};

typedef enum {
    int_t,
    char_t,
    str_t,
    real_t,
    bool_t,
    nul_t,
}primary_type;

struct pointer
{
    char* ident;
    struct type* type;
};

struct array
{
    struct type* type;
    intlist_t dims;
};

struct enum_type 
{
    identlist_t idents;
};

struct type {
    enum
    {
        primary_t,
        records_t,
        array_t,
        pointer_t,
        enum_t,
    }type_kind;
    union
    {
        primary_type   primary;
        struct records*      records_type;
        struct array*        array_type;
        struct pointer*      pointer_type;
        struct enum_type*    enum_type;
    }type_val;
};
#endif
