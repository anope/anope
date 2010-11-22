#!/bin/bash

HLANG=`cat ../include/language.h | grep "^\s" | sed 's/[\t,]//g'`
CPPLANG=`cat ../src/language.cpp | grep "\s\/\* [A-Z0-9_]* \*\/" | sed 's/\/\* \(.*\) \*\//\1/' | sed 's/\t//'`
echo "${HLANG} ${CPPLANG}" | sed 's/ /\n/' | sort | uniq -u

