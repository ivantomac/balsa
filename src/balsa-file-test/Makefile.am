## Process this file with automake to produce Makefile.in

INCLUDES =

bin_PROGRAMS = balsa-file-test

balsa_file_test_LDADD = @GLIB_LIBS@

balsa_file_test_SOURCES = main.c

lines:
	$(top_srcdir)/bin/count-lines $(balsa_file_test_SOURCES)
