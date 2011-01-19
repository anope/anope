SRCS =  anopesmtp.c db-merger.c epona2anope.c
OBJS =  $(SRCS:.c=.o)

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 

.c.o:
	$(CC) $(CFLAGS) -I../include/ $< -o $*

all: ${OBJS}

distclean: spotless

$(OBJS): Makefile
anopesmtp.o:    anopesmtp.c $(INCLUDES)
db-merger.o:    db-merger.c $(INCLUDES)
epona2anope.o:  epona2anope.c $(INCLUDES)

clean:
	rm -f *.o anopesmtp a.out db-merger epona2anope

spotless: clean

install:
	test -d ${BINDEST} || mkdir ${BINDEST}
	test -d $(BINDEST)/tools || mkdir $(BINDEST)/tools
	$(INSTALL) anopesmtp $(BINDEST)/tools/anopesmtp
	$(INSTALL) db-merger $(BINDEST)/tools/db-merger
	$(INSTALL) epona2anope $(BINDEST)/tools/epona2anope

DUMMY:
