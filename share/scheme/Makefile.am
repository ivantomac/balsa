## Process this file with automake to produce Makefile.in

schemedir = $(datadir)/scheme

scheme_DATA = \
	balsa-list-builtins.scm \
	balsa-make-helpers.scm \
	balsa-make-impl-test.scm \
	balsa-make-makefile.scm \
	balsa-make-test.scm \
	balsa-net-merge-netlists.scm \
	balsa-net-tran-cost.scm \
	balsa-netlist.scm \
	balsa-prog-banner.scm \
	balsa-sim-impl.scm \
	base-local.scm \
	base.scm \
	breeze-cost.scm \
	brz-parser.scm \
	brz-tech.scm \
	brz.scm \
	gen-espresso.scm \
	gen-gates.scm \
	gen-hcs.scm \
	gen-implicants.scm \
	gen-mofn-espresso.scm \
	gen-undeclared.scm \
	gen.scm \
	globals.scm \
	misc-banners.scm \
	misc-config.scm \
	misc-date.scm \
	misc-implicants.scm \
	misc-spans.scm \
	misc-switches.scm \
	misc.scm \
	net-connections.scm \
	net-insert-buffers.scm \
	net-modify.scm \
	net-parser.scm \
	net-verilog.scm \
	breeze2ps.scm \
	net.scm

EXTRA_DIST = base.scm.in base-local.scm.in globals.scm.in $(scheme_DATA)

lines:
	for a in $(SUBDIRS); do make -C $$a lines; done
