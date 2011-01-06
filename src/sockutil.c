/* Socket utility routines.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#include "services.h"

/*************************************************************************/

static char read_netbuf[NET_BUFSIZE];
static char *read_curpos = read_netbuf; /* Next byte to return */
static char *read_bufend = read_netbuf; /* Next position for data from socket */
static char *const read_buftop = read_netbuf + NET_BUFSIZE;
int32 total_read = 0;
static char write_netbuf[NET_BUFSIZE];
static char *write_curpos = write_netbuf;       /* Next byte to write to socket */
static char *write_bufend = write_netbuf;       /* Next position for data to socket */
static char *const write_buftop = write_netbuf + NET_BUFSIZE;
static int write_fd = -1;
int32 total_written;
static int lastchar = EOF;

/*************************************************************************/

/**
 * Return amount of data in read buffer.
 * @return int32
 */
int32 read_buffer_len()
{
    if (read_bufend >= read_curpos) {
        return read_bufend - read_curpos;
    } else {
        return (read_bufend + NET_BUFSIZE) - read_curpos;
    }
}

/*************************************************************************/

/**
 * Read data.
 * @param fd File Pointer
 * @param buf Buffer
 * @param int Length of buffer
 * @return int
 */
static int buffered_read(ano_socket_t fd, char *buf, int len)
{
    int nread, left = len;
    fd_set fds;
    struct timeval tv = { 0, 0 };
    int errno_save = ano_sockgeterr();

    if (fd < 0) {
        ano_sockseterr(SOCKERR_EBADF);
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
            nread = ano_sockread(fd, read_bufend, maxread);
            errno_save = ano_sockgeterr();
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
    ano_sockseterr(errno_save);
    return len - left;
}

/*************************************************************************/

/**
 * Optimized version of the above for reading a single character; returns
 * the character in an int or EOF, like fgetc().
 * @param fd File Pointer
 * @return int
 */
static int buffered_read_one(ano_socket_t fd)
{
    int nread;
    fd_set fds;
    struct timeval tv = { 0, 0 };
    char c;
    struct timeval *tvptr = (read_bufend == read_curpos ? NULL : &tv);
    int errno_save = ano_sockgeterr();

    if (fd < 0) {
        ano_sockseterr(SOCKERR_EBADF);
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
        nread = ano_sockread(fd, read_bufend, maxread);
        errno_save = ano_sockgeterr();
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
        ano_sockseterr(errno_save);
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

/**
 * Return amount of data in write buffer.
 * @return int
 */
int32 write_buffer_len()
{
    if (write_bufend >= write_curpos) {
        return write_bufend - write_curpos;
    } else {
        return (write_bufend + NET_BUFSIZE) - write_curpos;
    }
}

/*************************************************************************/

/**
 * Helper routine to try and write up to one chunk of data from the buffer
 * to the socket.  Return how much was written.
 * @param wait Wait
 * @return int
 */
static int flush_write_buffer(int wait)
{
    fd_set fds;
    struct timeval tv = { 0, 0 };
    int errno_save = ano_sockgeterr();

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
        nwritten = ano_sockwrite(write_fd, write_curpos, maxwrite);
        errno_save = ano_sockgeterr();
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
    ano_sockseterr(errno_save);
    return 0;
}

/*************************************************************************/

/**
 * Write data.
 * @param fd File Pointer
 * @param buf Buffer to write
 * @param len Length to write
 * @return int
 */
static int buffered_write(ano_socket_t fd, char *buf, int len)
{
    int nwritten, left = len;
    int errno_save = ano_sockgeterr();

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
    ano_sockseterr(errno_save);
    return len - left;
}


/*************************************************************************/

/**
 * Optimized version of the above for writing a single character; returns
 * the character in an int or EOF, like fputc().  Commented out because it
 * isn't currently used.
 * @param int to write
 * @param fd Pointer
 * @return int
 */
#if 0
static int buffered_write_one(int c, ano_socket_t fd)
{
    struct timeval tv = { 0, 0 };

    if (fd < 0) {
        ano_sockseterr(SOCKERR_EBADF);
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

/**
 * sgetc ?
 * @param int to read
 * @return int
 */
int sgetc(ano_socket_t s)
{
    int c;

    if (lastchar != EOF) {
        c = lastchar;
        lastchar = EOF;
        return c;
    }
    return buffered_read_one(s);
}

/*************************************************************************/

/**
 * sungetc ?
 * @param int c
 * @param int s
 * @return int
 */
int sungetc(int c, int s)
{
    return lastchar = c;
}

/*************************************************************************/

/**
 * If connection was broken, return NULL.  If the read timed out, return
 * (char *)-1.
 * @param buf Buffer to get
 * @param len Length
 * @param s Socket
 * @return buffer
 */
char *sgets(char *buf, int len, ano_socket_t s)
{
    int c = 0;
    struct timeval tv;
    fd_set fds;
    char *ptr = buf;

    flush_write_buffer(0);

    if (len == 0)
        return NULL;
    FD_ZERO(&fds);
    FD_SET(s, &fds);
    tv.tv_sec = ReadTimeout;
    tv.tv_usec = 0;
    while (read_buffer_len() == 0 &&
           (c = select(s + 1, &fds, NULL, NULL, &tv)) < 0) {
        if (ano_sockgeterr() != EINTR)
            break;
        flush_write_buffer(0);
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

/**
 * sgets2:  Read a line of text from a socket, and strip newline and
 *          carriage return characters from the end of the line.
 * @param buf Buffer to get
 * @param len Length
 * @param s Socket
 * @return buffer
 */
char *sgets2(char *buf, int len, ano_socket_t s)
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

/**
 * Read from a socket.  (Use this instead of read() because it has
 * buffering.)
 * @param s Socket
 * @param buf Buffer to get
 * @param len Length
 * @return int
 */
int sread(ano_socket_t s, char *buf, int len)
{
    return buffered_read(s, buf, len);
}

/*************************************************************************/

/**
 * sputs : write buffer
 * @param s Socket
 * @param str Buffer to write
 * @return int
 */
int sputs(char *str, ano_socket_t s)
{
    return buffered_write(s, str, strlen(str));
}

/*************************************************************************/

/**
 * sockprintf : a socket writting printf()
 * @param s Socket
 * @param fmt format of message
 * @param ... various args
 * @return int
 */
int sockprintf(ano_socket_t s, char *fmt, ...)
{
    va_list args;
    char buf[16384];            /* Really huge, to try and avoid truncation */
    int value;

    va_start(args, fmt);
    value = buffered_write(s, buf, vsnprintf(buf, sizeof(buf), fmt, args));
    va_end(args);
    return value;
}

/*************************************************************************/

#if !HAVE_GETHOSTBYNAME

/**
 * Translate an IP dotted-quad address to a 4-byte character string.
 * Return NULL if the given string is not in dotted-quad format.
 * @param ipaddr IP Address
 * @return char 4byte ip char string
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

/**
 * lhost/lport specify the local side of the connection.  If they are not
 * given (lhost==NULL, lport==0), then they are left free to vary.
 * @param host Remote Host
 * @param port Remote Port
 * @param lhost LocalHost
 * @param lport LocalPort
 * @return int if successful
 */
int conn(const char *host, int port, const char *lhost, int lport)
{
#if HAVE_GETHOSTBYNAME
    struct hostent *hp;
#else
    char *addr;
#endif
    struct sockaddr_in sa, lsa;
    ano_socket_t sock;
    int sockopt = 1;

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
        ano_sockseterr(SOCKERR_EINVAL);
        return -1;
    }
    memcpy((char *) &sa.sin_addr, addr, 4);
    sa.sin_family = AF_INET;
#endif
    sa.sin_port = htons((unsigned short) port);

    if ((sock = socket(sa.sin_family, SOCK_STREAM, 0)) < 0)
        return -1;

    if (setsockopt
        (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &sockopt,
         sizeof(int)) < 0)
        alog("debug: couldn't set SO_REUSEADDR on socket");

    if ((lhost || lport)
        && bind(sock, (struct sockaddr *) &lsa, sizeof(lsa)) < 0) {
        int errno_save = ano_sockgeterr();
        ano_sockclose(sock);
        ano_sockseterr(errno_save);
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        int errno_save = ano_sockgeterr();
        ano_sockclose(sock);
        ano_sockseterr(errno_save);
        return -1;
    }

    return sock;
}

/*************************************************************************/

/**
 * Close up the connection
 * @param s Socket
 * @return void
 */
void disconn(ano_socket_t s)
{
    shutdown(s, 2);
    ano_sockclose(s);
}

/*************************************************************************/
/* Windows support functions */

#ifdef _WIN32
/* Microsoft makes things nice and fun for us! */
struct u_WSA_errors {
    int error_code;
    char *error_string;
};

/* Must be sorted ascending by error code */
struct u_WSA_errors WSAErrors[] = {
    {WSAEINTR, "Interrupted system call"},
    {WSAEBADF, "Bad file number"},
    {WSAEACCES, "Permission denied"},
    {WSAEFAULT, "Bad address"},
    {WSAEINVAL, "Invalid argument"},
    {WSAEMFILE, "Too many open sockets"},
    {WSAEWOULDBLOCK, "Operation would block"},
    {WSAEINPROGRESS, "Operation now in progress"},
    {WSAEALREADY, "Operation already in progress"},
    {WSAENOTSOCK, "Socket operation on non-socket"},
    {WSAEDESTADDRREQ, "Destination address required"},
    {WSAEMSGSIZE, "Message too long"},
    {WSAEPROTOTYPE, "Protocol wrong type for socket"},
    {WSAENOPROTOOPT, "Bad protocol option"},
    {WSAEPROTONOSUPPORT, "Protocol not supported"},
    {WSAESOCKTNOSUPPORT, "Socket type not supported"},
    {WSAEOPNOTSUPP, "Operation not supported on socket"},
    {WSAEPFNOSUPPORT, "Protocol family not supported"},
    {WSAEAFNOSUPPORT, "Address family not supported"},
    {WSAEADDRINUSE, "Address already in use"},
    {WSAEADDRNOTAVAIL, "Can't assign requested address"},
    {WSAENETDOWN, "Network is down"},
    {WSAENETUNREACH, "Network is unreachable"},
    {WSAENETRESET, "Net connection reset"},
    {WSAECONNABORTED, "Software caused connection abort"},
    {WSAECONNRESET, "Connection reset by peer"},
    {WSAENOBUFS, "No buffer space available"},
    {WSAEISCONN, "Socket is already connected"},
    {WSAENOTCONN, "Socket is not connected"},
    {WSAESHUTDOWN, "Can't send after socket shutdown"},
    {WSAETOOMANYREFS, "Too many references, can't splice"},
    {WSAETIMEDOUT, "Connection timed out"},
    {WSAECONNREFUSED, "Connection refused"},
    {WSAELOOP, "Too many levels of symbolic links"},
    {WSAENAMETOOLONG, "File name too long"},
    {WSAEHOSTDOWN, "Host is down"},
    {WSAEHOSTUNREACH, "No route to host"},
    {WSAENOTEMPTY, "Directory not empty"},
    {WSAEPROCLIM, "Too many processes"},
    {WSAEUSERS, "Too many users"},
    {WSAEDQUOT, "Disc quota exceeded"},
    {WSAESTALE, "Stale NFS file handle"},
    {WSAEREMOTE, "Too many levels of remote in path"},
    {WSASYSNOTREADY, "Network subsystem is unavailable"},
    {WSAVERNOTSUPPORTED, "Winsock version not supported"},
    {WSANOTINITIALISED, "Winsock not yet initialized"},
    {WSAHOST_NOT_FOUND, "Host not found"},
    {WSATRY_AGAIN, "Non-authoritative host not found"},
    {WSANO_RECOVERY, "Non-recoverable errors"},
    {WSANO_DATA, "Valid name, no data record of requested type"},
    {WSAEDISCON, "Graceful disconnect in progress"},
#ifdef WSASYSCALLFAILURE
    {WSASYSCALLFAILURE, "System call failure"},
#endif
    {0, NULL}
};

char *ano_sockstrerror(int error)
{
    static char unkerr[64];
    int start = 0;
    int stop = sizeof(WSAErrors) / sizeof(WSAErrors[0]) - 1;
    int mid;

    /* Microsoft decided not to use sequential numbers for the error codes,
     * so we can't just use the array index for the code. But, at least
     * use a binary search to make it as fast as possible. 
     */
    while (start <= stop) {
        mid = (start + stop) / 2;
        if (WSAErrors[mid].error_code > error)
            stop = mid - 1;

        else if (WSAErrors[mid].error_code < error)
            start = mid + 1;
        else
            return WSAErrors[mid].error_string;
    }
    sprintf(unkerr, "Unknown Error: %d", error);
    return unkerr;
}

int ano_socksetnonb(ano_socket_t fd)
{
    u_long i = 1;
    return (!ioctlsocket(fd, FIONBIO, &i) ? -1 : 1);
}
#endif
