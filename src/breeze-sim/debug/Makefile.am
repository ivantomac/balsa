## Process this file with automake to produce Makefile.in

bin_PROGRAMS = breeze-sim-debug

INCLUDES = -I../../libs -I.. @GLIB_CFLAGS@ @GMODULE_CFLAGS@

breeze_sim_debug_LDFLAGS = -export-dynamic

breeze_sim_debug_LDADD = \
	../../libs/breeze/libbreeze.la ../../libs/balsasim/libbalsasim.la ../../libs/format/libformat.la -lm @GLIB_LIBS@ @GMODULE_LIBS@

breeze_sim_debug_SOURCES = \
	../main.c \
	../simulate.c ../simulate.h \
	../pattern_analysis.c ../pattern_analysis.h \
	../trace.c ../trace.h \
	../components_4phase.c ../components_4phase.h \
	../breezesim_plugin.h
