OBJS =  actions.o base64.o bots.o botserv.o channels.o chanserv.o command.o commands.o compat.o \
		config.o configreader.o encrypt.o hashcomp.o hostserv.o init.o ircd.o language.o log.o mail.o main.o \
		memory.o memoserv.o messages.o misc.o modes.o modules.o module.o modulemanager.o nickalias.o \
		nickcore.o nickserv.o operserv.o process.o protocol.o regchannel.o send.o servers.o sessions.o \
		sockets.o threadengine.o threadengine_pthread.o timers.o opertype.o users.o wildcard.o

INCLUDES = ../include/commands.h ../include/language.h \
	../include/sysconf.h ../include/config.h \
	../include/services.h ../include/regchannel.h \
	../include/timers.h ../include/extern.h \
	../include/modules.h ../include/operserv.h ../include/hashcomp.h \
	../include/threadengine.h ../include/mail.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}' \
	   'MAKEBIN=${MAKEBIN}' 'MYSQLDIR=${MYSQLDIR}'

.cpp.o:
	$(MAKEBIN) $(CC) $(CFLAGS) -I../include/ -c $<

all: services

distclean: spotless
distclean_modules: clean_modules spotless

services:  $(OBJS) 
	$(MAKEBIN) $(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS)

$(OBJS): Makefile
actions.o:      actions.cpp   $(INCLUDES)
base64.o:       base64.cpp    $(INCLUDES)
bots.o:         bots.cpp    $(INCLUDES)
botserv.o:      botserv.cpp   $(INCLUDES)
channels.o:     channels.cpp  $(INCLUDES)
chanserv.o:     chanserv.cpp  $(INCLUDES)
command.o:      command.cpp  $(INCLUDES)
commands.o:     commands.cpp  $(INCLUDES)
compat.o:       compat.cpp    $(INCLUDES)
config.o:       config.cpp    $(INCLUDES)
configreader.o: configreader.cpp $(INCLUDES)
encrypt.o:      encrypt.cpp   $(INCLUDES)
init.o:         init.cpp      $(INCLUDES)
ircd.o:         ircd.cpp      $(INCLUDES)
hostserv.o:     hostserv.cpp  $(INCLUDES)
language.o:     language.cpp  $(INCLUDES)
log.o:          log.cpp       $(INCLUDES)
mail.o:         mail.cpp      $(INCLUDES)
main.o:         main.cpp      $(INCLUDES)
memory.o:       memory.cpp    $(INCLUDES)
memoserv.o:     memoserv.cpp  $(INCLUDES)
messages.o:     messages.cpp  $(INCLUDES)
modes.o:        modes.cpp $(INCLUDES)
modules.o:      modules.cpp   $(INCLUDES)
module.o:       module.cpp   $(INCLUDES)
modulemanager.o:      modulemanager.cpp   $(INCLUDES)
misc.o:         misc.cpp      $(INCLUDES)
nickalias.o:    nickalias.cpp $(INCLUDES)
nickcore.o:     nickcore.cpp $(INCLUDES)
nickserv.o:     nickserv.cpp  $(INCLUDES)
operserv.o:     operserv.cpp  $(INCLUDES)
opertype.o:     opertype.cpp  $(INCLUDES)
process.o:      process.cpp   $(INCLUDES)
protocol.o:     protocol.cpp  $(INCLUDES)
regchannel.o:	regchannel.cpp $(INCLUDES)
send.o:         send.cpp      $(INCLUDES)
servers.o:      servers.cpp   $(INCLUDES)
sessions.o:     sessions.cpp  $(INCLUDES)
slist.o:        slist.cpp     $(INCLUDES)
sockets.o:       sockets.cpp  $(INCLUDES)
threadengine.o: threadengine.cpp $(INCLUDES)
threadengine_pthread.o: threadengine_pthread.cpp $(INCLUDES)
timers.o:       timers.cpp  $(INCLUDES)
users.o:        users.cpp     $(INCLUDES)
wildcard.o:     wildcard.cpp $(INCLUDES)

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
	$(CP) ../data/*.* $(INSTDIR)/data
	$(INSTALL) bin/mydbgen $(INSTDIR)/bin/mydbgen
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
