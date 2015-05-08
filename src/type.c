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
    free(p);
}

void free_records(struct records* r)
{
    for (unsigned i = 0; i < r->fields.size; ++i)
      free(r->fields.data[i]);
    list_free(r->fields);
    free(r);
}
void free_enum(struct enum_type* _enum)
{
  free(_enum);
}

void free_array(struct array *a)
{
  free(a);
}

void free_type(struct type *type)
{
  switch (type->type_kind)
  {
    case primary_t:
      break;
    case records_t:
      free_records(type->type_val.records_type);
      break;
    case array_t:
      free_array(type->type_val.array_type);
      break;
    case pointer_t:
      free_pointer(type->type_val.pointer_type);
      break;
    case enum_t:
      free_enum(type->type_val.enum_type);
      break;
  }
  free(type);
}
