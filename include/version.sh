#!/bin/sh
#
# Build version string and increment Services build number.
#

# Grab version information from the version control file.
CTRL="../version.log"
if [ -f $CTRL ] ; then
	. $CTRL
else
	echo "Error: Unable to find control file: $CTRL"
	exit 0
fi

VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_EXTRA} (${VERSION_BUILD})"
VERSIONDOTTED="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_EXTRA}.${VERSION_BUILD}"

if [ -f version.h ] ; then
	BUILD=`fgrep '#define BUILD' version.h | sed 's/^#define BUILD.*\([0-9]*\).*$/\1/'`
	BUILD=`expr $BUILD + 1 2>/dev/null`
else
	BUILD=1
fi
if [ ! "$BUILD" ] ; then
	BUILD=1
fi
cat >version.h <<EOF
/* Version information for Services.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and CREDITS for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * This file is auto-generated by version.sh
 *
 */

 #ifndef VERSION_H
 #define VERSION_H
 
#define VERSION_MAJOR	$VERSION_MAJOR
#define VERSION_MINOR	$VERSION_MINOR
#define VERSION_PATCH	$VERSION_PATCH
#define VERSION_EXTRA	"$VERSION_EXTRA"
#define VERSION_BUILD	$VERSION_BUILD

#define BUILD	"$BUILD"
#define VERSION_STRING "$VERSION"
#define VERSION_STRING_DOTTED "$VERSIONDOTTED"

#ifdef DEBUG_COMMANDS
# define VER_DEBUG "D"
#else
# define VER_DEBUG
#endif

#if defined(_WIN32)
# if _MSC_VER >= 1400
#  define VER_OS "W"
# else
#  define VER_OS "w"
# endif
#else
# define VER_OS
#endif

#if defined(USE_MYSQL)
# define VER_MYSQL "Q"
#else
# define VER_MYSQL
#endif

#if defined(USE_MODULES)
# define VER_MODULE "M"
#else
# define VER_MODULE
#endif

#endif

EOF

