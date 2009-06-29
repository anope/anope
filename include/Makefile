
all: services.h extern.h pseudo.h version.h

version.h: Makefile version.sh services.h pseudo.h messages.h $(SRCS)
	sh version.sh ../version.log $@

services.h: sysconf.h config.h extern.h
	touch $@

extern.h: slist.h
	touch $@

pseudo.h: commands.h timers.h datafiles.h slist.h
	touch $@

clean:
	(rm -f language.h)

distclean: clean
	(rm -f sysconf.h version.h)
