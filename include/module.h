#ifndef MODULE_H
#define MODULE_H

#include "services.h"
#include "commands.h"
#include "modules.h"

#if GETTEXT_FOUND
# include <libintl.h>
# define _(x) gettext(x)
#else
# define _(x) x
#endif

#endif // MODULE_H
