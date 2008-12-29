SRCS =  anopesmtp.c db-merger.c db-convert.c
OBJS =  $(SRCS:.c=.o)

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
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
	test -d ${INSTDIR} || mkdir ${INSTDIR}
	test -d $(INSTDIR)/tools || mkdir $(INSTDIR)/tools
	$(INSTALL) anopesmtp $(INSTDIR)/tools/anopesmtp
	$(INSTALL) db-merger $(INSTDIR)/tools/db-merger
	$(INSTALL) db-convert $(INSTDIR)/tools/db-convert

DUMMY:
