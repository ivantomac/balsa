## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libbreeze.la

INCLUDES = -I.. @GLIB_CFLAGS@

breezedir = $(includedir)/breeze

breeze_HEADERS = \
	lscan.h \
	lparse.h \
	libbreeze.h \
	callcontexttree.h \
	threadgroups.h \
	breezestructs.h \
        flatten.h \
	flatten_breezefiles.h \
	flatten_flatten.h \
	flatten_types.h \
	behaviour.h

libbreeze_la_LDFLAGS = -static

libbreeze_la_SOURCES = \
	lscan.c \
	lparse.c \
	libbreeze.c \
	callcontexttree.c \
	threadgroups.c \
        flatten.c \
	flatten_breezefiles.c \
	flatten_flatten.c \
	flatten_types.c \
	behaviour.c


lines:
	$(top_srcdir)/bin/count-lines $(libbreeze_la_SOURCES) $(breeze_HEADERS)
