/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "sysconf.h"

#define BUFSIZE 1024

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstdint>
#include <stdexcept>

#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif

/* Pull in the various bits of STL */
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
#include <regex>
#include <stack>

#include "defs.h"

#define _(x) x

#ifndef _WIN32
# define DllExport
# define CoreExport
# define MARK_DEPRECATED __attribute((deprecated))
# define anope_close close
#else
# include "anope_windows.h"
#endif

#ifndef DEBUG_BUILD
# define NDEBUG
#endif
#include <assert.h>

#define anope_abstract = 0

