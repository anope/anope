include ./Makefile.inc

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'BINDEST=${BINDEST}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
      	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' 'DATDEST=${DATDEST}' \
   	   'RUNGROUP=${RUNGROUP}' 'MODULE_PATH=${MODULE_PATH}' \
	   'PROFILE=${PROFILE}' 'SHARED=${SHARED}'

OBJECTS= $(SRCS:.c=.o)
SO_FILES=$(OBJECTS:.o=.s)
CDEFS= -g -rdynamic -Wall
CFLAGS=$(CFLAGS) $(CDEFS)

all: modules subs

modules: $(OBJECTS) $(SO_FILES)

install: 
	$(CP) ./*.so $(MODULE_PATH) 

distclean: clean spotless

.c.o:
	$(CC) $(CFLAGS) -I../${INCLUDEDIR} -c $<

.o.s:
	$(CC) ${SHARED} $< -o $*.so ${PROFILE}

subs:
	@for i in $(SUBS); do \
        echo "make all in $$i..."; \
        (cd $$i; $(MAKE) $(MAKEARGS) all); done

clean:
	rm -f *.o *.so *.c~ core

spotless: clean
	rm -f *.so Makefile.inc

