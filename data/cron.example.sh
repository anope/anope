#!/bin/sh

# Crontab script for Anope
#
# To know how to install the crontab, read the INSTALL file.

###############################################################
# CONFIGURATION
###############################################################

# Anope binary directory
ANOPATH=/home/ircd/anope/bin

# Anope data directory
ANODATA=/home/ircd/anope/data

# Name of the pid file
ANOPIDF=anope.pid

# Name of the executable
ANOPROG=anope

# Parameters to pass to the executable
ANOARGS=""
#ANOARGS="-debug"

###############################################################
#                 DON'T EDIT ANYTHING BELOW                   #
###############################################################

PATH=/bin:/usr/bin:/usr/local/bin

ANOPID=

cd $ANOPATH

if [ -f "$ANODATA/$ANOPIDF" ]
then
	ANOPID=`cat "$ANODATA/$ANOPIDF"`
	kill -0 $ANOPID 2>/dev/null
	if [ $? -eq 0 ]
	then
		exit
	fi
	rm -f "$ANODATA/$ANOPIDF"
fi

./$ANOPROG $ANOARGS
