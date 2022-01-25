## Process this file with automake to produce Makefile.in

bin_PROGRAMS = breeze-sim-mpi

INCLUDES = -I../../libs -I.. @GLIB_CFLAGS@ -DMPI -DREORDERED_EVENT_DRIVEN_SCHEDULER `mpicc -showme:compile`

breeze_sim_mpi_LDFLAGS = -export-dynamic `mpicc -showme:link`

breeze_sim_mpi_LDADD = \
	../../libs/breeze/libbreeze.la ../../libs/balsasim/libbalsasim.la ../../libs/format/libformat.la -lm @GLIB_LIBS@

breeze_sim_mpi_SOURCES = \
	../main.c \
	../simulate.c ../simulate.h \
	../pattern_analysis.c ../pattern_analysis.h \
	../trace.c ../trace.h \
	../components_4phase.c ../components_4phase.h \
	../breezesim_plugin.h
