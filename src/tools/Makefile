OBJS =  anopesmtp.o
SRCS =  anopesmtp.c

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 

.c.o:
	$(CC) $(CFLAGS) -I../include/ -c $<

all: anopesmtp

distclean: clean spotless

anopesmpt:  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(ANOPELIBS) $(MLIBS) -o $@ $(LDFLAGS) 

$(OBJS): Makefile
anopesmpt.o:    anopesmpt.c $(INCLUDES)

clean:
	rm -f *.o anopesmtp a.out

spotless: clean

install: anopesmtp
	test -d ${BINDEST} || mkdir ${BINDEST}
	$(INSTALL) anopesmtp $(BINDEST)/anopesmtp

DUMMY:
