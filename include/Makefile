all: services.h extern.h version.h
	./version ../src/version.sh version.h

version.h: Makefile services.h version.cpp $(SRCS)
	$(CC) version.cpp -o version

services.h: sysconf.h config.h extern.h
	touch $@

clean:
	(rm -f language.h)

distclean: clean
	(rm -f sysconf.h version.h version)
