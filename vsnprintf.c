/* An implementation of vsnprintf() for systems that don't have it.
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

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

typedef int (*_pfmt_writefunc_t)
 (const char *buf, size_t len, void *arg1, void *arg2);

int my_vsnprintf(char *string, size_t size, const char *format,
                 va_list args);

/*************************************************************************/

/* Basic format routine for *printf() interfaces.  Takes a writing function
 * and two (optional) parameters, one pointer and one integer, for that
 * function which are passed on unmodified.  The function should return the
 * number of bytes written, and 0 (not -1!) on write failure.
 */

static int _pfmt(const char *format, va_list args,
                 _pfmt_writefunc_t writefunc, void *arg1, void *arg2)
{
    int total = 0;              /* Total bytes written */
    const char *startptr;       /* Beginning of non-token text in format string.
                                 *    Used for writing in bulk instead of
                                 *    character-at-a-time. */
    int n;                      /* Bytes written in last writefunc() call */
    int valid;                  /* Was this a valid %-token? */
    int alt_form;               /* "Alternate form"? (# flag) */
    int zero_pad;               /* Zero-pad value? */
    int left_justify;           /* Left-justify?  (0 means right-justify) */
    int always_sign;            /* Always add sign value? */
    int width;                  /* Field width */
    int precision;              /* Precision */
    int argsize;                /* Size of argument: 0=normal, 1=short, 2=long,
                                 *    3=long long */
    int what;                   /* What are we working on?  0=flags, 1=width,
                                 *    2=precision, 3=argsize, 4=argtype */
    long intval;                /* Integer value */
    char *strval;               /* String value */
    void *ptrval;               /* Pointer value */
    char numbuf[64];            /* Temporary buffer for printing numbers */
    char *numptr;               /* Pointer to start of printed number in numbuf */


    intval = 0;
    strval = NULL;
    ptrval = NULL;

    startptr = format;
    while (*format) {
        if (*format != '%') {
            format++;
            continue;
        }
        if (startptr != format) {
            /* Write out accumulated text */
            n = writefunc(startptr, format - startptr, arg1, arg2);
            total += n;
            /* Abort on short write */
            if (n != format - startptr)
                break;
            /* Point to this token, in case it's a bad one */
            startptr = format;
        }

        valid = 0;              /* 1 if valid, -1 if known not valid (syntax error) */
        alt_form = 0;
        left_justify = 0;
        always_sign = 0;
        zero_pad = 0;
        width = -1;
        precision = -1;
        argsize = 0;
        what = 0;

        while (!valid && *++format) {   /* Broken out of by terminal chars */
            switch (*format) {

                /* Flags */
            case '#':
                if (what != 0) {
                    valid = -1;
                    break;
                }
                alt_form = 1;
                break;
            case '-':
                if (what != 0) {
                    valid = -1;
                    break;
                }
                left_justify = 1;
                break;
            case '+':
                if (what != 0) {
                    valid = -1;
                    break;
                }
                always_sign = 1;
                break;
            case '0':
                if (what == 0) {
                    zero_pad = 1;
                    break;
                }
                /* else fall through */

                /* Field widths */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (what == 0)
                    what = 1;
                else if (what > 2) {
                    valid = -1;
                    break;
                }
                if (what == 1) {
                    if (width < 0)
                        width = 0;
                    width = width * 10 + (*format) - '0';
                } else {
                    if (precision < 0)
                        precision = 0;
                    precision = precision * 10 + (*format) - '0';
                }
                break;
            case '*':
                if (what == 0)
                    what = 1;
                else if (what >= 2) {
                    valid = -1;
                    break;
                }
                if (what == 1) {
                    width = va_arg(args, int);
                    if (width < 0) {
                        width = -width;
                        left_justify = 1;
                    }
                } else {
                    precision = va_arg(args, int);
                }
                break;
            case '.':
                if (what >= 2) {
                    valid = -1;
                    break;
                }
                what = 2;
                break;

                /* Argument sizes */
            case 'h':
                if (what > 3) {
                    valid = -1;
                    break;
                }
                argsize = 1;
                what = 4;
                break;
            case 'l':
                if (what > 3) {
                    valid = -1;
                    break;
                }
                argsize = 2;
                what = 4;
                break;
            case 'L':
                if (what > 3) {
                    valid = -1;
                    break;
                }
                argsize = 3;
                what = 4;
                break;

                /* Argument types */
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                if (argsize == 1)
                    intval = va_arg(args, short);
                else if (argsize == 2)
                    intval = va_arg(args, long);
                else if (argsize == 3)
                    /* XXX we don't handle long longs yet */
                    intval = va_arg(args, long);
                else
                    intval = va_arg(args, int);
                valid = 1;
                break;
            case 'c':
                intval = va_arg(args, unsigned char);
                valid = 1;
                break;
            case 's':
                strval = va_arg(args, char *);
                valid = 1;
                break;
            case 'p':
                ptrval = va_arg(args, void *);
                valid = 1;
                break;
            case 'n':
                *((int *) va_arg(args, int *)) = total;
                valid = 1;
                break;

                /* All other characters--this neatly catches "%%" too */
            default:
                valid = -1;
                break;
            }                   /* switch (*format) */
        }
        if (valid != 1) {
            /* Not a valid %-token; start loop over (token will get printed
             * out next time through). */
            continue;
        }

        /* Don't zero-pad if a precision was given or left-justifying */
        if (precision != -1 || left_justify)
            zero_pad = 0;

        /* For numbers, limit precision to the size of the print buffer */
        if ((*format == 'd' || *format == 'i' || *format == 'o'
             || *format == 'u' || *format == 'x' || *format == 'X')
            && precision > (signed) sizeof(numbuf)) {
            precision = sizeof(numbuf);
        }

        switch (*format++) {    /* Do something with this token */
        case 'p':
            /* Print the NULL value specially */
            if (ptrval == NULL) {
                total += writefunc("(null)", 6, arg1, arg2);
                break;
            }
            /* For all other values, pretend it's really %#.8x */
            alt_form = 1;
            zero_pad = 0;
            precision = 8;
            intval = (long) ptrval;
            /* Fall through */

        case 'x':
        case 'X':{
                static const char x_chars[] = "0123456789abcdef0x";
                static const char X_chars[] = "0123456789ABCDEF0X";
                const char *chars =
                    (format[-1] == 'X') ? X_chars : x_chars;
                const char *padstr = zero_pad ? "0" : " ";
                unsigned long uintval;
                int len;

                uintval = (unsigned long) intval;
                if (alt_form && uintval != 0) {
                    n = writefunc(chars + 16, 2, arg1, arg2);
                    total += n;
                    if (n != 2)
                        break;
                    width -= 2;
                }
                if (precision < 1)
                    precision = 1;
                numptr = numbuf + sizeof(numbuf);
                for (len = 0; len < precision || uintval != 0; len++) {
                    *--numptr = chars[uintval % 16];
                    uintval /= 16;
                }
                if (left_justify) {
                    n = writefunc(numptr, len, arg1, arg2);
                    total += n;
                    if (n != len)
                        break;
                }
                while (len < width) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify)
                    total += writefunc(numptr, len, arg1, arg2);
                break;
            }                   /* case 'x', 'X' */

        case 'o':{
                const char *padstr = zero_pad ? "0" : " ";
                unsigned long uintval;
                int len;

                uintval = (unsigned long) intval;
                if (precision < 1)
                    precision = 1;
                numptr = numbuf + sizeof(numbuf);
                for (len = 0; len < precision || uintval != 0; len++) {
                    *--numptr = '0' + uintval % 8;
                    uintval /= 8;
                }
                if (alt_form && *numptr != '0') {
                    *--numptr = '0';
                    len++;
                }
                if (left_justify) {
                    n = writefunc(numptr, len, arg1, arg2);
                    total += n;
                    if (n != len)
                        break;
                }
                while (len < width) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify)
                    total += writefunc(numptr, len, arg1, arg2);
                break;
            }                   /* case 'o' */

            if (alt_form && *numptr != '0')
                *--numptr = '0';
        case 'u':{
                const char *padstr = zero_pad ? "0" : " ";
                unsigned long uintval;
                int len;

                uintval = (unsigned long) intval;
                if (precision < 1)
                    precision = 1;
                numptr = numbuf + sizeof(numbuf);
                for (len = 0; len < precision || uintval != 0; len++) {
                    *--numptr = '0' + uintval % 10;
                    uintval /= 10;
                }
                if (left_justify) {
                    n = writefunc(numptr, len, arg1, arg2);
                    total += n;
                    if (n != len)
                        break;
                }
                while (len < width) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify)
                    total += writefunc(numptr, len, arg1, arg2);
                break;
            }                   /* case 'u' */

        case 'd':
        case 'i':{
                const char *padstr = zero_pad ? "0" : " ";
                int len;

                numptr = numbuf + sizeof(numbuf);
                len = 0;
                if (intval < 0) {
                    if (1 != writefunc("-", 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                    intval = -intval;
                    if (intval < 0) {   /* true for 0x800...0 */
                        *numptr-- = '0' - intval % 10;
                        len++;
                        intval /= 10;
                        intval = -intval;
                    }
                } else if (intval >= 0 && always_sign) {
                    if (1 != writefunc("+", 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (precision < 1)
                    precision = 1;
                for (; len < precision || intval != 0; len++) {
                    *--numptr = '0' + intval % 10;
                    intval /= 10;
                }
                if (left_justify) {
                    n = writefunc(numptr, len, arg1, arg2);
                    total += n;
                    if (n != len)
                        break;
                }
                while (len < width) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify)
                    total += writefunc(numptr, len, arg1, arg2);
                break;
            }                   /* case 'd', 'i' */

        case 'c':{
                const char *padstr = zero_pad ? "0" : " ";
                unsigned char c = (unsigned char) intval;

                if (left_justify) {
                    if (1 != writefunc(&c, 1, arg1, arg2))
                        break;
                    total++;
                }
                while (width > 1) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify)
                    total += writefunc(&c, 1, arg1, arg2);
                break;
            }                   /* case 'c' */

        case 's':{
                const char *padstr = zero_pad ? "0" : " ";
                int len;

                /* Catch null strings */
                if (strval == NULL) {
                    total += writefunc("(null)", 6, arg1, arg2);
                    break;
                }
                len = strlen(strval);
                if (precision < 0 || precision > len)
                    precision = len;
                if (left_justify && len > 0) {
                    n = writefunc(strval, precision, arg1, arg2);
                    total += n;
                    if (n != precision)
                        break;
                }
                while (width > precision) {
                    if (1 != writefunc(padstr, 1, arg1, arg2))
                        break;
                    total++;
                    width--;
                }
                if (!left_justify && len > 0)
                    total += writefunc(strval, precision, arg1, arg2);
                break;
            }                   /* case 's' */

        }                       /* switch (*format++) */

        startptr = format;      /* Start again after this %-token */
    }                           /* while (*format) */

    /* Write anything left over. */
    if (startptr != format)
        total += writefunc(startptr, format - startptr, arg1, arg2);

    /* Return total bytes written. */
    return total;
}

/*************************************************************************/

static int writefunc(const char *buf, size_t len, char **string,
                     size_t * size)
{
    if (*size <= 0)
        return 0;
    if (len > (*size) - 1)
        len = (*size) - 1;
    if (len == 0)
        return 0;
    memcpy(*string, buf, len);
    (*string) += len;
    (*string)[0] = 0;
    return len;
}

int my_vsnprintf(char *string, size_t size, const char *format,
                 va_list args)
{
    int ret;

    if (size <= 0)
        return 0;
    ret =
        _pfmt(format, args, (_pfmt_writefunc_t) writefunc, &string, &size);
    return ret;
}

/*************************************************************************/
