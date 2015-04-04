#ifndef _TYPE_H
#define _TYPE_H

#include "data_struct/list/list.h"

struct field
{
    char* ident;
    struct type* t;
};

typedef list_tpl(struct field *) fieldlist_t;
typedef list_tpl(int) indiceslist_t;
typedef list_tpl(char *) idents;

struct records {
    char* ident;
    fieldlist_t fields;
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
    struct type* t;
};

struct tab
{
    char* ident;
    indiceslist_t indices;
};

struct enum_type 
{
    idents idents;
};

struct type {
    enum
    {
        primary_t,
        records_t,
        tab_t,
        pointer_t,
        enum_t,
    }type_kind;
    union
    {
        primary_type   primary;
        struct records*      records_type;
        struct tab*          tab_type;
        struct pointer*      pointer_type;
        struct enum_type*    enum_type;
    }type_val;
};
#endif
