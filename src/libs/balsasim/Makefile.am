## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libbalsasim.la

INCLUDES = -I..

balsasimdir = $(includedir)/balsasim

balsasim_HEADERS = \
	builtin.h \
	object.h \
	type.h \
	list.h \
	parameter.h \
	bstring.h \
	bfile.h

libbalsasim_la_LDFLAGS = -static

libbalsasim_la_SOURCES = \
	builtin.c \
	object.c \
	type.c \
	list.c \
	parameter.c \
	bstring.c \
	bfile.c

lines:
	$(top_srcdir)/bin/count-lines $(libbalsasim_la_SOURCES) $(balsasim_HEADERS)
