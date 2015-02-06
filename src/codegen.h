#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "grammar.h"

void print_instructions(struct block instructions);
void print_instruction(struct instruction *i);
void free_instructions(struct block instructions);
void free_instruction(struct instruction *i);
void print_expression(struct expr *e);
void free_expression(struct expr *e);

#endif
