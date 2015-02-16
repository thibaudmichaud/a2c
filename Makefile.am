AM_CFLAGS = -Wall -Werror -Wextra -std=c99 -I$(srcdir)/src -D_DEFAULT_SOURCE
AM_YFLAGS = -d -v

bin_PROGRAMS = a2c
a2c_SOURCES = src/parser.y src/lexer.l src/typechecking/typecheck.c src/codegen.c src/a2c.c

check_PROGRAMS = tests/list
tests_list_SOURCES = tests/list.c

TESTS = tests/parser.test tests/list.test
