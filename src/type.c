#include "type.h"

void free_pointer(struct pointer* p)
{
    switch(p->type->type_kind)
    {
        case primary_t:
            free(p->type);
            break;
        case records_t:
            free_records(p->type->type_val.records_type);
            break;
        case array_t:
            break;
        case pointer_t:
            free_pointer(p->type->type_val.pointer_type);
            break;
        case enum_t:
            break;
    }
}

void free_records(struct records* r)
{
    for(unsigned int i = 0 ; i < r->fields->size; ++i)
    {
        struct field* field = list_nth(*r->fields, i);
        switch(field->type->type_kind)
        {
            case primary_t:
                free(field->type);
                break;
            case records_t:
                free_records(field->type->type_val.records_type);
                break;
            case array_t:
                break;
            case pointer_t:
                free_pointer(field->type->type_val.pointer_type);
                break;
            case enum_t:
                free_enum(field->type->type_val.enum_type);
                break;
        }
    }
    free(r);
}
void free_enum(struct enum_type* _enum)
{
    for(unsigned int i = 0 ; i < _enum->idents.size; ++i)
    {
        free(list_nth(_enum->idents,i));
    }
}
