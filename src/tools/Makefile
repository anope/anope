SRCS =  anopesmtp.c db-convert.c
OBJS =  $(SRCS:.c=.o)

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' 'MAKEBIN=${MAKEBIN}'

.c.o:
	$(MAKEBIN) $(CC) $(CFLAGS) -I../../include/ -c $<

all: ${OBJS}

distclean: spotless

anopesmtp: anopesmtp.o
	$(CC) $(CFLAGS) anopesmtp.o $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS)
db-convert: db-convert.o
	$(CC) $(CFLAGS) db-convert.o $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS)

$(OBJS): Makefile
anopesmtp.o:    anopesmtp.c $(INCLUDES)
db-convert.o:	db-convert.c $(INCLUDES)

clean:
	rm -f *.o anopesmtp a.out db-convert

spotless: clean

install: anopesmtp db-convert
	test -d ${INSTDIR} || mkdir ${INSTDIR}
	test -d $(INSTDIR)/bin || mkdir $(INSTDIR)/bin
	$(INSTALL) anopesmtp $(INSTDIR)/bin/anopesmtp
	$(INSTALL) db-convert $(INSTDIR)/bin/db-convert

DUMMY:
