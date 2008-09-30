#!/bin/sh

echo "Generating build information using aclocal, autoheader, automake and autoconf."
echo

# Regerate configuration files
aclocal
autoheader
automake --gnu --add-missing --copy
autoconf

echo
echo "Now you are ready to run ./configure"
