# Makefile for language module

include ../Makefile.inc


LANGOBJS = cat de en_us es fr gr nl pt tr it ru
LANGSRCS = cat de en_us.l es fr.l gr.l nl.l pt.l tr.l it.l ru.l

LANGCOMP = ./langcomp
#LANGCOMP = ./langcomp -w


all: $(LANGOBJS)


install: all
	mkdir -p $(DATDEST)/languages
ifdef RUNGROUP
	chgrp $(RUNGROUP) $(DATDEST)/languages
	chmod 770 $(DATDEST)/languages
else
	chmod 700 $(DATDEST)/languages
endif
	cp $(LANGOBJS) $(DATDEST)/languages
ifdef RUNGROUP
	chgrp $(RUNGROUP) $(DATDEST)/languages/*
	chmod 660 $(DATDEST)/languages/*
else
	chmod 600 $(DATDEST)/languages/*
endif


clean:
	rm -f $(LANGOBJS) langcomp

spotless: clean
	rm -f language.h index


cat:       cat.l       langcomp index
	./langcomp $@.l
de:       de.l       langcomp index
	./langcomp $@.l
en_us:    en_us.l    langcomp index
	./langcomp $@.l
es:       es.l       langcomp index
	./langcomp $@.l
fr:       fr.l       langcomp index
	./langcomp $@.l
gr:       gr.l       langcomp index
	./langcomp $@.l
nl:       nl.l       langcomp index
	./langcomp $@.l
pt:       pt.l       langcomp index
	./langcomp $@.l
tr:       tr.l       langcomp index
	./langcomp $@.l
it:       it.l       langcomp index
	./langcomp $@.l
ru:       ru.l       langcomp index
	./langcomp $@.l

langcomp: langcomp.c
	$(CC) $(CFLAGS) langcomp.c -o $@


language.h: index Makefile
	@perl -e <index >$@ '\
		print STDERR "Generating language.h... "; \
		$$i=0; \
		while (<>) { \
			chop; \
			printf "#define %-32s %d\n", $$_, $$i++; \
		} \
		print "\n#define NUM_STRINGS $$i\n"; \
		print STDERR "$$i strings\n";'

index: en_us.l
	grep '^[A-Z]' en_us.l >index
