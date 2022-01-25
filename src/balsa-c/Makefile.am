## Process this file with automake to produce Makefile.in

INCLUDES = @GLIB_CFLAGS@ -Wall -Wno-char-subscripts -Wno-unused-label

bin_PROGRAMS = balsa-c

balsa_c_LDADD = @GLIB_LIBS@

hand_c_srcs = \
	Errors.c Errors.h \
	Positions.c Positions.h \
	BalsaScanSource.c BalsaScanSource.h \
	BreezeScanSource.c BreezeScanSource.h \
	accesses.c accesses.h \
	arith.c arith.h \
	block.c block.h \
	channels.c channels.h \
	commands.c commands.h \
	components.c components.h \
	contexts.c contexts.h \
	decls.c decls.h \
	exprs.c exprs.h \
	flags.c flags.h \
	instances.c instances.h \
	lists.c lists.h \
	lvalues.c lvalues.h \
	misc.c misc.h \
	misclists.c misclists.h \
	operators.c operators.h \
	optimise.c optimise.h \
	output.c output.h \
	parts.c parts.h \
	ports.c ports.h \
	pretty.c pretty.h \
	procedures.c procedures.h \
	spans.c spans.h \
	implicants.c implicants.h \
	types.c types.h \
	values.c values.h \
	wires.c wires.h \
	balsa-c.c \
	banner.h \
	Idents.c Idents.h \
	rMemory.h \
	DynArray.h \
	StringMem.h \
	rSystem.h \
	General.c General.h \
	Sets.c Sets.h \
	callcontexts.c callcontexts.h

hand_other_srcs = \
	Balsa.ag Balsa.pars Balsa.scan \
	Breeze.pars Breeze.scan

auto_c_srcs = \
	BalsaScan.c \
	BreezeScan.c \
	Balsa.c \
	Breeze.c \
	Semantics.c \
	Tree.c

auto_h_srcs = \
	BalsaScan.h \
	BreezeScan.h \
	Balsa.h \
	Breeze.h \
	Semantics.h \
	Tree.h

auto_other_srcs = \
	Balsa.rex Balsa.lalr \
	BalsaScan.rpp \
	Breeze.rex Breeze.lalr \
	BreezeScan.rpp \
	yyTree.w

.PHONY: auto-clean

auto-clean: clean

balsa_c_SOURCES = $(hand_c_srcs) $(auto_c_srcs) $(auto_h_srcs)

EXTRA_balsa_c_SOURCES = $(hand_other_srcs) $(auto_other_srcs)

BUILT_SOURCES = $(auto_h_srcs)

EXTRA_DIST = template.c template.h fixag.pl fixrex.pl

BalsaScan.rpp: Balsa.lalr

Balsa.lalr: Balsa.pars Tree.h BalsaScanSource.h arith.h ports.h implicants.h
	if test -z $(CG); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Balsa.lalr; \
	else \
		$(CG) -cxjz Balsa.pars; \
	fi

Balsa.rex: Balsa.scan BalsaScan.rpp
	if test -z $(RPP); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Balsa.rex; \
	else \
		$(RPP) BalsaScan.rpp < Balsa.scan > Balsa.rex; \
	fi

BalsaScan.h: BalsaScan.c

BalsaScan.c: Balsa.rex 
	if test -z $(REX); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch BalsaScan.c; \
	else \
		$(REX) -cd Balsa.rex; \
		./fixrex.pl BalsaScan.c > BalsaScan_fixed.c; \
		mv -f BalsaScan_fixed.c BalsaScan.c; \
	fi

Balsa.h: Balsa.c

Balsa.c: Balsa.lalr
	if test -z $(LALR); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Balsa.c; \
	else \
		$(LALR) -c -d Balsa.lalr 2>&1 | grep -v '^Information'; \
	fi

BreezeScan.rpp: Breeze.lalr

Breeze.lalr: Breeze.pars Tree.h BreezeScanSource.h arith.h ports.h implicants.h wires.h callcontexts.h
	if test -z $(CG); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Breeze.lalr; \
	else \
		$(CG) -cxjz Breeze.pars; \
	fi

Breeze.rex: Breeze.scan BreezeScan.rpp
	if test -z $(RPP); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Breeze.rex; \
	else \
		$(RPP) BreezeScan.rpp < Breeze.scan > Breeze.rex; \
	fi

BreezeScan.h: BreezeScan.c

BreezeScan.c: Breeze.rex 
	if test -z $(REX); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch BreezeScan.c; \
	else \
		$(REX) -cd Breeze.rex; \
		./fixrex.pl BreezeScan.c > BreezeScan_fixed.c; \
		mv -f BreezeScan_fixed.c BreezeScan.c; \
	fi

Breeze.h: Breeze.c

Breeze.c: Breeze.lalr
	if test -z $(LALR); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Breeze.c; \
	else \
		$(LALR) -c -d Breeze.lalr; \
	fi

Tree.h: Tree.c
Semantics.h: Tree.c
Semactics.c: Tree.c
yyTree.w: Tree.c

Tree.c: Balsa.ag
	if test -z $(AST); then \
		echo '*** Cocktail not installed, may be needed to build modified source properly'; \
		touch Tree.c; \
	else \
		$(AST) -I -cdimyRD68 Balsa.ag; \
		./fixag.pl Semantics.c > Semantics_fixed.c; \
		mv -f Semantics_fixed.c Semantics.c; \
	fi

lines:
	$(top_srcdir)/bin/count-lines $(hand_c_srcs) $(hand_other_srcs)
