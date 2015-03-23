AM_CFLAGS = -Wall -Werror -Wextra -std=c99 -I$(srcdir)/src -g -D_DEFAULT_SOURCE
AM_YFLAGS = --report=state -d

bin_PROGRAMS = a2c
a2c_SOURCES = src/parser.y \
	      src/lexer.l \
	      src/typechecking/typecheck.c \
	      src/codegen.c \
	      src/a2c.c \
	      src/var_table.c

check_PROGRAMS = tests/hashtable tests/list
tests_hashtable_SOURCES = tests/hashtable.c

test_list_source= tests/list.c

TESTS = tests/parser.test \
	tests/list.test \
	tests/parser-testfail.test \
	tests/hashtable.test \
	tests/test-typedecl.test
