# Process this file with automake to produce Makefile.in

SUBDIRS = pixmaps

INCLUDES = @GTK_CFLAGS@ @GMODULE_CFLAGS@ \
	-I../libs -I../../extra/scintilla-wrapper/scintilla/include

bin_PROGRAMS = balsa-mgr

interface_files = balsa-mgr.glade balsa-mgr.gladep
EXTRA_DIST = $(interface_files)

balsa_mgr_SOURCES = \
	support.c support.h \
	main.c main.h \
	project.c project.h \
	file.c file.h \
	miscgtk.c miscgtk.h \
	options.c options.h \
	paths.c paths.h \
	buffer.c buffer.h \
	commands.c commands.h \
	projopts.c projopts.h \
	testopts.c testopts.h \
	filemanager.c filemanager.h \
	utils.c utils.h \
	makefiletable.c makefiletable.h \
	executionmanager.c executionmanager.h \
	addfiledialogue.c addfiledialogue.h \
	technology.c technology.h \
	workspace.c workspace.h \
	mainwindow.c mainwindow.h \
	executionwindow.c executionwindow.h \
	consolewindow.c consolewindow.h \
	aboutdialogue.c aboutdialogue.h \
	optsdialogue.c optsdialogue.h \
	menus.c menus.h \
	icons.c icons.h \
	projoptsdialogue.c projoptsdialogue.h \
	imploptsdialogue.c imploptsdialogue.h \
	builtinliboptsdialogue.c builtinliboptsdialogue.h \
	editor.c editor.h \
	ScintillaLexBalsa.cxx \
	interface.c interface.h \
	callbacks.h


balsa_mgr_LDADD = ../libs/breeze/libbreeze.la ../libs/format/libformat.la ../../extra/scintilla-wrapper/.libs/libscintilla.a @GTK_LIBS@ @GTHREAD_LIBS@ @GMODULE_LIBS@

ScintillaLexBalsa.o: ScintillaLexBalsa.cxx
	g++ ${INCLUDES} ${CFLAGS} ${CXXFLAGS} ${CPPFLAGS} -I../../extra/scintilla-wrapper -Wno-char-subscripts -DGTK -DSCI_LEXER -c ScintillaLexBalsa.cxx

lines:
	$(top_srcdir)/bin/count-lines $(balsa_mgr_SOURCES)
