AM_CFLAGS = -Wall -Werror -Wextra -g -std=c99 -I$(srcdir)/src -O0 -g -D_DEFAULT_SOURCE
AM_YFLAGS = --report=state -d

bin_PROGRAMS = a2c
a2c_SOURCES = src/lexer.c \
	      src/parser.c \
	      src/type.c \
	      src/type_table.c \
	      src/get_line.c \
	      src/funtable.c \
	      src/var_table.c \
	      src/typecheck.c \
	      src/codegen.c \
	      src/a2c.c \
	      src/error.c \
	      src/stdlibalgo.c

include_HEADERS = src/standard_lib/standard_lib.h

check_PROGRAMS = tests/hashtable tests/list

test_list_source= tests/list.c

TESTS = tests/parser.test \
	tests/test-ifthen.test \
	tests/list.test \
	tests/parser-testfail.test \
	tests/hashtable.test \
	tests/test-typedecl.test \
	tests/algolist.test \
	tests/emptyalgolist.test \
	tests/const-decl.test \
	tests/fibo.test \
	tests/all.test \
	tests/switch.test \
	tests/switchnodef.test \
	tests/multiply.test \
	tests/array.test \
	tests/struct.test \
	tests/product.test \
	tests/typerec.test \
	tests/ptr.test \
	tests/globvalparam.test \
	tests/allouer.test \
	tests/intvect.test \
	tests/arbrebin.test
