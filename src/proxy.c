/* Proxy detector.
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
#include "pseudo.h"
#include <fcntl.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF
#endif

/* Hashed list of HostCache; threads must not use it! */
HostCache *hcache[1024];

/*************************************************************************/

/* Equivalent to inet_ntoa */

void ntoa(struct in_addr addr, char *ipaddr, int len)
{
    unsigned char *bytes = (unsigned char *) &addr.s_addr;
    snprintf(ipaddr, len, "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2],
             bytes[3]);
}

/*************************************************************************/

#ifdef USE_THREADS

/*************************************************************************/

#define HASH(host)	((tolower((host)[0])&31)<<5 | (tolower((host)[1])&31))

/* Proxy queue; access controlled by queuemut */
SList pxqueue;

pthread_mutex_t queuemut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queuecond = PTHREAD_COND_INITIALIZER;

#if !defined(HAS_NICKIP) && !defined(HAVE_GETHOSTBYNAME_R6) && !defined(HAVE_GETHOSTBYNAME_R5) && !defined(HAVE_GETHOSTBYNAME_R3)
pthread_mutex_t resmut = PTHREAD_MUTEX_INITIALIZER;
#endif

static uint32 aton(char *ipaddr);
static void proxy_akill(char *host);
HostCache *proxy_cache_add(char *host);
static void proxy_cache_del(HostCache * hc);
static HostCache *proxy_cache_find(char *host);
static int proxy_connect(unsigned long ip, unsigned short port);
static void proxy_queue_cleanup_unlock(void *arg);
static void proxy_queue_lock(void);
static void proxy_queue_signal(void);
static void proxy_queue_unlock(void);
static void proxy_queue_wait(void);
static int proxy_read(int s, char *buf, size_t buflen);
#ifndef HAS_NICKIP
static uint32 proxy_resolve(char *host);
#endif
static int proxy_scan(uint32 ip);
static void *proxy_thread_main(void *arg);

/*************************************************************************/

/* Equivalent to inet_addr */

static uint32 aton(char *ipaddr)
{
    int i;
    long lv;
    char *endptr;
    uint32 res;
    unsigned char *bytes = (unsigned char *) &res;

    for (i = 0; i < 4; i++) {
        if (!*ipaddr)
            return INADDR_NONE;

        lv = strtol(ipaddr, &endptr, 10);
        if (lv < 0 || lv > 255 || (*endptr != 0 && *endptr != '.'))
            return INADDR_NONE;

        bytes[i] = (unsigned char) lv;
        ipaddr = (!*endptr ? endptr : ++endptr);
    }

    if (*endptr)
        return INADDR_NONE;

    return res;
}

/*************************************************************************/

void get_proxy_stats(long *nrec, long *memuse)
{
    int i;
    long mem = 0, count = 0;
    HostCache *hc;

    for (i = 0; i < 1024; i++) {
        for (hc = hcache[i]; hc; hc = hc->next) {
            count += 1;
            mem += sizeof(HostCache);
            mem += strlen(hc->host) + 1;
        }
    }

    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/

/* Akills the given host, and issues a GLOBOPS if configured so */

static void proxy_akill(char *host)
{
    s_akill("*", host, s_OperServ, time(NULL),
            time(NULL) + (ProxyExpire ? ProxyExpire : 86400 * 2),
            ProxyAkillReason);
    if (WallProxy)
        wallops(s_OperServ, "Insecure proxy \2%s\2 has been AKILLed.",
                host);
}

/*************************************************************************/

/* Adds a cache entry after having it allocated */

HostCache *proxy_cache_add(char *host)
{
    HostCache *hc;
    int index = HASH(host);

    hc = scalloc(1, sizeof(HostCache));
    hc->host = sstrdup(host);
    hc->used = time(NULL);

    hc->prev = NULL;
    hc->next = hcache[index];
    if (hc->next)
        hc->next->prev = hc;
    hcache[index] = hc;

    if (debug)
        alog("debug: Added %s to host cache", host);

    return hc;
}

/*************************************************************************/

/* Deletes and frees a proxy cache entry */

static void proxy_cache_del(HostCache * hc)
{
    /* Just to be sure */
    if (hc->status < 0)
        return;

    if (debug)
        alog("debug: Deleting %s from host cache", hc->host);

    if (hc->status > HC_NORMAL)
        s_rakill("*", hc->host);

    if (hc->next)
        hc->next->prev = hc->prev;
    if (hc->prev)
        hc->prev->next = hc->next;
    else
        hcache[HASH(hc->host)] = hc->next;

    if (hc->host)
        free(hc->host);

    free(hc);
}

/*************************************************************************/

/* Finds a proxy cache entry */

static HostCache *proxy_cache_find(char *host)
{
    HostCache *hc;

    for (hc = hcache[HASH(host)]; hc; hc = hc->next) {
        if (stricmp(hc->host, host) == 0)
            return hc;
    }

    return NULL;
}

/*************************************************************************/

/* Checks whether the specified host is in the cache.
 * If so:
 *   * if it's a proxy, take the appropriate actions, including killing nick
 *   * if it's not a proxy, do nothing
 * If not:
 *   * add the host to the cache
 *   * add the host to the queue
 *   * send a signal to a waiting thread (if any)
 * 
 * Returns 0 if nick is to be added to internal list, 1 else
 */

int proxy_check(char *nick, char *host, uint32 ip)
{
    int i;
    char **message;
    HostCache *hc;

    if ((hc = proxy_cache_find(host))) {
        hc->used = time(NULL);

        if (hc->status <= HC_NORMAL)
            return 0;

        proxy_akill(host);
        return 0;
    }

    for (message = ProxyMessage, i = 0; i < 8 && *message && **message;
         message++, i++)
        notice(s_GlobalNoticer, nick, *message);

    hc = proxy_cache_add(host);
#ifdef HAS_NICKIP
    hc->ip = htonl(ip);
#endif
    hc->status = HC_QUEUED;

    proxy_queue_lock();
    slist_add(&pxqueue, hc);
    if (debug)
        alog("debug: Added %s to proxy queue", hc->host);
    proxy_queue_signal();
    proxy_queue_unlock();

    return 0;
}

/*************************************************************************/

/* Initiates a non-blocking connection */

static int proxy_connect(unsigned long ip, unsigned short port)
{
    struct sockaddr_in sin;
    int s;

    fd_set fds;
    struct timeval tv;
    int error, errlen;

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    if (fcntl(s, F_SETFL, O_NONBLOCK) == -1) {
        close(s);
        return -1;
    }

    memset(&sin, 0, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip;
    sin.sin_port = htons(port);

    if (connect(s, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) ==
        -1 && errno != EINPROGRESS) {
        close(s);
        return -1;
    }

    FD_ZERO(&fds);
    FD_SET(s, &fds);

    tv.tv_sec = ProxyTimeout;
    tv.tv_usec = 0;

    if (select(s + 1, NULL, &fds, NULL, &tv) <= 0) {
        close(s);
        return -1;
    }

    errlen = sizeof(int);
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &errlen) == -1
        || error != 0) {
        close(s);
        return -1;
    }

    return s;
}

/*************************************************************************/

/* Deletes expired cache entries */

void proxy_expire()
{
    int i;
    HostCache *hc, *next;
    time_t t = time(NULL);

    for (i = 0; i < 1024; i++) {
        for (hc = hcache[i]; hc; hc = next) {
            next = hc->next;

            /* Don't expire not scanned yet entries */
            if (hc->status < HC_NORMAL)
                continue;

            if (hc->status == HC_NORMAL
                && t - hc->used >= ProxyCacheExpire) {
                proxy_cache_del(hc);
                continue;
            }

            if (ProxyExpire && hc->status > HC_NORMAL
                && t - hc->used >= ProxyExpire) {
                alog("proxy: Expiring proxy %s", hc->host);
                proxy_cache_del(hc);
            }
        }
    }
}

/*************************************************************************/

/* Initializes the proxy detector. Returns 1 on success, 0 on error. */

int proxy_init(void)
{
    int i;
    pthread_t th;

    slist_init(&pxqueue);

    for (i = 1; i <= ProxyThreads; i++) {
        if (pthread_create(&th, NULL, proxy_thread_main, NULL))
            return 0;
        if (pthread_detach(th))
            return 0;
        if (debug)
            alog("debug: Creating proxy thread %ld (%d of %d)", (long) th,
                 i, ProxyThreads);
    }

    alog("Proxy detector initialized");

    return 1;
}

/*************************************************************************/

static void proxy_queue_cleanup_unlock(void *arg)
{
    proxy_queue_unlock();
}

/*************************************************************************/

static void proxy_queue_lock(void)
{
    if (debug)
        alog("debug: Thread %ld: Locking proxy queue mutex",
             (long) pthread_self());
    pthread_mutex_lock(&queuemut);
}

/*************************************************************************/

static void proxy_queue_signal(void)
{
    if (debug)
        alog("debug: Thread %ld: Signaling proxy queue condition",
             (long) pthread_self());
    pthread_cond_signal(&queuecond);
}

/*************************************************************************/

static void proxy_queue_unlock(void)
{
    if (debug)
        alog("debug: Thread %ld: Unlocking proxy queue mutex",
             (long) pthread_self());
    pthread_mutex_unlock(&queuemut);
}

/*************************************************************************/

static void proxy_queue_wait(void)
{
    if (debug)
        alog("debug: Thread %ld: waiting proxy queue condition",
             (long) pthread_self());
    pthread_cond_wait(&queuecond, &queuemut);
}

/*************************************************************************/

/* Reads from the socket, in a non-blocking manner */

static int proxy_read(int s, char *buf, size_t buflen)
{
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(s, &fds);

    tv.tv_sec = ProxyTimeout;
    tv.tv_usec = 0;

    if (select(s + 1, &fds, NULL, NULL, &tv) <= 0)
        return -1;

    return recv(s, buf, buflen, 0);
}

/*************************************************************************/

/* Resolves hostnames in a thread safe manner */

#ifndef HAS_NICKIP

static uint32 proxy_resolve(char *host)
{
    struct hostent *hentp = NULL;
    uint32 ip = INADDR_NONE;
#if defined(HAVE_GETHOSTBYNAME_R6)
    struct hostent hent;
    char hbuf[8192];
    int herrno;

    if (gethostbyname_r(host, &hent, hbuf, sizeof(hbuf), &hentp, &herrno) <
        0)
        hentp = NULL;
#elif defined(HAVE_GETHOSTBYNAME_R5)
    struct hostent hent char hbuf[8192];
    int herrno;
    hentp = gethostbyname_r(host, &hent, hbuf, sizeof(hbuf), &herrno);
#elif defined(HAVE_GETHOSTBYNAME_R3)
    struct hostent hent;
    struct hostent_data data;
    hentp = gethostbyname_r(host, &hent, &data);
#else
    /* Make it safe that way */
    pthread_mutex_lock(&resmut);
    hentp = gethostbyname(host);
#endif

    if (hentp) {
        memcpy(&ip, hentp->h_addr, sizeof(hentp->h_length));
        if (debug) {
            char ipbuf[16];
            struct in_addr addr;
            addr.s_addr = ip;
            ntoa(addr, ipbuf, sizeof(ipbuf));
            alog("debug: Thread %ld: resolved %s to %s",
                 (long) pthread_self(), host, ipbuf);
        }
    }
#if !defined(HAVE_GETHOSTBYNAME_R6) && !defined(HAVE_GETHOSTBYNAME_R5) && !defined(HAVE_GETHOSTBYNAME_R3)
    pthread_mutex_unlock(&resmut);
#endif

    return ip;
}

#endif

/*************************************************************************/

/* Scans the given host for proxy */

static int proxy_scan(uint32 ip)
{
    int s;                      /* Socket */
    int i;

    if (ip == INADDR_NONE)
        return HC_NORMAL;

    /* Scan for SOCKS (4/5) */

    for (i = 0; i < 2; i++) {
        if ((s = proxy_connect(ip, 1080)) == -1)
            break;

        if (ProxyCheckSocks4 && i == 0) {
            /* SOCKS4 */

            char buf[9];
            uint32 sip;

            sip = aton(ProxyTestServer);
            sip = htonl(sip);

            buf[0] = 4;
            buf[1] = 1;
            buf[2] = (((unsigned short) ProxyTestPort) >> 8) & 0xFF;
            buf[3] = ((unsigned short) ProxyTestPort) & 0xFF;
            buf[4] = (sip >> 24) & 0xFF;
            buf[5] = (sip >> 16) & 0xFF;
            buf[6] = (sip >> 8) & 0xFF;
            buf[7] = sip & 0xFF;
            buf[8] = 0;

            if (send(s, buf, 9, 0) != 9) {
                close(s);
                return HC_NORMAL;
            }

            if (proxy_read(s, buf, 2) != 2) {
                close(s);
                continue;
            }

            if (buf[1] == 90) {
                close(s);
                return HC_SOCKS4;
            }

        } else if (ProxyCheckSocks5 && i == 1) {
            /* SOCKS5 */

            char buf[10];
            uint32 sip;

            if (send(s, "\5\1\0", 3, 0) != 3) {
                close(s);
                continue;
            }

            memset(buf, 0, sizeof(buf));

            if (proxy_read(s, buf, 2) != 2) {
                close(s);
                continue;
            }

            if (buf[0] != 5 || buf[1] != 0) {
                close(s);
                continue;
            }

            sip = aton(ProxyTestServer);
            sip = htonl(sip);

            buf[0] = 5;
            buf[1] = 1;
            buf[2] = 0;
            buf[3] = 1;
            buf[4] = (sip >> 24) & 0xFF;
            buf[5] = (sip >> 16) & 0xFF;
            buf[6] = (sip >> 8) & 0xFF;
            buf[7] = sip & 0xFF;
            buf[8] = (((unsigned short) ProxyTestPort) >> 8) & 0xFF;
            buf[9] = ((unsigned short) ProxyTestPort) & 0xFF;

            if (send(s, buf, 10, 0) != 10) {
                close(s);
                continue;
            }

            memset(buf, 0, sizeof(buf));

            if (proxy_read(s, buf, 2) != 2) {
                close(s);
                continue;
            }

            if (buf[0] == 5 && buf[1] == 0) {
                close(s);
                return HC_SOCKS5;
            }
        }

        close(s);
    }

    /* Scan for HTTP proxy */
    for (i = 0; i < 3; i++) {
        if ((i ==
             0 ? ProxyCheckHTTP2 : (i ==
                                    1 ? ProxyCheckHTTP1 : ProxyCheckHTTP3))
            && (s =
                proxy_connect(ip,
                              (i == 0 ? 8080 : (i == 1 ? 3128 : 80)))) !=
            -1) {
            int bread;
            char buf[64];

            snprintf(buf, sizeof(buf), "CONNECT %s:%d HTTP/1.0\n\n",
                     ProxyTestServer, ProxyTestPort);
            if (send(s, buf, strlen(buf), 0) == strlen(buf)) {
                if ((bread = proxy_read(s, buf, 15)) >= 12) {
                    buf[bread] = 0;

                    if (!strnicmp(buf, "HTTP/1.0 200", 12) || !stricmp(buf, "HTTP/1.1 200 Co")) {       /* Apache may return 200 OK
                                                                                                           even if it's not processing
                                                                                                           the CONNECT request. :/ */
                        close(s);
                        return HC_HTTP;
                    }
                }
            }
            close(s);
        }
    }

    /* Scan for Wingate */
    if (ProxyCheckWingate && (s = proxy_connect(ip, 23)) != -1) {
        char buf[9];

        if (proxy_read(s, buf, 8) == 8) {
            buf[8] = '\0';
            if (!stricmp(buf, "Wingate>") || !stricmp(buf, "Too many")) {
                close(s);
                return HC_WINGATE;
            }
        }
        close(s);
    }

    return HC_NORMAL;
}

/*************************************************************************/

/* Proxy detector threads entry point */

static void *proxy_thread_main(void *arg)
{
    while (1) {
        pthread_cleanup_push(&proxy_queue_cleanup_unlock, NULL);
        proxy_queue_lock();
        proxy_queue_wait();
        pthread_cleanup_pop(1);

        /* We loop until there is no more host to check in the list */
        while (1) {
            HostCache *hc = NULL;
            int status;

            pthread_cleanup_push(&proxy_queue_cleanup_unlock, NULL);
            proxy_queue_lock();
            if (pxqueue.count > 0) {
                hc = pxqueue.list[0];
                hc->status = HC_PROGRESS;
                slist_delete(&pxqueue, 0);
            }
            pthread_cleanup_pop(1);

            if (!hc)
                break;

            if (debug) {
                if (hc->ip) {
                    char ipbuf[16];
                    struct in_addr in;
                    in.s_addr = hc->ip;
                    ntoa(in, ipbuf, sizeof(ipbuf));
                    alog("debug: Scanning host %s [%s] for proxy",
                         hc->host, ipbuf);
                } else {
                    alog("debug: Scanning host %s for proxy", hc->host);
                }
            }
#ifndef HAS_NICKIP
            /* Test if it's an IP, and if not try to resolve the hostname */
            if ((hc->ip = aton(hc->host)) == INADDR_NONE)
                hc->ip = proxy_resolve(hc->host);
#endif
            status = proxy_scan(hc->ip);

            if (debug) {
                char ipbuf[16];
                struct in_addr in;
                in.s_addr = hc->ip;
                ntoa(in, ipbuf, sizeof(ipbuf));
                alog("debug: Scan for %s [%s] complete, result: %d",
                     hc->host, ipbuf, status);
            }

            if (status > HC_NORMAL)
                proxy_akill(hc->host);

            hc->status = status;
        }
    }

    return NULL;
}

/*************************************************************************/

#endif

/*************************************************************************/

/* OperServ CACHE */

int do_cache(User * u)
{
#ifdef USE_THREADS
    char *cmd = strtok(NULL, " ");
    char *pattern = strtok(NULL, " ");

    if (!ProxyDetect) {
        notice_lang(s_OperServ, u, OPER_CACHE_DISABLED);
        return MOD_CONT;
    }

    if (!cmd || !pattern) {
        syntax_error(s_OperServ, u, "CACHE", OPER_CACHE_SYNTAX);
    } else if (!stricmp(cmd, "DEL")) {
        HostCache *hc;

        if (!(hc = proxy_cache_find(pattern))) {
            notice_lang(s_OperServ, u, OPER_CACHE_NOT_FOUND, pattern);
            return MOD_CONT;
        }

        proxy_cache_del(hc);
        notice_lang(s_OperServ, u, OPER_CACHE_REMOVED, pattern);

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "LIST")) {
        char *option = strtok(NULL, " ");
        int i, restrict = 0, count = 0, total = 0;
        HostCache *hc;

        static int statusdesc[7] = {
            OPER_CACHE_QUEUED,
            OPER_CACHE_PROGRESS,
            OPER_CACHE_NORMAL,
            OPER_CACHE_WINGATE,
            OPER_CACHE_SOCKS4,
            OPER_CACHE_SOCKS5,
            OPER_CACHE_HTTP
        };

        if (option && !stricmp(option, "QUEUED"))
            restrict = 1;
        else if (option && !stricmp(option, "ALL"))
            restrict = 2;

        notice_lang(s_OperServ, u, OPER_CACHE_HEADER);

        for (i = 0; i < 1024; i++) {
            for (hc = hcache[i]; hc; hc = hc->next) {
                if (!match_wild_nocase(pattern, hc->host))
                    continue;
                if ((restrict == 0 && hc->status <= HC_NORMAL)
                    || (restrict == 1 && hc->status >= HC_NORMAL))
                    continue;
                total++;
                if (count >= ProxyMax)
                    continue;
                notice_lang(s_OperServ, u, OPER_CACHE_LIST, hc->host,
                            getstring(u->na, statusdesc[hc->status + 2]));
                count++;
            }
        }

        notice_lang(s_OperServ, u, OPER_CACHE_FOOTER, count, total);

    } else {
        syntax_error(s_OperServ, u, "CACHE", OPER_CACHE_SYNTAX);
    }
#else
    notice_lang(s_OperServ, u, OPER_CACHE_DISABLED);
#endif
    return MOD_CONT;
}

/*************************************************************************/
