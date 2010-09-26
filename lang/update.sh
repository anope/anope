#!/bin/bash

pushd ../src > /dev/null
xgettext -C -s -d Anope -o ../lang/anope.pot --from-code=utf-8 --keyword=_ language.cpp
popd > /dev/null

for f in *.po
do
	msgmerge -v -s -U $f `echo $f | cut -d'.' -f1`.pot
done
