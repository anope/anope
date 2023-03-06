#!/bin/sh

if [ "${PWD##*/}" != "language" ]
then
	echo "Please run this script in the language/ subfolder of an anope source tree."
	exit 1
fi

# truncate
: > anope.pot

# find .cpp, .h, and .conf files
# exclude docs and third party modules
# run xgettext on found files
find ../ \
	! -path '../docs/*' \
	-a ! -path '../modules/third/*' \
	-a \( -name '*.cpp' \
		-o -name '*.h' \
		-o -name '*.conf' \
	\) \
	-exec \
		xgettext \
			--escape \
			--language=C++ \
			--sort-output \
			--default-domain=Anope \
			--join-existing \
			--output=anope.pot \
			--from-code=utf-8 \
			--keyword \
			--keyword=_ \
			{} +

for f in *.po
do
	echo "Merging $f"
	msgmerge \
		--no-location \
		--no-wrap \
		--sort-output \
		--update \
		--verbose \
		"${f}" \
		"${f%%.*}.pot"
done

rm -f -- *~
