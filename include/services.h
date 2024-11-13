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

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <bitset>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#ifndef _WIN32
# include <unistd.h>
#endif

#include "defs.h"
#include "sysconf.h"

#define BUFSIZE 1024

#define _(x) x
#define N_(x, y) x, y

#ifndef _WIN32
# define DllExport __attribute__ ((visibility ("default")))
# define CoreExport __attribute__ ((visibility ("default")))
# define anope_close close
#else
# include "anope_windows.h"
#endif
