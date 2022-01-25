## Process this file with automake to produce Makefile.in

INCLUDES = -I..

formatlibdir = @formatlibdir@
formatincdir = @formatincdir@

formatlib_LTLIBRARIES = libformat.la

formatinc_HEADERS = \
	data.h \
	format.h \
	external.h \
	dictionary.h

libformat_la_LDFLAGS = -fPIC -FPIC -static

libformat_la_SOURCES = \
	data.c \
	format.c \
	external.c \
	dictionary.c

EXTRA_DIST = test1.c test2.c

lines:
	$(top_srcdir)/bin/count-lines $(libformat_la_SOURCES) $(formatinc_HEADERS)
