/* base64 routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

/*
   This is borrowed from Unreal
*/

#include "services.h"

static char *int_to_base64(long);
static long base64_to_int(char *);

char *base64enc(long i)
{
    if (i < 0)
        return ("0");
    return int_to_base64(i);
}

long base64dec(char *b64)
{
    if (b64)
        return base64_to_int(b64);
    else
        return 0;
}

/* ':' and '#' and '&' and '+' and '@' must never be in this table. */
/* these tables must NEVER CHANGE! >) */
char int6_to_base64_map[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D',
    'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '{', '}'
};

char base64_to_int6_map[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
    -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, 63, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static char *int_to_base64(long val)
{
    /* 32/6 == max 6 bytes for representation, 
     * +1 for the null, +1 for byte boundaries 
     */
    static char base64buf[8];
    long i = 7;

    base64buf[i] = '\0';

    /* Temporary debugging code.. remove before 2038 ;p.
     * This might happen in case of 64bit longs (opteron/ia64),
     * if the value is then too large it can easily lead to
     * a buffer underflow and thus to a crash. -- Syzop
     */
    if (val > 2147483647L) {
        abort();
    }

    do {
        base64buf[--i] = int6_to_base64_map[val & 63];
    }
    while (val >>= 6);

    return base64buf + i;
}

static long base64_to_int(char *b64)
{
    int v = base64_to_int6_map[(u_char) * b64++];

    if (!b64)
        return 0;

    while (*b64) {
        v <<= 6;
        v += base64_to_int6_map[(u_char) * b64++];
    }

    return v;
}

long base64dects(char *ts)
{
    char *token;

    if (!ts) {
        return 0;
    }
    token = myStrGetToken(ts, '!', 1);

    if (!token) {
        return strtoul(ts, NULL, 10);;
    }
    return base64dec(token);
}
