include ./Makefile.inc

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
      	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'PROFILE=${PROFILE}' 'SHARED=${SHARED}' 'MODULEFLAGS=${MODULEFLAGS}'\
	   'MAKEBIN=${MAKEBIN}'

OBJECTS= $(SRCS:.cpp=.so)
CDEFS= -rdynamic -Wall

all: modules subs

modules: $(OBJECTS) $(SO_FILES)

install:
	$(CP) ./*.so $(INSTDIR)/data/modules

distclean: clean spotless

.SUFFIXES: .cpp .so

.cpp.so:
	$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -o $@ $<

subs:
	@for i in $(SUBS); do \
        echo "make all in $$i..."; \
        (cd $$i; $(MAKE) $(MAKEARGS) all); done

subs_clean:
	@for i in $(SUBS); do \
	echo "cleaning in $$i..."; \
	(cd $$i; $(MAKE) clean); done

clean: subs_clean
	rm -f *.so

spotless: subs_clean
	rm -f *.so Makefile.inc


