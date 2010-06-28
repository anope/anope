SRCS =  anopesmtp.cpp db-convert.cpp
OBJS =  $(SRCS:.cpp=)

INCLUDES = ../../include/services.h

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
     	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' 'MAKEBIN=${MAKEBIN}'

.cpp:
	$(MAKEBIN) $(CC) $(CFLAGS) -I../../include/ ../hashcomp.o $< -o $@

all: ${OBJS}

distclean: spotless

$(OBJS): Makefile
anopesmtp:    anopesmtp.cpp $(INCLUDES)
db-convert:	  db-convert.cpp $(INCLUDES)

clean:
	rm -f *.o anopesmtp a.out db-convert

spotless: clean

install:
	test -d ${INSTDIR} || mkdir ${INSTDIR}
	test -d $(INSTDIR)/bin || mkdir $(INSTDIR)/bin
	$(INSTALL) anopesmtp $(INSTDIR)/bin/anopesmtp
	$(INSTALL) db-convert $(INSTDIR)/bin/db-convert

DUMMY:
