## Process this file with automake to produce Makefile.in

SUBDIRS = libs \
	balsa-c balsa-file-test balsa-mgr breeze-sim breeze-sim-ctrl

lines:
	for a in $(SUBDIRS); do make -C $$a lines; done
