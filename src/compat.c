/* Compatibility routines.
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"

/*************************************************************************/

#if !HAVE_STRICMP && !HAVE_STRCASECMP

/* stricmp, strnicmp:  Case-insensitive versions of strcmp() and
 *                     strncmp().
 */

int stricmp(const char *s1, const char *s2)
{
    register int c;

    while ((c = tolower(*s1)) == tolower(*s2)) {
        if (c == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}

int strnicmp(const char *s1, const char *s2, size_t len)
{
    register int c;

    if (!len)
        return 0;
    while ((c = tolower(*s1)) == tolower(*s2) && len > 0) {
        if (c == 0 || --len == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}
#endif

/*************************************************************************/

