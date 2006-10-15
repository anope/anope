MYSQL_OBJ = $(MYSQL:.c=.o)
RDB_OBJ = $(RDB:.c=.o)
OBJS =  actions.o base64.o botserv.o channels.o chanserv.o commands.o compat.o \
		config.o datafiles.o encrypt.o events.o helpserv.o hostserv.o init.o ircd.o language.o list.o log.o mail.o main.o \
		memory.o memoserv.o messages.o misc.o modules.o news.o nickserv.o operserv.o \
		process.o send.o servers.o sessions.o slist.o sockutil.o timeout.o users.o  \
		$(RDB_OBJ) $(MYSQL_OBJ) 
SRCS =  actions.c base64.c botserv.c channels.c chanserv.c commands.c compat.c \
		config.c datafiles.c encrypt.c events.c helpserv.c hostserv.c init.c ircd.c language.c list.c log.c mail.c main.c \
		memory.c memoserv.c messages.c misc.c modules.c news.c nickserv.c operserv.c \
		process.c send.c servers.c sessions.c slist.c sockutil.c timeout.c users.c \
		$(RDB) $(MYSQL) 

INCLUDES = ../include/commands.h ../include/defs.h  ../include/language.h \
	../include/pseudo.h ../include/sysconf.h ../include/config.h \
	../include/encrypt.h ../include/messages.h ../include/services.h \
	../include/timeout.h ../include/datafiles.h ../include/extern.h \
	../include/modules.h ../include/slist.h 

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 'MODULE_PATH=${MODULE_PATH}' 'MYSQL=${MYSQL}'\
	   'RDB=${RDB}' 'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}'

.c.o:
	$(CC) $(CFLAGS) -I../include/ -c $<

all: services

distclean: spotless
distclean_modules: clean_modules spotless

services:  $(OBJS) mod_version
	$(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS) 

$(OBJS): Makefile
actions.o:      actions.c   $(INCLUDES)
base64.o:       base64.c    $(INCLUDES)    
botserv.o:      botserv.c   $(INCLUDES)    
channels.o:     channels.c  $(INCLUDES) 
chanserv.o:     chanserv.c  $(INCLUDES)   
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
list.o:         list.c      $(INCLUDES)
log.o:          log.c       $(INCLUDES)
mail.o:         mail.c      $(INCLUDES)
main.o:         main.c      $(INCLUDES)
memory.o:       memory.c    $(INCLUDES)
memoserv.o:     memoserv.c  $(INCLUDES)
messages.o:     messages.c  $(INCLUDES)
modules.o:      modules.c   $(INCLUDES)
misc.o:         misc.c      $(INCLUDES)
news.o:         news.c      $(INCLUDES)
nickserv.o:     nickserv.c  $(INCLUDES)
operserv.o:     operserv.c  $(INCLUDES)
process.o:      process.c   $(INCLUDES)
send.o:         send.c      $(INCLUDES)
servers.o:      servers.c   $(INCLUDES)
sessions.o:     sessions.c  $(INCLUDES)
slist.o:        slist.c     $(INCLUDES)
sockutil.o:     sockutil.c  $(INCLUDES)
timeout.o:      timeout.c   $(INCLUDES)
users.o:        users.c     $(INCLUDES)
vsnprintf.o:    vsnprintf.c $(INCLUDES)
mysql.o:        mysql.c     $(INCLUDES)
rdb.o:          rdb.c       $(INCLUDES)

mod_version:	mod_version.c	$(INCLUDES)
	$(CC) $(CDEFS) $(CFLAGS) $(MODULEFLAGS) -I../include/ -c mod_version.c


modules: DUMMY
	(cd modules ; ./configure ; ${MAKE} ${MAKEARGS} all)

protocols: DUMMY
	(cd protocol ; ./configure ; ${MAKE} ${MAKEARGS} all)
core: DUMMY
	(cd core ; ./configure ; ${MAKE} ${MAKEARGS} all)

clean: clean_modules clean_protocols clean_core
	rm -f *.o services a.out
clean_modules:
	@touch modules/Makefile.inc # Horribly ugly...
	(cd modules ; ${MAKE} clean)

clean_protocols:
	@touch protocol/Makefile.inc
	(cd protocol ; ${MAKE} clean)

clean_core:
	@touch core/Makefile.inc
	(cd core ; ${MAKE} clean)

spotless:
	(cd modules ; ${MAKE} distclean)
	(cd protocol ; ${MAKE} distclean)
	(cd core ; ${MAKE} distclean)

install: services
	test -d ${BINDEST} || mkdir ${BINDEST}
	$(INSTALL) services $(BINDEST)/services
	$(INSTALL) bin/anoperc $(BINDEST)/anoperc
	rm -f $(BINDEST)/listnicks $(BINDEST)/listchans
	ln $(BINDEST)/services $(BINDEST)/listnicks
	ln $(BINDEST)/services $(BINDEST)/listchans
	(cd ../lang ; $(MAKE) install)
	$(CP) ../data/* $(DATDEST)
	$(INSTALL) bin/mydbgen $(DATDEST)/mydbgen
	test -d $(DATDEST)/backups || mkdir $(DATDEST)/backups
	test -d $(DATDEST)/logs || mkdir $(DATDEST)/logs
	@if [ "$(MODULE_PATH)" ] ; then \
		test -d ${MODULE_PATH} || mkdir ${MODULE_PATH} ; \
		test -d ${MODULE_PATH}/runtime || mkdir ${MODULE_PATH}/runtime ; \
		(cd modules ; $(MAKE) install) ; \
		(cd protocol ; ${MAKE} install) ; \
		(cd core ; ${MAKE} install) ; \
	fi
	@if [ "$(RUNGROUP)" ] ; then \
		echo chgrp -R $(RUNGROUP) $(DATDEST) ; \
		chgrp -R $(RUNGROUP) $(DATDEST) ; \
		echo chmod -R g+rw $(DATDEST) ; \
		chmod -R g+rw $(DATDEST) ; \
		echo find $(DATDEST) -type d -exec chmod g+xs \'\{\}\' \\\; ; \
		find $(DATDEST) -type d -exec chmod g+xs '{}' \; ; \
	fi

DUMMY:
