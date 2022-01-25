## Process this file with automake to produce Makefile.in

bin_SCRIPTS = \
	balsa-espresso \
	balsa-scheme-shell \
	balsa-scheme-script \
	balsa-config \
	balsa-make-builtin-lib

balsa_scripts = \
	balsa-list-builtins \
	balsa-make-makefile \
	breeze2ps \
	breeze-opt \
	breeze-cost \
	balsa-netlist \
	balsa-make-helpers \
	balsa-make-test \
	balsa-make-impl-test \
	balsa-sim-impl \
	balsa-prog-banner \
	balsa-net-tran-cost \
	balsa-net-merge-netlists

EXTRA_DIST = \
	balsa-config.in \
	balsa-espresso.in \
	balsa-make-builtin-lib.in \
	balsa-local-script.in \
	balsa-scheme-shell.in \
	balsa-scheme-script.in \
	count-lines \
	count-lines-total

install-exec-hook:
	for script in $(balsa_scripts); do \
		$(RM) -f $(bindir)/$${script}; \
		$(LN_S) ./balsa-scheme-script $(bindir)/$${script}; \
	done; \
	$(INSTALL) -m 755 ../libtool $(bindir)/balsa-libtool
	$(INSTALL) -m 755 ../install-sh $(bindir)/balsa-install-sh

lines:
