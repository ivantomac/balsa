## Process this file with automake to produce Makefile.in

balsasimdir = $(datadir)/balsa/sim

breeze_files = fileio.breeze string.breeze memory.breeze sim.breeze portio.breeze
balsa_files = fileio.balsa string.balsa memory.balsa sim.balsa portio.balsa

builtin_libs = fileio string memory sim
la_files = fileio.la string.la memory.la sim.la
c_files = fileio.c string.c memory.c sim.c

balsasim_DATA = $(breeze_files) $(balsa_files)

noinst_DATA = $(la_files)

CLEANFILES = $(breeze_files) *.o *.lo $(la_files)

EXTRA_DIST = $(balsa_files) $(c_files)

BALSAC=$(top_builddir)/src/balsa-c/balsa-c
BALSACFLAGS=-e -I $(top_srcdir)/share -b -i

MBI=$(top_builddir)/bin/balsa-make-builtin-lib
MBIFLAGS=-i $(top_builddir) -j $(balsasimdir)
MBICFLAGS=-I $(top_builddir)/src/libs

$(breeze_files): $(BALSAC)
$(la_files): $(MBI)

.balsa.breeze: $(BALSAC)
	$(BALSAC) $(BALSACFLAGS) $<

.c.la: $(MBI)
	$(SHELL) $(MBI) $(MBIFLAGS) -c `echo $< | sed 's/.c$$//'` $(MBICFLAGS) $<

install-exec-hook: $(top_builddir)/bin/balsa-make-builtin-lib
	for lib in $(builtin_libs); do \
		$(SHELL) $(MBI) $(MBIFLAGS) -I $${lib} $(MBICFLAGS) $${lib}.c; \
	done

lines:
	$(top_srcdir)/bin/count-lines $(balsa_files) $(c_files)
