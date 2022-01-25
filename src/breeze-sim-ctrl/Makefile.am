## Process this file with automake to produce Makefile.in

SUBDIRS = pixmaps

bin_PROGRAMS = breeze-sim-ctrl

INCLUDES = @GTK_CFLAGS@ @GLIB_CFLAGS@ -I../libs

breeze_sim_ctrl_LDADD = ../libs/breeze/libbreeze.la @GTK_LIBS@ @GLIB_LIBS@ -lm

breeze_sim_ctrl_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	mainwindow2.c mainwindow2.h \
	structure.c structure.h \
	simulation.c simulation.h \
	libspring.c libspring.h libspring_internals.h \
	drawing.c drawing.h \
	simtrace.c simtrace.h \
	sourceview.c sourceview.h \
	core.c core.h \
	listview.c listview.h \
	graphview.c graphview.h \
	graphviewGUI.c graphviewGUI.h \
	timelineview.c timelineview.h \
	timelinebehaviourview.c timelinebehaviourview.h \
	selectionview.c selectionview.h \
	debug.c debug.h \
	partitioning.c partitioning.h

lines:
	$(top_srcdir)/bin/count-lines $(libspring_a_SOURCES) $(breeze_sim_ctrl_SOURCES)
