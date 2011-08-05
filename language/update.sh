#!/bin/bash

rm anope.pot
touch anope.pot

cd ..
FILES=`find ./ -name *.cpp -o -name *.h | grep -v /modules/third/`
for f in $FILES
do
	xgettext -C -s -d Anope -j -o language/anope.pot --from-code=utf-8 --keyword --keyword=_ $f
done

cd -

for f in *.po
do
	msgmerge -v -s -U $f `echo $f | cut -d'.' -f1`.pot
done

rm -f *~
