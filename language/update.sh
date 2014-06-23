#!/bin/bash

rm -f anope.pot
touch anope.pot

cd ..
FILES=`find ./ -name *.cpp -o -name *.h -o -name *.conf | grep -v /modules/third/`
xgettext -E -C -s -d Anope -j -o language/anope.pot --from-code=utf-8 --keyword --keyword=_ $FILES
cd -

for f in *.po
do
	msgmerge -v -s -U $f `echo $f | cut -d'.' -f1`.pot
done

rm -f *~
