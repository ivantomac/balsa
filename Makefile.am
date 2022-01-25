## Process this file with automake to produce Makefile.in

balsaconfigdir = $(datadir)/config

balsaconfig_DATA = balsa

SUBDIRS = doc extra bin src share

EXTRA_DIST = header bootstrap balsa

lines:
	rm -f /tmp/linecount.total
	touch /tmp/linecount.total
	for a in $(SUBDIRS); do make -C $$a lines; done
	$(top_srcdir)/bin/count-lines-total /tmp/linecount.total
	rm -f /tmp/linecount.total
