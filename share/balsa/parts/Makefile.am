## Process this file with automake to produce Makefile.in

balsapartsdir = $(datadir)/balsa/parts

balsa_files = buffers.balsa counters.balsa
breeze_files = buffers.breeze counters.breeze

balsaparts_DATA = $(breeze_files) $(balsa_files)

CLEANFILES = $(breeze_files)

EXTRA_DIST = $(balsa_files)

BALSAC=$(top_builddir)/src/balsa-c/balsa-c
BALSACFLAGS=-e -I $(top_srcdir)/share -b -i

$(breeze_files): $(BALSAC)

.balsa.breeze:
	$(BALSAC) $(BALSACFLAGS) $<

lines:
	$(top_srcdir)/bin/count-lines $(EXTRA_DIST)
