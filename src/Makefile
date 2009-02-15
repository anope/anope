OBJS =  actions.o base64.o bots.o botserv.o channels.o chanserv.o command.o commands.o compat.o \
		config.o datafiles.o encrypt.o events.o hashcomp.o helpserv.o hostserv.o init.o ircd.o language.o log.o mail.o main.o \
		memory.o memoserv.o messages.o misc.o modules.o news.o nickserv.o operserv.o \
		process.o protocol.o send.o servers.o sessions.o slist.o sockutil.o opertype.o timeout.o users.o module.o modulemanager.o configreader.o \
		wildcard.o nickcore.o

INCLUDES = ../include/commands.h ../include/defs.h  ../include/language.h \
	../include/pseudo.h ../include/sysconf.h ../include/config.h \
	../include/encrypt.h ../include/messages.h ../include/services.h \
	../include/timeout.h ../include/datafiles.h ../include/extern.h \
	../include/modules.h ../include/slist.h ../include/hashcomp.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}' \
	   'MAKEBIN=${MAKEBIN}'

.c.o:
	@$(MAKEBIN) $(CC) $(CFLAGS) -I../include/ -c $<

.cpp.o:
	@$(MAKEBIN) $(CC) $(CFLAGS) -I../include/ -c $<

all: services

distclean: spotless
distclean_modules: clean_modules spotless

services:  $(OBJS) mod_version
	@$(MAKEBIN) $(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS)

$(OBJS): Makefile
nickcore.o:     nickcore.cpp $(INCLUDES)
actions.o:      actions.c   $(INCLUDES)
base64.o:       base64.c    $(INCLUDES)
bots.o:         bots.cpp    $(INCLUDES)
botserv.o:      botserv.c   $(INCLUDES)
channels.o:     channels.c  $(INCLUDES)
chanserv.o:     chanserv.c  $(INCLUDES)
command.o:     command.cpp  $(INCLUDES)
commands.o:     commands.c  $(INCLUDES)
compat.o:       compat.c    $(INCLUDES)
config.o:       config.c    $(INCLUDES)
datafiles.o:    datafiles.c $(INCLUDES)
encrypt.o:      encrypt.c   $(INCLUDES)
events.o:       events.c    $(INCLUDES)
init.o:         init.c      $(INCLUDES)
ircd.o:         ircd.c      $(INCLUDES)
helpserv.o:     helpserv.c  $(INCLUDES)
hostserv.o:     hostserv.c  $(INCLUDES)
language.o:     language.c  $(INCLUDES)
log.o:          log.c       $(INCLUDES)
mail.o:         mail.c      $(INCLUDES)
main.o:         main.c      $(INCLUDES)
memory.o:       memory.c    $(INCLUDES)
memoserv.o:     memoserv.c  $(INCLUDES)
messages.o:     messages.c  $(INCLUDES)
modules.o:      modules.c   $(INCLUDES)
module.o:       module.cpp   $(INCLUDES)
modulemanager.o:      modulemanager.cpp   $(INCLUDES)
misc.o:         misc.c      $(INCLUDES)
news.o:         news.c      $(INCLUDES)
nickserv.o:     nickserv.c  $(INCLUDES)
operserv.o:     operserv.c  $(INCLUDES)
process.o:      process.c   $(INCLUDES)
protocol.o:     protocol.cpp  $(INCLUDES)
send.o:         send.c      $(INCLUDES)
servers.o:      servers.c   $(INCLUDES)
sessions.o:     sessions.c  $(INCLUDES)
slist.o:        slist.c     $(INCLUDES)
sockutil.o:     sockutil.c  $(INCLUDES)
opertype.o:   opertype.cpp  $(INCLUDES)
timeout.o:      timeout.c   $(INCLUDES)
users.o:        users.c     $(INCLUDES)
vsnprintf.o:    vsnprintf.c $(INCLUDES)
wildcard.o:     wildcard.cpp $(INCLUDES)

mod_version:	mod_version.c	$(INCLUDES)
	@$(MAKEBIN) $(CC) $(CDEFS) $(CFLAGS) $(MODULEFLAGS) -I../include/ -c mod_version.c

modules: DUMMY
	@modules/configure modules
	@${MAKE} -C modules ${MAKEARGS} all

protocols: DUMMY
	@protocol/configure protocol
	@${MAKE} -C protocol ${MAKEARGS} all

core: DUMMY
	@core/configure core
	@${MAKE} -C core ${MAKEARGS} all

clean: clean_modules clean_protocols clean_core
	rm -f *.o services a.out

clean_modules:
	@touch modules/Makefile.inc # Horribly ugly...
	@${MAKE} -C modules clean

clean_protocols:
	@touch protocol/Makefile.inc
	@${MAKE} -C protocol clean

clean_core:
	@touch core/Makefile.inc
	@${MAKE} -C core clean

spotless:
	@${MAKE} -C modules distclean
	@${MAKE} -C protocol distclean
	@${MAKE} -C core distclean

install: services
	test -d ${INSTDIR} || mkdir ${INSTDIR}
	test -d ${INSTDIR}/bin || mkdir ${INSTDIR}/bin
	$(INSTALL) services $(INSTDIR)/bin/services
	$(INSTALL) bin/anoperc $(INSTDIR)/bin/anoperc
	test -d ${INSTDIR}/data || mkdir ${INSTDIR}/data
	(cd ../lang ; $(MAKE) install)
	$(CP) ../data/* $(INSTDIR)/data
	$(INSTALL) bin/mydbgen $(INSTDIR)/data/mydbgen
	test -d $(INSTDIR)/data/backups || mkdir $(INSTDIR)/data/backups
	test -d $(INSTDIR)/data/logs || mkdir $(INSTDIR)/data/logs
	@if [ "$(INSTDIR)/data/modules" ] ; then \
		test -d ${INSTDIR}/data/modules || mkdir ${INSTDIR}/data/modules ; \
		test -d ${INSTDIR}/data/modules/runtime || mkdir ${INSTDIR}/data/modules/runtime ; \
		(cd modules ; $(MAKE) install) ; \
		(cd protocol ; ${MAKE} install) ; \
		(cd core ; ${MAKE} install) ; \
	fi
	@if [ "$(RUNGROUP)" ] ; then \
		echo chgrp -R $(RUNGROUP) $(INSTDIR)/data ; \
		chgrp -R $(RUNGROUP) $(INSTDIR)/data ; \
		echo chmod -R g+rw $(INSTDIR)/data ; \
		chmod -R g+rw $(INSTDIR)/data ; \
		echo find $(INSTDIR)/data -type d -exec chmod g+xs \'\{\}\' \\\; ; \
		find $(INSTDIR)/data -type d -exec chmod g+xs '{}' \; ; \
	fi

DUMMY:
