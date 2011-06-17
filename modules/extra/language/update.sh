#!/bin/bash

for f in `ls ../*.cpp`
do
	BASE=`basename $f | cut -d'.' -f1`
	xgettext -C -s -d $BASE -o $BASE.pot --from-code=utf-8 --keyword --keyword=_ $f
done

for f in `ls *.po`
do
	msgmerge -v -s -U $f `basename $f | cut -d'.' -f1`.pot
done

rm -f *~
