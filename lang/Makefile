# Makefile for language module

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 'USE_MODULES=${USE_MODULES}'

LANGOBJS = cat de en_us es fr gr hun it nl pl pt ru tr
LANGSRCS = cat.l de.l en_us.l es.l fr.l gr.l hun.l it.l nl.l pl.l pt.l ru.l tr.l

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


cat:      cat.l      langcomp index
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
hun:      hun.l       langcomp index
	./langcomp $@.l
it:       it.l       langcomp index
	./langcomp $@.l
nl:       nl.l       langcomp index
	./langcomp $@.l
pl:       pl.l       langcomp index
	./langcomp $@.l
pt:       pt.l       langcomp index
	./langcomp $@.l
ru:       ru.l       langcomp index
	./langcomp $@.l
tr:       tr.l       langcomp index
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
