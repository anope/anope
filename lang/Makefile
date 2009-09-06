# Makefile for language module

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' 'MAKEBIN=${MAKEBIN}'

LANGOBJS = cat de en_us es fr gr hun it nl pl pt ru tr
LANGSRCS = cat.l de.l en_us.l es.l fr.l gr.l hun.l it.l nl.l pl.l pt.l ru.l tr.l

LANGCOMP = ./langcomp
#LANGCOMP = ./langcomp -w


all: $(LANGOBJS)

distclean: clean spotless

install: all
	test -d $(INSTDIR)/data/languages || mkdir $(INSTDIR)/data/languages
	@if [ "$(RUNGROUP)" ] ; then \
		echo chgrp -R $(RUNGROUP) $(INSTDIR)/data/languages ; \
		chgrp -R $(RUNGROUP) $(INSTDIR)/data/languages ; \
		echo chmod -R g+rw $(INSTDIR)/data/languages ; \
		chmod -R g+rw $(INSTDIR)/data/languages ; \
		$(CP) $(LANGOBJS) $(INSTDIR)/data/languages ; \
		echo chgrp -R $(RUNGROUP) $(INSTDIR)/data/languages ; \
		chgrp -R $(RUNGROUP) $(INSTDIR)/data/languages/* ; \
		echo chmod -R g+rw $(INSTDIR)/data/languages/* ; \
		chmod -R g+rw $(INSTDIR)/data/languages/* ; \
	fi
	@if [ ! "$(RUNGROUP)" ] ; then \
		chmod 700 $(INSTDIR)/data/languages/ ; \
		$(CP) $(LANGOBJS) $(INSTDIR)/data/languages ; \
		chmod 600 $(INSTDIR)/data/languages/* ; \
	fi

clean:
	rm -f $(LANGOBJS) langcomp language.h

spotless: clean
	rm -f index


cat:      cat.l      langcomp index
	@./langcomp $@.l $@
de:       de.l       langcomp index
	@./langcomp $@.l $@
en_us:    en_us.l    langcomp index
	@./langcomp $@.l $@
es:       es.l       langcomp index
	@./langcomp $@.l $@
fr:       fr.l       langcomp index
	@./langcomp $@.l $@
gr:       gr.l       langcomp index
	@./langcomp $@.l $@
hun:      hun.l       langcomp index
	@./langcomp $@.l $@
it:       it.l       langcomp index
	@./langcomp $@.l $@
nl:       nl.l       langcomp index
	@./langcomp $@.l $@
pl:       pl.l       langcomp index
	@./langcomp $@.l $@
pt:       pt.l       langcomp index
	@./langcomp $@.l $@
ru:       ru.l       langcomp index
	@./langcomp $@.l $@
tr:       tr.l       langcomp index
	@./langcomp $@.l $@

langcomp: langcomp.c
	$(MAKEBIN) $(CC) $(CFLAGS) langcomp.c -o $@


language.h: index Makefile
	@perl -e <index >$@ 'print STDERR "Generating language.h... "; $$i=0; while (<>) { chop; printf "#define %-32s %d\n", $$_, $$i++; } print "\n#define NUM_STRINGS $$i\n"; print STDERR "$$i strings\n";'
	@cp language.h ../include

index: en_us.l
	@grep '^[A-Z]' en_us.l >index
