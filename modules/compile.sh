#!/bin/sh

# Disabled for Anope 1.6 until we figure out a better way to
# download modules safely.
# if [ "$1" = "getmods" ] ; then
#         wget -nv -r -l 1 -A c -nd -nc http://modules.anope.org/download/
#         rm -f robots.txt
#         exit 0
# fi

if [ ! -f ../Makefile.inc ]; then
	echo ""
	echo "*** ERROR: Unable to find ../Makefile.inc. You must ./configure Anope before"
	echo "*** ERROR: compiling modules. Please read the INSTALL document for details."
	echo ""
	exit 1
fi

echo -n "SRCS=" > ./Makefile.inc
FIRST=1
for oldfile in *.c
do
	if [ "$FIRST" = 1 ] ; then
		echo -n " "$oldfile >> ./Makefile.inc
	else
		echo "\\" >> ./Makefile.inc
		echo -n "     " $oldfile >> ./Makefile.inc
	fi
	FIRST=0
done
echo "" >> ./Makefile.inc

make

if [ "$1" = "install" ] ; then
	make install
fi
