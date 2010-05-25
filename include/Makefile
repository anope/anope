
all: services.h extern.h version.h

version.h: Makefile version.sh services.h $(SRCS)
	sh version.sh ../version.log $@

services.h: sysconf.h config.h extern.h
	touch $@

clean:
	(rm -f language.h)

distclean: clean
	(rm -f sysconf.h version.h)
