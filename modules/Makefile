include ../Makefile.inc
include ./Makefile.inc

OBJECTS= $(SRCS:.c=.o)
SO_FILES=$(OBJECTS:.o=.s)
CDEFS= -g -rdynamic -Wall
CFLAGS=$(BASE_CFLAGS) $(CDEFS)

all: $(OBJECTS)

install:  $(SO_FILES)
	$(CP_ALL) ./*.so $(MODULE_PATH) 

distclean: clean spotless

.c.o:
	$(CC) $(CFLAGS) -c $<

.o.s:
	ld -shared $< -o $*.so

clean:
	rm -f *.o core

spotless: clean
	rm -f *.so Makefile.inc
