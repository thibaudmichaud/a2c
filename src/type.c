#include "type.h"

void free_pointer(struct pointer* p)
{
    free(p);
}

void free_records(struct records* r)
{
    for (unsigned i = 0; i < r->fields.size; ++i)
    {
      free(r->fields.data[i]->type);
      free(r->fields.data[i]->ident);
      free(r->fields.data[i]);
    }
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
  free(type->name);
  free(type);
}
