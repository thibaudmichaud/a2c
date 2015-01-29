AM_CFLAGS = -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200112L

BUILT_SOURCES = parser.h
AM_YFLAGS = -d

bin_PROGRAMS = a2c parser

a2c_SOURCES = a2c.c
parser_SOURCES = lexer.l parser.y

lexer_SOURCES = lexer.l
