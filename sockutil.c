/* Socket utility routines.
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

/*************************************************************************/
/*************************************************************************/

/* Read from a socket with buffering. */

static char read_netbuf[NET_BUFSIZE];
static char *read_curpos = read_netbuf; /* Next byte to return */
static char *read_bufend = read_netbuf; /* Next position for data from socket */
static char *const read_buftop = read_netbuf + NET_BUFSIZE;
int32 total_read = 0;


/* Return amount of data in read buffer. */

int32 read_buffer_len()
{
    if (read_bufend >= read_curpos)
        return read_bufend - read_curpos;
    else
        return (read_bufend + NET_BUFSIZE) - read_curpos;
}


/* Read data. */

static int buffered_read(int fd, char *buf, int len)
{
    int nread, left = len;
    fd_set fds;
    struct timeval tv = { 0, 0 };
    int errno_save = errno;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    while (left > 0) {
        struct timeval *tvptr = (read_bufend == read_curpos ? NULL : &tv);
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        while (read_bufend != read_curpos - 1
               && !(read_curpos == read_netbuf
                    && read_bufend == read_buftop - 1)
               && select(fd + 1, &fds, 0, 0, tvptr) == 1) {
            int maxread;
            tvptr = &tv;        /* don't wait next time */
            if (read_bufend < read_curpos)      /* wrapped around? */
                maxread = (read_curpos - 1) - read_bufend;
            else if (read_curpos == read_netbuf)
                maxread = read_buftop - read_bufend - 1;
            else
                maxread = read_buftop - read_bufend;
            nread = read(fd, read_bufend, maxread);
            errno_save = errno;
            if (debug >= 3)
                alog("debug: buffered_read wanted %d, got %d", maxread,
                     nread);
            if (nread <= 0)
                break;
            read_bufend += nread;
            if (read_bufend == read_buftop)
                read_bufend = read_netbuf;
        }
        if (read_curpos == read_bufend) /* No more data on socket */
            break;
        /* See if we can gobble up the rest of the buffer. */
        if (read_curpos + left >= read_buftop && read_bufend < read_curpos) {
            nread = read_buftop - read_curpos;
            memcpy(buf, read_curpos, nread);
            buf += nread;
            left -= nread;
            read_curpos = read_netbuf;
        }
        /* Now everything we need is in a single chunk at read_curpos. */
        if (read_bufend > read_curpos && read_bufend - read_curpos < left)
            nread = read_bufend - read_curpos;
        else
            nread = left;
        if (nread) {
            memcpy(buf, read_curpos, nread);
            buf += nread;
            left -= nread;
            read_curpos += nread;
        }
    }
    total_read += len - left;
    if (debug >= 4) {
        alog("debug: buffered_read(%d,%p,%d) returning %d",
             fd, buf, len, len - left);
    }
    errno = errno_save;
    return len - left;
}

/* Optimized version of the above for reading a single character; returns
 * the character in an int or EOF, like fgetc(). */

static int buffered_read_one(int fd)
{
    int nread;
    fd_set fds;
    struct timeval tv = { 0, 0 };
    char c;
    struct timeval *tvptr = (read_bufend == read_curpos ? NULL : &tv);
    int errno_save = errno;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    while (read_bufend != read_curpos - 1
           && !(read_curpos == read_netbuf
                && read_bufend == read_buftop - 1)
           && select(fd + 1, &fds, 0, 0, tvptr) == 1) {
        int maxread;
        tvptr = &tv;            /* don't wait next time */
        if (read_bufend < read_curpos)  /* wrapped around? */
            maxread = (read_curpos - 1) - read_bufend;
        else if (read_curpos == read_netbuf)
            maxread = read_buftop - read_bufend - 1;
        else
            maxread = read_buftop - read_bufend;
        nread = read(fd, read_bufend, maxread);
        errno_save = errno;
        if (debug >= 3)
            alog("debug: buffered_read_one wanted %d, got %d", maxread,
                 nread);
        if (nread <= 0)
            break;
        read_bufend += nread;
        if (read_bufend == read_buftop)
            read_bufend = read_netbuf;
    }
    if (read_curpos == read_bufend) {   /* No more data on socket */
        if (debug >= 4)
            alog("debug: buffered_read_one(%d) returning %d", fd, EOF);
        errno = errno_save;
        return EOF;
    }
    c = *read_curpos++;
    if (read_curpos == read_buftop)
        read_curpos = read_netbuf;
    total_read++;
    if (debug >= 4)
        alog("debug: buffered_read_one(%d) returning %d", fd, c);
    return (int) c & 0xFF;
}

/*************************************************************************/

/* Write to a socket with buffering.  Note that this assumes only one
 * socket. */

static char write_netbuf[NET_BUFSIZE];
static char *write_curpos = write_netbuf;       /* Next byte to write to socket */
static char *write_bufend = write_netbuf;       /* Next position for data to socket */
static char *const write_buftop = write_netbuf + NET_BUFSIZE;
static int write_fd = -1;
int32 total_written;


/* Return amount of data in write buffer. */

int32 write_buffer_len()
{
    if (write_bufend >= write_curpos)
        return write_bufend - write_curpos;
    else
        return (write_bufend + NET_BUFSIZE) - write_curpos;
}


/* Helper routine to try and write up to one chunk of data from the buffer
 * to the socket.  Return how much was written. */

static int flush_write_buffer(int wait)
{
    fd_set fds;
    struct timeval tv = { 0, 0 };
    int errno_save = errno;

    if (write_bufend == write_curpos || write_fd == -1)
        return 0;
    FD_ZERO(&fds);
    FD_SET(write_fd, &fds);
    if (select(write_fd + 1, 0, &fds, 0, wait ? NULL : &tv) == 1) {
        int maxwrite, nwritten;
        if (write_curpos > write_bufend)        /* wrapped around? */
            maxwrite = write_buftop - write_curpos;
        else if (write_bufend == write_netbuf)
            maxwrite = write_buftop - write_curpos - 1;
        else
            maxwrite = write_bufend - write_curpos;
        nwritten = write(write_fd, write_curpos, maxwrite);
        errno_save = errno;
        if (debug >= 3)
            alog("debug: flush_write_buffer wanted %d, got %d", maxwrite,
                 nwritten);
        if (nwritten > 0) {
            write_curpos += nwritten;
            if (write_curpos == write_buftop)
                write_curpos = write_netbuf;
            total_written += nwritten;
            return nwritten;
        }
    }
    errno = errno_save;
    return 0;
}


/* Write data. */

static int buffered_write(int fd, char *buf, int len)
{
    int nwritten, left = len;
    int errno_save = errno;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    write_fd = fd;

    while (left > 0) {

        /* Don't try putting anything in the buffer if it's full. */
        if (write_curpos != write_bufend + 1 &&
            (write_curpos != write_netbuf
             || write_bufend != write_buftop - 1)) {
            /* See if we need to write up to the end of the buffer. */
            if (write_bufend + left >= write_buftop
                && write_curpos <= write_bufend) {
                nwritten = write_buftop - write_bufend;
                memcpy(write_bufend, buf, nwritten);
                buf += nwritten;
                left -= nwritten;
                write_bufend = write_netbuf;
            }
            /* Now we can copy a single chunk to write_bufend. */
            if (write_curpos > write_bufend
                && write_curpos - write_bufend - 1 < left)
                nwritten = write_curpos - write_bufend - 1;
            else
                nwritten = left;
            if (nwritten) {
                memcpy(write_bufend, buf, nwritten);
                buf += nwritten;
                left -= nwritten;
                write_bufend += nwritten;
            }
        }

        /* Now write to the socket as much as we can. */
        if (write_curpos == write_bufend + 1 ||
            (write_curpos == write_netbuf
             && write_bufend == write_buftop - 1))
            flush_write_buffer(1);
        else
            flush_write_buffer(0);
        errno_save = errno;
        if (write_curpos == write_bufend + 1 ||
            (write_curpos == write_netbuf
             && write_bufend == write_buftop - 1)) {
            /* Write failed on full buffer */
            break;
        }
    }

    if (debug >= 4) {
        alog("debug: buffered_write(%d,%p,%d) returning %d",
             fd, buf, len, len - left);
    }
    errno = errno_save;
    return len - left;
}

/* Optimized version of the above for writing a single character; returns
 * the character in an int or EOF, like fputc().  Commented out because it
 * isn't currently used. */

#if 0
static int buffered_write_one(int c, int fd)
{
    struct timeval tv = { 0, 0 };

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    write_fd = fd;

    /* Try to flush the buffer if it's full. */
    if (write_curpos == write_bufend + 1 ||
        (write_curpos == write_netbuf
         && write_bufend == write_buftop - 1)) {
        flush_write_buffer(1);
        if (write_curpos == write_bufend + 1 ||
            (write_curpos == write_netbuf
             && write_bufend == write_buftop - 1)) {
            /* Write failed */
            if (debug >= 4)
                alog("debug: buffered_write_one(%d) returning %d", fd,
                     EOF);
            return EOF;
        }
    }

    /* Write the character. */
    *write_bufend++ = c;
    if (write_bufend == write_buftop)
        write_bufend = write_netbuf;

    /* Move it to the socket if we can. */
    flush_write_buffer(0);

    if (debug >= 4)
        alog("debug: buffered_write_one(%d) returning %d", fd, c);
    return (int) c & 0xFF;
}
#endif                          /* 0 */

/*************************************************************************/
/*************************************************************************/

static int lastchar = EOF;

int sgetc(int s)
{
    int c;

    if (lastchar != EOF) {
        c = lastchar;
        lastchar = EOF;
        return c;
    }
    return buffered_read_one(s);
}

int sungetc(int c, int s)
{
    return lastchar = c;
}

/*************************************************************************/

/* If connection was broken, return NULL.  If the read timed out, return
 * (char *)-1.
 */

char *sgets(char *buf, int len, int s)
{
    int c = 0;
    struct timeval tv;
    fd_set fds;
    char *ptr = buf;

    if (len == 0)
        return NULL;
    FD_SET(s, &fds);
    tv.tv_sec = ReadTimeout;
    tv.tv_usec = 0;
    while (read_buffer_len() == 0 &&
           (c = select(s + 1, &fds, NULL, NULL, &tv)) < 0) {
        if (errno != EINTR)
            break;
    }
    if (read_buffer_len() == 0 && c == 0)
        return (char *) -1;
    c = sgetc(s);
    while (--len && (*ptr++ = c) != '\n' && (c = sgetc(s)) >= 0);
    if (c < 0)
        return NULL;
    *ptr = 0;
    return buf;
}

/*************************************************************************/

/* sgets2:  Read a line of text from a socket, and strip newline and
 *          carriage return characters from the end of the line.
 */

char *sgets2(char *buf, int len, int s)
{
    char *str = sgets(buf, len, s);

    if (!str || str == (char *) -1)
        return str;
    str = buf + strlen(buf) - 1;
    if (*str == '\n')
        *str-- = 0;
    if (*str == '\r')
        *str = 0;
    return buf;
}

/*************************************************************************/

/* Read from a socket.  (Use this instead of read() because it has
 * buffering.) */

int sread(int s, char *buf, int len)
{
    return buffered_read(s, buf, len);
}

/*************************************************************************/

int sputs(char *str, int s)
{
    return buffered_write(s, str, strlen(str));
}

/*************************************************************************/

int sockprintf(int s, char *fmt, ...)
{
    va_list args;
    char buf[16384];            /* Really huge, to try and avoid truncation */

    va_start(args, fmt);
    return buffered_write(s, buf, vsnprintf(buf, sizeof(buf), fmt, args));
}

/*************************************************************************/
/*************************************************************************/

#if !HAVE_GETHOSTBYNAME

/* Translate an IP dotted-quad address to a 4-byte character string.
 * Return NULL if the given string is not in dotted-quad format.
 */

static char *pack_ip(const char *ipaddr)
{
    static char ipbuf[4];
    int tmp[4], i;

    if (sscanf(ipaddr, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3])
        != 4)
        return NULL;
    for (i = 0; i < 4; i++) {
        if (tmp[i] < 0 || tmp[i] > 255)
            return NULL;
        ipbuf[i] = tmp[i];
    }
    return ipbuf;
}

#endif

/*************************************************************************/

/* lhost/lport specify the local side of the connection.  If they are not
 * given (lhost==NULL, lport==0), then they are left free to vary.
 */

int conn(const char *host, int port, const char *lhost, int lport)
{
#if HAVE_GETHOSTBYNAME
    struct hostent *hp;
#else
    char *addr;
#endif
    struct sockaddr_in sa, lsa;
    int sock;

    memset(&lsa, 0, sizeof(lsa));
    if (lhost) {
#if HAVE_GETHOSTBYNAME
        if ((hp = gethostbyname(lhost)) != NULL) {
            memcpy((char *) &lsa.sin_addr, hp->h_addr, hp->h_length);
            lsa.sin_family = hp->h_addrtype;
#else
        if (addr = pack_ip(lhost)) {
            memcpy((char *) &lsa.sin_addr, addr, 4);
            lsa.sin_family = AF_INET;
#endif
        } else {
            lhost = NULL;
        }
    }
    if (lport)
        lsa.sin_port = htons((unsigned short) lport);

    memset(&sa, 0, sizeof(sa));
#if HAVE_GETHOSTBYNAME
    if (!(hp = gethostbyname(host)))
        return -1;
    memcpy((char *) &sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
#else
    if (!(addr = pack_ip(host))) {
        alog("conn(): `%s' is not a valid IP address", host);
        errno = EINVAL;
        return -1;
    }
    memcpy((char *) &sa.sin_addr, addr, 4);
    sa.sin_family = AF_INET;
#endif
    sa.sin_port = htons((unsigned short) port);

    if ((sock = socket(sa.sin_family, SOCK_STREAM, 0)) < 0)
        return -1;

    if ((lhost || lport)
        && bind(sock, (struct sockaddr *) &lsa, sizeof(lsa)) < 0) {
        int errno_save = errno;
        close(sock);
        errno = errno_save;
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        int errno_save = errno;
        close(sock);
        errno = errno_save;
        return -1;
    }

    return sock;
}

/*************************************************************************/

void disconn(int s)
{
    shutdown(s, 2);
    close(s);
}
