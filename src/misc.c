
/* Miscellaneous routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"
#include "language.h"

/* Cheaper than isspace() or isblank() */
#define issp(c) ((c) == 32)

/*************************************************************************/

/* toupper/tolower:  Like the ANSI functions, but make sure we return an
 *                   int instead of a (signed) char.
 */

int toupper(char c)
{
    if (islower(c))
        return (unsigned char) c - ('a' - 'A');
    else
        return (unsigned char) c;
}

int tolower(char c)
{
    if (isupper(c))
        return (unsigned char) c + ('a' - 'A');
    else
        return (unsigned char) c;
}

/*************************************************************************/

/* strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *           add a null terminator after the last character copied.
 */

char *strscpy(char *d, const char *s, size_t len)
{
    char *d_orig = d;

    if (!len)
        return d;
    while (--len && (*d++ = *s++));
    *d = '\0';
    return d_orig;
}

/*************************************************************************/

/* stristr:  Search case-insensitively for string s2 within string s1,
 *           returning the first occurrence of s2 or NULL if s2 was not
 *           found.
 */

char *stristr(char *s1, char *s2)
{
    register char *s = s1, *d = s2;

    while (*s1) {
        if (tolower(*s1) == tolower(*d)) {
            s1++;
            d++;
            if (*d == 0)
                return s;
        } else {
            s = ++s1;
            d = s2;
        }
    }
    return NULL;
}

/*************************************************************************/

/* strnrepl:  Replace occurrences of `old' with `new' in string `s'.  Stop
 *            replacing if a replacement would cause the string to exceed
 *            `size' bytes (including the null terminator).  Return the
 *            string.
 */

char *strnrepl(char *s, int32 size, const char *old, const char *new)
{
    char *ptr = s;
    int32 left = strlen(s);
    int32 avail = size - (left + 1);
    int32 oldlen = strlen(old);
    int32 newlen = strlen(new);
    int32 diff = newlen - oldlen;

    while (left >= oldlen) {
        if (strncmp(ptr, old, oldlen) != 0) {
            left--;
            ptr++;
            continue;
        }
        if (diff > avail)
            break;
        if (diff != 0)
            memmove(ptr + oldlen + diff, ptr + oldlen, left + 1);
        strncpy(ptr, new, newlen);
        ptr += newlen;
        left -= oldlen;
    }
    return s;
}

/*************************************************************************/
/*************************************************************************/

/* merge_args:  Take an argument count and argument vector and merge them
 *              into a single string in which each argument is separated by
 *              a space.
 */

char *merge_args(int argc, char **argv)
{
    int i;
    static char s[4096];
    char *t;

    t = s;
    for (i = 0; i < argc; i++)
        t += snprintf(t, sizeof(s) - (t - s), "%s%s", *argv++,
                      (i < argc - 1) ? " " : "");
    return s;
}

/*************************************************************************/
/*************************************************************************/

/* match_wild:  Attempt to match a string to a pattern which might contain
 *              '*' or '?' wildcards.  Return 1 if the string matches the
 *              pattern, 0 if not.
 */

static int do_match_wild(const char *pattern, const char *str, int docase)
{
    char c;
    const char *s;

    /* This WILL eventually terminate: either by *pattern == 0, or by a
     * trailing '*'. */

    for (;;) {
        switch (c = *pattern++) {
        case 0:
            if (!*str)
                return 1;
            return 0;
        case '?':
            if (!*str)
                return 0;
            str++;
            break;
        case '*':
            if (!*pattern)
                return 1;       /* trailing '*' matches everything else */
            s = str;
            while (*s) {
                if ((docase ? (*s == *pattern)
                     : (tolower(*s) == tolower(*pattern)))
                    && do_match_wild(pattern, s, docase))
                    return 1;
                s++;
            }
            break;
        default:
            if (docase ? (*str++ != c) : (tolower(*str++) != tolower(c)))
                return 0;
            break;
        }                       /* switch */
    }
}


int match_wild(const char *pattern, const char *str)
{
    return do_match_wild(pattern, str, 1);
}

int match_wild_nocase(const char *pattern, const char *str)
{
    return do_match_wild(pattern, str, 0);
}

/*************************************************************************/
/*************************************************************************/

/* Process a string containing a number/range list in the form
 * "n1[-n2][,n3[-n4]]...", calling a caller-specified routine for each
 * number in the list.  If the callback returns -1, stop immediately.
 * Returns the sum of all nonnegative return values from the callback.
 * If `count' is non-NULL, it will be set to the total number of times the
 * callback was called.
 *
 * The callback should be of type range_callback_t, which is defined as:
 *	int (*range_callback_t)(User *u, int num, va_list args)
 */

int process_numlist(const char *numstr, int *count_ret,
                    range_callback_t callback, User * u, ...)
{
    int n1, n2, i;
    int res = 0, retval = 0, count = 0;
    va_list args;

    va_start(args, u);

    /*
     * This algorithm ignores invalid characters, ignores a dash
     * when it precedes a comma, and ignores everything from the
     * end of a valid number or range to the next comma or null.
     */
    for (;;) {
        n1 = n2 = strtol(numstr, (char **) &numstr, 10);
        numstr += strcspn(numstr, "0123456789,-");
        if (*numstr == '-') {
            numstr++;
            numstr += strcspn(numstr, "0123456789,");
            if (isdigit(*numstr)) {
                n2 = strtol(numstr, (char **) &numstr, 10);
                numstr += strcspn(numstr, "0123456789,-");
            }
        }
        for (i = n1; i <= n2 && i >= 0; i++) {
            int res = callback(u, i, args);
            count++;
            if (res < 0)
                break;
            retval += res;
            if (count >= 32767) {
                if (count_ret)
                    *count_ret = count;
                return retval;
            }
        }
        if (res < -1)
            break;
        numstr += strcspn(numstr, ",");
        if (*numstr)
            numstr++;
        else
            break;
    }
    if (count_ret)
        *count_ret = count;
    return retval;
}

/*************************************************************************/

/* dotime:  Return the number of seconds corresponding to the given time
 *          string.  If the given string does not represent a valid time,
 *          return -1.
 *
 *          A time string is either a plain integer (representing a number
 *          of seconds), or an integer followed by one of these characters:
 *          "s" (seconds), "m" (minutes), "h" (hours), or "d" (days).
 */

int dotime(const char *s)
{
    int amount;

    amount = strtol(s, (char **) &s, 10);
    if (*s) {
        switch (*s) {
        case 's':
            return amount;
        case 'm':
            return amount * 60;
        case 'h':
            return amount * 3600;
        case 'd':
            return amount * 86400;
        default:
            return -1;
        }
    } else {
        return amount;
    }
}

/*************************************************************************/

/* Expresses in a string the period of time represented by a given amount
   of seconds (with days/hours/minutes). */

char *duration(NickAlias * na, char *buf, int bufsize, time_t seconds)
{
    int days = 0, hours = 0, minutes = 0;
    int need_comma = 0;

    char buf2[64], *end;
    char *comma = getstring(na, COMMA_SPACE);

    /* We first calculate everything */
    days = seconds / 86400;
    seconds -= (days * 86400);
    hours = seconds / 3600;
    seconds -= (hours * 3600);
    minutes = seconds / 60;

    if (!days && !hours && !minutes) {
        snprintf(buf, bufsize,
                 getstring(na,
                           (seconds <=
                            1 ? DURATION_SECOND : DURATION_SECONDS)),
                 seconds);
    } else {
        end = buf;
        if (days) {
            snprintf(buf2, sizeof(buf2),
                     getstring(na,
                               (days == 1 ? DURATION_DAY : DURATION_DAYS)),
                     days);
            end += snprintf(end, bufsize - (end - buf), "%s", buf2);
            need_comma = 1;
        }
        if (hours) {
            snprintf(buf2, sizeof(buf2),
                     getstring(na,
                               (hours ==
                                1 ? DURATION_HOUR : DURATION_HOURS)),
                     hours);
            end +=
                snprintf(end, bufsize - (end - buf), "%s%s",
                         (need_comma ? comma : ""), buf2);
            need_comma = 1;
        }
        if (minutes) {
            snprintf(buf2, sizeof(buf2),
                     getstring(na,
                               (minutes ==
                                1 ? DURATION_MINUTE : DURATION_MINUTES)),
                     minutes);
            end +=
                snprintf(end, bufsize - (end - buf), "%s%s",
                         (need_comma ? comma : ""), buf2);
            need_comma = 1;
        }
    }

    return buf;
}

/*************************************************************************/

/* Generates a human readable string of type "expires in ..." */

char *expire_left(NickAlias * na, char *buf, int len, time_t expires)
{
    time_t now = time(NULL);

    if (!expires) {
        strncpy(buf, getstring(na, NO_EXPIRE), len);
    } else if (expires <= now) {
        strncpy(buf, getstring(na, EXPIRES_SOON), len);
    } else {
        time_t diff = expires - now + 59;

        if (diff >= 86400) {
            int days = diff / 86400;
            snprintf(buf, len,
                     getstring(na, (days == 1) ? EXPIRES_1D : EXPIRES_D),
                     days);
        } else {
            if (diff <= 3600) {
                int minutes = diff / 60;
                snprintf(buf, len,
                         getstring(na,
                                   (minutes ==
                                    1) ? EXPIRES_1M : EXPIRES_M), minutes);
            } else {
                int hours = diff / 3600, minutes;
                diff -= (hours * 3600);
                minutes = diff / 60;
                snprintf(buf, len,
                         getstring(na,
                                   ((hours == 1
                                     && minutes ==
                                     1) ? EXPIRES_1H1M : ((hours == 1
                                                           && minutes !=
                                                           1) ? EXPIRES_1HM
                                                          : ((hours != 1
                                                              && minutes ==
                                                              1) ?
                                                             EXPIRES_H1M :
                                                             EXPIRES_HM)))),
                         hours, minutes);
            }
        }
    }

    return buf;
}

/**
 * Return 1 if a host is valid, 0 if it isnt.
 * host = string to check
 * type = format, 1 = ip4addr, 2 = hostname
 *
 * shortname  =  ( letter / digit ) *( letter / digit / "-" ) *( letter / digit )
 * hostname   =  shortname *( "." shortname )
 * ip4addr    =  1*3digit "." 1*3digit "." 1*3digit "." 1*3digit
 *
 **/

int doValidHost(const char *host, int type)
{
    int idx = 0;
    int len = 0;
    int sec_len = 0;
    int dots = 1;
    if (type != 1 && type != 2) {
        return 0;
    }
    if (!host) {
        return 0;
    }

    len = strlen(host);

    if (len > HOSTMAX) {
        return 0;
    }

    switch (type) {
    case 1:
        for (idx = 0; idx < len; idx++) {
            if (isdigit(host[idx])) {
                if (sec_len < 3) {
                    sec_len++;
                } else {
                    return 0;
                }
            } else {
                if (idx == 0) {
                    return 0;
                }               /* cant start with a non-digit */
                if (host[idx] != '.') {
                    return 0;
                }               /* only . is a valid non-digit */
                if (sec_len > 3) {
                    return 0;
                }               /* sections cant be more than 3 digits */
                sec_len = 0;
                dots++;
            }
        }
        if (dots != 4) {
            return 0;
        }
        break;
    case 2:
        dots = 0;
        for (idx = 0; idx < len; idx++) {
            if (!isalnum(host[idx])) {
                if (idx == 0) {
                    return 0;
                }
                if ((host[idx] != '.') && (host[idx] != '-')) {
                    return 0;
                }
                if (host[idx] == '.') {
                    dots++;
                }
            }
        }
        if (host[len - 1] == '.') {
            return 0;
        }
        /**
	 * Ultimate3 dosnt like a non-dotted hosts at all, nor does unreal,
	 * so just dont allow them.
	 **/
        if (dots == 0) {
            return 0;
        }

        break;
    }
    return 1;
}

/**
 * Return 1 if a host is valid, 0 if it isnt.
 * host = string to check
 * type = format, 1 = ip4addr, 2 = hostname, 3 = either
 *
 * shortname  =  ( letter / digit ) *( letter / digit / "-" ) *( letter / digit )
 * hostname   =  shortname *( "." shortname )
 * ip4addr    =  1*3digit "." 1*3digit "." 1*3digit "." 1*3digit
 *
 **/
int isValidHost(const char *host, int type)
{
    int status = 0;
    if (type == 3) {
        if (!(status = doValidHost(host, 1))) {
            status = doValidHost(host, 2);
        }
    } else {
        status = doValidHost(host, type);
    }
    return status;
}

int isvalidchar(const char c)
{
    if (((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')) ||
        ((c >= '0') && (c <= '9')) || (c == '.') || (c == '-'))
        return 1;
    else
        return 0;
}

char *myStrGetToken(const char *str, const char dilim, int token_number)
{
    int len, idx, counter = 0, start_pos = 0;
    char *substring = NULL;
    if (!str) {
        return NULL;
    }
    len = strlen(str);
    for (idx = 0; idx <= len; idx++) {
        if ((str[idx] == dilim) || (idx == len)) {
            if (counter == token_number) {
                substring = myStrSubString(str, start_pos, idx);
                counter++;
            } else {
                start_pos = idx + 1;
                counter++;
            }
        }
    }
    return substring;
}

char *myStrGetOnlyToken(const char *str, const char dilim,
                        int token_number)
{
    int len, idx, counter = 0, start_pos = 0;
    char *substring = NULL;
    if (!str) {
        return NULL;
    }
    len = strlen(str);
    for (idx = 0; idx <= len; idx++) {
        if (str[idx] == dilim) {
            if (counter == token_number) {
                if (str[idx] == '\r')
                    substring = myStrSubString(str, start_pos, idx - 1);
                else
                    substring = myStrSubString(str, start_pos, idx);
                counter++;
            } else {
                start_pos = idx + 1;
                counter++;
            }
        }
    }
    return substring;
}

char *myStrGetTokenRemainder(const char *str, const char dilim,
                             int token_number)
{
    int len, idx, counter = 0, start_pos = 0;
    char *substring = NULL;
    if (!str) {
        return NULL;
    }
    len = strlen(str);

    for (idx = 0; idx <= len; idx++) {
        if ((str[idx] == dilim) || (idx == len)) {
            if (counter == token_number) {
                substring = myStrSubString(str, start_pos, len);
                counter++;
            } else {
                start_pos = idx + 1;
                counter++;
            }
        }
    }
    return substring;
}

char *myStrSubString(const char *src, int start, int end)
{
    char *substring = NULL;
    int len, idx;
    if (!src) {
        return NULL;
    }
    len = strlen(src);
    if (((start >= 0) && (end <= len)) && (end > start)) {
        substring = (char *) malloc(sizeof(char) * ((end - start) + 1));
        for (idx = 0; idx <= end - start; idx++) {
            substring[idx] = src[start + idx];
        }
        substring[end - start] = '\0';
    }
    return substring;
}

void doCleanBuffer(char *str)
{
    char *in = str;
    char *out = str;
    char ch;

    while (issp(ch = *in++));
    if (ch != '\0')
        for (;;) {
            *out++ = ch;
            ch = *in++;
            if (ch == '\0')
                break;
            if (!issp(ch))
                continue;
            while (issp(ch = *in++));
            if (ch == '\0')
                break;
            *out++ = ' ';
        }
    *out = ch;                  // == '\0'
}

void EnforceQlinedNick(char *nick, char *killer)
{
    User *u2;

    if ((u2 = finduser(nick))) {
        alog("Killed Q-lined nick: %s!%s@%s", u2->nick, u2->username,
             u2->host);
        kill_user(killer, u2->nick,
                  "This nick is reserved for Services. Please use a non Q-Lined nick.");
    }
}

int nickIsServices(char *nick)
{
    int found = 0;

    if (s_NickServ && (stricmp(nick, s_NickServ) == 0))
        found++;
    else if (s_ChanServ && (stricmp(nick, s_ChanServ) == 0))
        found++;
    else if (s_HostServ && (stricmp(nick, s_HostServ) == 0))
        found++;
    else if (s_MemoServ && (stricmp(nick, s_MemoServ) == 0))
        found++;
    else if (s_BotServ && (stricmp(nick, s_BotServ) == 0))
        found++;
    else if (s_HelpServ && (stricmp(nick, s_HelpServ) == 0))
        found++;
    else if (s_OperServ && (stricmp(nick, s_OperServ) == 0))
        found++;
    else if (s_DevNull && (stricmp(nick, s_DevNull) == 0))
        found++;
    else if (s_GlobalNoticer && (stricmp(nick, s_GlobalNoticer) == 0))
        found++;
    else if (s_NickServAlias && (stricmp(nick, s_NickServAlias) == 0))
        found++;
    else if (s_ChanServAlias && (stricmp(nick, s_ChanServAlias) == 0))
        found++;
    else if (s_MemoServAlias && (stricmp(nick, s_MemoServAlias) == 0))
        found++;
    else if (s_BotServAlias && (stricmp(nick, s_BotServAlias) == 0))
        found++;
    else if (s_HelpServAlias && (stricmp(nick, s_HelpServAlias) == 0))
        found++;
    else if (s_OperServAlias && (stricmp(nick, s_OperServAlias) == 0))
        found++;
    else if (s_DevNullAlias && (stricmp(nick, s_DevNullAlias) == 0))
        found++;
    else if (s_HostServAlias && (stricmp(nick, s_HostServAlias) == 0))
        found++;
    else if (s_GlobalNoticerAlias
             && (stricmp(nick, s_GlobalNoticerAlias) == 0))
        found++;
    else if (s_BotServ) {
        BotInfo *bi;
        int i;
        for (i = 0; i < 256; i++) {
            for (bi = botlists[i]; bi; bi = bi->next) {
                if (stricmp(nick, bi->nick) == 0) {
                    found++;
                    continue;
                }
            }
        }
    }

    return found;
}


