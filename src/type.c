#include "type.h"

// void copy_type(struct type *t)
// {
//   struct type *newt = malloc(sizeof(struct type));
//   newt->type_kind = t->type_kind;
//   switch (t->type_kind)
//   {
//     case primary_t:
//       newt->type_val = t->type_val;
//       break;
//     case records_t:
//       newt->type_val.records_type = malloc(sizeof(struct record_type));
//       newt->type_val.records_type->fields
//       for (unsigned i = 0; i < t->type_val.records_type->fields->size; ++i)
//       {
//         newt->type_val.records_type->fields
//       }
//   }
// }

void free_pointer(struct pointer* p)
{
    switch(p->type->type_kind)
    {
        case primary_t:
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
    free(p);
}

void free_records(struct records* r)
{
    for(unsigned int i = 0 ; i < r->fields.size; ++i)
    {
        struct field* field = list_nth(r->fields, i);
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
