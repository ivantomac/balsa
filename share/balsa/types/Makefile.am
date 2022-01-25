## Process this file with automake to produce Makefile.in

balsatypesdir = $(datadir)/balsa/types

balsa_files = basic.balsa builtin.balsa synthesis.balsa types.balsa
breeze_files = basic.breeze builtin.breeze synthesis.breeze types.breeze

builtin_libs = builtin types
la_files = builtin.la types.la
c_files = builtin.c types.c

balsatypes_DATA = $(breeze_files) $(balsa_files)

noinst_DATA = $(la_files)

CLEANFILES = $(breeze_files) *.o *.lo $(la_files)

EXTRA_DIST = $(balsa_files) $(c_files)

BALSAC=$(top_builddir)/src/balsa-c/balsa-c
BALSACFLAGS=-e -I $(top_srcdir)/share -b -i

MBI=$(top_builddir)/bin/balsa-make-builtin-lib
MBIFLAGS=-i $(top_builddir) -j $(balsatypesdir)
MBICFLAGS=-I $(top_builddir)/src/libs

builtin.breeze: builtin.balsa $(BALSAC)
	$(BALSAC) $(BALSACFLAGS) -j $<

basic.breeze synthesis.breeze types.breeze: builtin.breeze
$(breeze_files): $(BALSAC)
$(la_files): $(MBI)

.balsa.breeze:
	$(BALSAC) $(BALSACFLAGS) $<

.c.la: $(MBI)
	$(SHELL) $(MBI) $(MBIFLAGS) -c `echo $< | sed 's/.c$$//'` $(MBICFLAGS) $<

install-exec-hook: $(top_builddir)/bin/balsa-make-builtin-lib
	for lib in $(builtin_libs); do \
		$(SHELL) $(MBI) $(MBIFLAGS) -I $${lib} $(MBICFLAGS) $${lib}.c; \
	done

lines:
	$(top_srcdir)/bin/count-lines $(balsa_files) $(c_files)
