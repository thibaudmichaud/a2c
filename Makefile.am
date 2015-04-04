AM_CFLAGS = -Wall -Werror -Wextra -std=c99 -I$(srcdir)/src -O0 -g -D_DEFAULT_SOURCE
AM_YFLAGS = --report=state -d

bin_PROGRAMS = a2c
a2c_SOURCES = src/parser.y \
	      src/lexer.l \
	      src/typechecking/typecheck.c \
	      src/codegen.c \
	      src/type_table.c \
	      src/var_table.c \
	      src/a2c.c

check_PROGRAMS = tests/hashtable tests/list

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
  tests/switch.test
