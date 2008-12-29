
all: services.h extern.h pseudo.h version.h

version.h: Makefile version.sh services.h pseudo.h messages.h $(SRCS)
	sh version.sh

services.h: sysconf.h config.h extern.h
	touch $@

extern.h: slist.h
	touch $@

pseudo.h: commands.h timeout.h encrypt.h datafiles.h slist.h
	touch $@

clean: 
	(rm -f language.h)

distclean: clean
	(rm -f sysconf.h version.h)
