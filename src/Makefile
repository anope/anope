MYSQL_OBJ = $(MYSQL:.c=.o)
RDB_OBJ = $(RDB:.c=.o)
OBJS =  actions.o botserv.o channels.o chanserv.o commands.o compat.o converter.o \
        config.o datafiles.o encrypt.o helpserv.o hostserv.o init.o language.o list.o log.o mail.o main.o \
	memory.o memoserv.o messages.o misc.o modules.o news.o nickserv.o operserv.o \
	process.o protocol.o proxy.o send.o servers.o sessions.o slist.o sockutil.o \
	timeout.o users.o \
	$(VSNPRINTF_O) $(RDB_OBJ) $(MYSQL_OBJ)
SRCS =  actions.c botserv.c channels.c chanserv.c commands.c compat.c converter.c \
        config.c datafiles.c encrypt.c helpserv.c hostserv.c init.c language.c list.c log.c mail.c main.c \
        memory.c memoserv.c messages.c misc.c modules.c news.c nickserv.c operserv.c \
        process.c protocol.c proxy.c send.c servers.c sessions.c slist.c sockutil.c \
        timeout.c users.c \
        $(VSNPRINTF_C) $(RDB) $(MYSQL)

INCLUDES = ../include/commands.h ../include/defs.h  ../include/language.h \
	../include/pseudo.h ../include/sysconf.h ../include/config.h \
	../include/encrypt.h ../include/messages.h ../include/services.h \
	../include/timeout.h ../include/datafiles.h ../include/extern.h \
	../include/modules.h ../include/slist.h ../include/version.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 'MODULE_PATH=${MODULE_PATH}' 'MYSQL=${MYSQL}'\
	   'RDB=${RDB}'

.c.o:
	$(CC) $(CFLAGS) -I../include/ -c $<

all: services

services:  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(ELIBS) 

$(OBJS): Makefile
actions.o:      actions.c   $(INCLUDES)
botserv.o:      botserv.c   $(INCLUDES)    
channels.o:     channels.c  $(INCLUDES) 
chanserv.o:     chanserv.c  $(INCLUDES)   
commands.o:     commands.c  $(INCLUDES)   
compat.o:       compat.c    $(INCLUDES)
config.o:       config.c    $(INCLUDES) 
converter.o:    converter.c $(INCLUDES) 
datafiles.o:    datafiles.c $(INCLUDES)
encrypt.o:      encrypt.c   $(INCLUDES) 
init.o:         init.c      $(INCLUDES)
hostserv.o:     hostserv.c  $(INCLUDES)   
language.o:     language.c  $(INCLUDES)   
list.o:         list.c      $(INCLUDES)
log.o:          log.c       $(INCLUDES)
mail.o:         mail.c      $(INCLUDES)
main.o:         main.c      $(INCLUDES)
memory.o:       memory.c    $(INCLUDES)
memoserv.o:     memoserv.c  $(INCLUDES)
messages.o:     messages.c  $(INCLUDES)
misc.o:         misc.c      $(INCLUDES)
news.o:         news.c      $(INCLUDES)
nickserv.o:     nickserv.c  $(INCLUDES)
operserv.o:     operserv.c  $(INCLUDES)
process.o:      process.c   $(INCLUDES)
protocol.o:     protocol.c  $(INCLUDES)
proxy.o:        proxy.c     $(INCLUDES) 
send.o:         send.c      $(INCLUDES)
sessions.o:     sessions.c  $(INCLUDES)
slist.o:        slist.c     $(INCLUDES)
sockutil.o:     sockutil.c  $(INCLUDES)
timeout.o:      timeout.c   $(INCLUDES)
users.o:        users.c     $(INCLUDES)
vsnprintf.o:    vsnprintf.c $(INCLUDES)
mysql.o:        mysql.c     $(INCLUDES)
rdb.o:          rdb.c       $(INCLUDES)

modules: DUMMY
	(cd modules ; ./configure ; ${MAKE} ${MAKEARGS} all)

clean:
	(cd modules ; ${MAKE} ${MAKEARGS} clean)
	rm -f *.o services a.out

install: services
	test -d ${BINDEST} || mkdir ${BINDEST}
	$(INSTALL) services $(BINDEST)/services
	$(INSTALL) bin/anoperc $(BINDEST)/anoperc
	rm -f $(BINDEST)/listnicks $(BINDEST)/listchans
	ln $(BINDEST)/services $(BINDEST)/listnicks
	ln $(BINDEST)/services $(BINDEST)/listchans
	(cd ../lang ; $(MAKE) install)
	$(CP) ../data/* $(DATDEST)
	test -d $(DATDEST)/backups || mkdir $(DATDEST)/backups
	test -d $(DATDEST)/logs || mkdir $(DATDEST)/logs
	@if [ "$(MODULE_PATH)" ] ; then \
		test -d ${MODULE_PATH} || mkdir ${MODULE_PATH} ; \
		test -d ${MODULE_PATH}/runtime || mkdir ${MODULE_PATH}/runtime ; \
		(cd modules ; $(MAKE) install) ; \
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
