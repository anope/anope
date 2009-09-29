include ./Makefile.inc

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
      	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'PROFILE=${PROFILE}' 'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}'\
	   'MAKEBIN=${MAKEBIN}'

OBJECTS= $(SRCS:.c=.so)
OBJECTS+= $(SRCS:.cpp=.so)
CDEFS= -rdynamic -Wall

all: modules subs

modules: $(OBJECTS)

install:
	$(CP) ./*.so $(INSTDIR)/data/modules
	@for i in $(SUBS); do \
	echo "make install in $$i..."; \
	(cd $$i; $(MAKE) $(MAKEARGS) install);done

distclean: spotless

%.so: %.c
	$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -o $*.so $*.c

%.so: %.cpp
	$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -o $*.so $*.cpp

subs:
	@for i in $(SUBS); do \
        echo "make all in $$i..."; \
        (cd $$i; $(MAKE) $(MAKEARGS) all); done

subs_clean:
	@for i in $(SUBS); do \
	echo "cleaning in $$i..."; \
	(cd $$i; $(MAKE) $(MAKEARGS) clean); done

clean: subs_clean
	rm -f *.so

spotless:
	rm -f *.so Makefile.inc

