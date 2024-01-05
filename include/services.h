/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "sysconf.h"

#define BUFSIZE 1024

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>

#include <cstring>

#ifndef _WIN32
#include <unistd.h>
#endif

/* Pull in the various bits of STL */
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <exception>
#include <list>
#include <vector>
#include <deque>
#include <bitset>
#include <set>
#include <algorithm>
#include <iterator>

#include "defs.h"

#define _(x) x

#ifndef _WIN32
# define DllExport __attribute__ ((visibility ("default")))
# define CoreExport __attribute__ ((visibility ("default")))
# define anope_close close
#else
# include "anope_windows.h"
#endif
