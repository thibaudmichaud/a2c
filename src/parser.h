#ifndef PARSER_H_
#define PARSER_H_

#include <grammar.h>

struct expr *parse_expression(void);
instructionlist_t parse(void);

#endif
