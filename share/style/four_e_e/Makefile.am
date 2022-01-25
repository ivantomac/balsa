## Process this file with automake to produce Makefile.in

stylefour_e_edir = $(datadir)/style/four_e_e

stylefour_e_e_DATA = \
	startup.scm \
	Arbiter.abs \
	Bar.abs \
	BuiltinVariable.abs \
	Call.abs \
	CallDemux.abs \
	CallDemuxPush.abs \
	CallMux.abs \
	Case.abs \
	CaseFetch.abs \
	Concur.abs \
	DecisionWait.abs \
	Encode.abs \
	FalseVariable.abs \
	Fetch.abs \
	NullAdapt.abs \
	PassivatorPush.abs \
	Sequence.abs \
	Variable.abs \
	While.abs

EXTRA_DIST = $(stylefour_e_e_DATA)

lines:
	$(top_srcdir)/bin/count-lines $(stylefour_e_e_DATA)
