# Makefile for Epona.
#
# Epona (c) 2000-2002 PegSoft
# Contact us at epona@pegsoft.net
#
# This program is free but copyrighted software; see the file COPYING for
# details.
#
# Based on the original code of Services by Andy Church.

include Makefile.inc

###########################################################################
########################## Configuration section ##########################

# Note that changing any of these options (or, in fact, anything in this
# file) will automatically cause a full rebuild of Services.

# Compilation options:
#	-DCLEAN_COMPILE	 Attempt to compile without any warnings (note that
#			    this may reduce performance)
#	-DSTREAMLINED    Leave out "fancy" options to enhance performance

CDEFS =

# Add any extra flags you want here.  The default line enables warnings and
# debugging symbols on GCC.  If you have a non-GCC compiler, you may want
# to comment it out or change it.

MORE_CFLAGS = -Wall -g


######################## End configuration section ########################
###########################################################################


CFLAGS = $(CDEFS) $(BASE_CFLAGS) $(MORE_CFLAGS)


OBJS =	actions.o botserv.o channels.o chanserv.o commands.o compat.o converter.o \
	config.o datafiles.o encrypt.o helpserv.o hostserv.o init.o language.o list.o log.o mail.o main.o \
	memory.o memoserv.o messages.o misc.o modules.o news.o nickserv.o operserv.o \
	process.o protocol.o proxy.o send.o sessions.o slist.o sockutil.o \
	timeout.o users.o \
	$(VSNPRINTF_O) $(RDB_O) $(MYSQL_O)
SRCS =	actions.c botserv.c channels.c chanserv.c commands.c compat.c converter.c \
	config.c datafiles.c encrypt.c helpserv.c hostserv.c init.c language.c list.c log.c mail.c main.c \
	memory.c memoserv.c messages.c misc.c modules.c news.c nickserv.c operserv.c \
	process.c protocol.c proxy.c send.c sessions.c slist.c sockutil.c \
	timeout.c users.c \
	$(VSNPRINTF_C) $(RDB_C) $(MYSQL_C)

.c.o:
	$(CC) $(CFLAGS) -c $<


all: $(PROGRAM) languages modules
	@echo Now run \"$(MAKE) install\" to install Services.

myclean:
	rm -f *.o $(PROGRAM) a.out


clean: myclean
	(cd lang ; $(MAKE) spotless)
	rm -f language.h
	(cd modules ; $(MAKE) clean)

spotless: myclean
	(cd lang ; $(MAKE) spotless)
	(cd modules ; rm -f *.so Makefile.inc)
	rm -f config.cache configure.log sysconf.h Makefile.inc language.h version.h *~

distclean: spotless

install: $(PROGRAM) languages
	$(INSTALL) services $(BINDEST)/services
	rm -f $(BINDEST)/listnicks $(BINDEST)/listchans
	ln $(BINDEST)/services $(BINDEST)/listnicks
	ln $(BINDEST)/services $(BINDEST)/listchans
	(cd lang ; $(MAKE) install)
	$(CP_ALL) data/* $(DATDEST)
	test -d $(DATDEST)/backups || mkdir $(DATDEST)/backups
	test -d $(DATDEST)/logs || mkdir $(DATDEST)/logs
	@if [ "$(MODULE_PATH)" ] ; then \
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
	@echo ""
	@echo "Don't forget to create/update your services.conf file!  See"
	@echo "the README for details."
	@echo ""

###########################################################################

$(PROGRAM): version.h $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) $(MLIBS) -o $@ $(ELIBS)

languages: FRC
	(cd lang ; $(MAKE) CFLAGS="$(CFLAGS)")

modules: FRC
	@if [ "$(MODULE_PATH)" ] ; then \
		(cd modules ; ./configure ; $(MAKE) CFLAGS="$(CFLAGS)") \
	fi

# Catch any changes in compilation options at the top of this file
$(OBJS): Makefile

actions.o:	actions.c	services.h
botserv.o:	botserv.c	services.h pseudo.h
channels.o:	channels.c	services.h
chanserv.o:	chanserv.c	services.h pseudo.h
commands.o:	commands.c	services.h commands.h language.h
compat.o:	compat.c	services.h
config.o:	config.c	services.h
converter.o:    converter.c services.h datafiles.h
datafiles.o:	datafiles.c	services.h datafiles.h
encrypt.o:	encrypt.c	encrypt.h sysconf.h
init.o:		init.c		services.h
hostserv.o:     hostserv.c      services.h pseudo.h
language.o:	language.c	services.h language.h
list.o:		list.c		services.h
log.o:		log.c		services.h pseudo.h
mail.o:		mail.c		services.h language.h
main.o:		main.c		services.h timeout.h version.h
memory.o:	memory.c	services.h
memoserv.o:	memoserv.c	services.h pseudo.h
messages.o:	messages.c	services.h messages.h language.h
misc.o:		misc.c		services.h language.h
news.o:		news.c		services.h pseudo.h
nickserv.o:	nickserv.c	services.h pseudo.h
operserv.o:	operserv.c	services.h pseudo.h
process.o:	process.c	services.h messages.h
protocol.o:	protocol.c	services.h
proxy.o:	proxy.c		services.h pseudo.h
send.o:		send.c		services.h
sessions.o:     sessions.c      services.h pseudo.h
slist.o:	slist.c		services.h slist.h 
sockutil.o:	sockutil.c	services.h
timeout.o:	timeout.c	services.h timeout.h
users.o:	users.c		services.h
vsnprintf.o:	vsnprintf.c


services.h: sysconf.h config.h extern.h
	touch $@

extern.h:	slist.h
	touch $@

pseudo.h: commands.h language.h timeout.h encrypt.h datafiles.h slist.h
	touch $@

version.h: Makefile version.sh version.log services.h pseudo.h messages.h $(SRCS)
	sh version.sh

language.h: lang/language.h
	cp -p lang/language.h .

lang/language.h: lang/Makefile lang/index
	(cd lang ; $(MAKE) language.h)

lang/index: 
	(cd lang ; $(MAKE) index)

###########################################################################

FRC:
