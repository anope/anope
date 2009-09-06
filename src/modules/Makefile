include ./Makefile.inc

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
      	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'PROFILE=${PROFILE}' 'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}'\
	   'MAKEBIN=${MAKEBIN}'

OBJECTS= $(SRCS:.c=.o)
OBJECTS+= $(SRCS:.cpp=.o)
SO_FILES=$(OBJECTS:.o=.s)
CDEFS= -rdynamic -Wall

all: modules subs

modules: $(OBJECTS) $(SO_FILES)

install:
	$(CP) ./*.so $(INSTDIR)/data/modules
	@for i in $(SUBS); do \
	echo "make install in $$i..."; \
	(cd $$i; $(MAKE) $(MAKEARGS) install);done

distclean: spotless

.c.o:
	$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -c $<

.cpp.o:
	$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -c $<

.o.s:
	$(MAKEBIN) $(CC) ${SHARED} ../mod_version.o $< -o $*.so  ${PROFILE}
	@$(TOUCH) $*.s

subs:
	@for i in $(SUBS); do \
        echo "make all in $$i..."; \
        (cd $$i; $(MAKE) $(MAKEARGS) all); done

subs_clean:
	@for i in $(SUBS); do \
	echo "cleaning in $$i..."; \
	(cd $$i; $(MAKE) $(MAKEARGS) clean); done

clean: subs_clean
	rm -f *.o *.s *.so *.c~ core

spotless:
	rm -f *.o *.s *.so *.c~ core *.so Makefile.inc

