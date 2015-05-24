#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "grammar.h"

void print_prog(struct prog *prog);
void print_algo(struct algo *algo);
void print_arglist(arglist_t l);
void free_algo(struct algo *algo);
void print_instructions(instructionlist_t instructions, int indent);
void print_instruction(struct instruction *i, int indent);
void free_instructions(instructionlist_t instructions);
void free_instruction(struct instruction *i);
void print_expression(struct expr *e);
void print_exprlist(exprlist_t l);
void print_const_decl(constdecllist_t const_decls, int indent);
void free_expression(struct expr *e);
void free_expressions(exprlist_t l);
void free_prog(struct prog *prog);

#endif
