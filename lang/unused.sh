#!/bin/bash

for string in `egrep "^.+[A-Z]+," ../include/language.h | sed "s/\t\(.*\),/\1/"` ; do
	CMD=`grep -RI $string {../src,../modules,../include} | grep -v "language\.cpp:.*\/\* $string" | grep -v "language\.h:"`
	if [ ! -n "$CMD" ] ; then
		echo "$string is unused"
		sed -i "s/\/\* $string \*\//\/* $string - UNUSED *\//" ../src/language.cpp
		sed -i "/$string,/d" ../include/language.h
	fi
done
