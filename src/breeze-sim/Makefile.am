## Process this file with automake to produce Makefile.in

SUBDIRS = fast debug early

EXTRA_DIST = breeze-sim mpi \
	components_4phase_eagerfvs.c \
	components_4phase_early.c \
	components_4phase_for_sam.c \
	components_4phase_broad.c \
	components_4phase_hla.c

bin_SCRIPTS = breeze-sim

lines:
	$(top_srcdir)/bin/count-lines *.c *.h
