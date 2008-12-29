SRCS =  anopesmtp.c db-merger.c db-convert.c
OBJS =  $(SRCS:.c=.o)

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 'MAKEBIN=${MAKEBIN}'

.c.o:
	@$(MAKEBIN) $(CC) $(CFLAGS) -I../include/ -c $<

all: ${OBJS}

distclean: spotless

anopesmpt:  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS) 

$(OBJS): Makefile
anopesmtp.o:    anopesmtp.c $(INCLUDES)
db-merger.o:    db-merger.c $(INCLUDES)
db-convert.o:	db-convert.c $(INCLUDES)

clean:
	rm -f *.o anopesmtp a.out db-merger db-convert

spotless: clean

install: anopesmtp db-merger db-convert
	test -d ${BINDEST} || mkdir ${BINDEST}
	test -d $(BINDEST)/tools || mkdir $(BINDEST)/tools
	$(INSTALL) anopesmtp $(BINDEST)/tools/anopesmtp
	$(INSTALL) db-merger $(BINDEST)/tools/db-merger
	$(INSTALL) db-convert $(BINDEST)/tools/db-convert

DUMMY:
