/* Initalization and related routines.
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
int servernum = 0;

extern void moduleAddMsgs(void);
/*************************************************************************/

/* Send a NICK command for the given pseudo-client. If `user' is NULL,
 * send NICK commands for all the pseudo-clients.
 * 
 * Now also sends MODE and SQLINE */
#if defined(IRC_HYBRID)
# define NICK(nick,name,modes) \
    do { \
	kill_user(NULL, (nick), "Nick used by Services"); \
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", (nick), time(NULL), (modes), \
		ServiceUser, ServiceHost, ServerName, (name)); \
	} while (0)
#elif defined(IRC_ULTIMATE3)
# define NICK(nick,name,modes) \
    do { \
	send_cmd(NULL, "CLIENT %s 1 %ld %s + %s %s * %s 0 0 :%s", (nick), time(NULL), (modes), \
		ServiceUser, ServiceHost, ServerName, (name)); \
	send_cmd(NULL, "SQLINE %s :Reserved for services", (nick)); \
	} while (0)
#elif defined(IRC_RAGE2)
# define NICK(nick,name,modes) \
    do { \
	send_cmd(NULL, "SNICK %s %ld 1 %s %s 0 * %s 0 %s :%s", (nick), time(NULL), ServiceUser, \
		ServiceHost, ServerName, (modes), (name)); \
	send_cmd(NULL, "SQLINE %s :Reserved for services", (nick)); \
    } while (0)
#elif defined(IRC_BAHAMUT)
# define NICK(nick,name,modes) \
    do { \
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s 0 0 :%s", (nick), time(NULL), (modes), \
		ServiceUser, ServiceHost, ServerName, (name)); \
	send_cmd(NULL, "SQLINE %s :Reserved for services", (nick)); \
	} while (0)
#elif defined(IRC_UNREAL)
# define NICK(nick,name,modes) \
    do { \
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 %s * :%s", (nick), time(NULL), \
		ServiceUser, ServiceHost, ServerName, (modes), (name)); \
	send_cmd(NULL, "SQLINE %s :Reserved for services", (nick)); \
    } while (0)
#elif defined(IRC_DREAMFORGE)
# define NICK(nick,name,modes) \
    do { \
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 :%s", (nick), time(NULL), \
		ServiceUser, ServiceHost, ServerName, (name)); \
	if (strcmp(modes, "+")) send_cmd((nick), "MODE %s %s", (nick), (modes)); \
	send_cmd(NULL, "SQLINE %s :Reserved for services", (nick)); \
    } while (0)
#elif defined(IRC_PTLINK)
# define NICK(nick,name,modes) \
    do { \
        send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", (nick), time(NULL), \
                (modes), ServiceUser, ServiceHost, ServiceHost, ServerName, (name)); \
    } while (0)
#endif

void introduce_user(const char *user)
{
    /* Watch out for infinite loops... */
#define LTSIZE 20
    static int lasttimes[LTSIZE];
    if (lasttimes[0] >= time(NULL) - 3)
        fatal("introduce_user() loop detected");
    memmove(lasttimes, lasttimes + 1, sizeof(lasttimes) - sizeof(int));
    lasttimes[LTSIZE - 1] = time(NULL);
#undef LTSIZE

    if (!user || stricmp(user, s_NickServ) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_NickServ, desc_NickServ, "+S");
#elif defined(IRC_RAGE2)
        NICK(s_NickServ, desc_NickServ, "+dS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_NickServ, desc_NickServ, "+oS");
#else
        NICK(s_NickServ, desc_NickServ, "+o");
#endif
    if (!user || stricmp(user, s_ChanServ) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_ChanServ, desc_ChanServ, "+S");
#elif defined(IRC_RAGE2)
        NICK(s_ChanServ, desc_ChanServ, "+dS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_ChanServ, desc_ChanServ, "+oS");
#else
        NICK(s_ChanServ, desc_ChanServ, "+o");
#endif

#ifdef HAS_VHOST
    if (s_HostServ && (!user || stricmp(user, s_HostServ) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_HostServ, desc_HostServ, "+oS");
#elif defined(IRC_RAGE2)
        NICK(s_HostServ, desc_HostServ, "+dS");
#else
        NICK(s_HostServ, desc_HostServ, "+o");
#endif
#endif

    if (!user || stricmp(user, s_MemoServ) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_MemoServ, desc_MemoServ, "+S");
#elif defined(IRC_RAGE2)
        NICK(s_MemoServ, desc_MemoServ, "+dS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_MemoServ, desc_MemoServ, "+oS");
#else
        NICK(s_MemoServ, desc_MemoServ, "+o");
#endif
    if (s_BotServ && (!user || stricmp(user, s_BotServ) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_BotServ, desc_BotServ, "+S");
#elif defined(IRC_RAGE2)
        NICK(s_BotServ, desc_BotServ, "+dS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_BotServ, desc_BotServ, "+oS");
#else
        NICK(s_BotServ, desc_BotServ, "+o");
#endif
    if (!user || stricmp(user, s_HelpServ) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_HelpServ, desc_HelpServ, "+Sh");
#elif defined(IRC_RAGE2)
        NICK(s_HelpServ, desc_HelpServ, "+dSh");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_HelpServ, desc_HelpServ, "+oS");
#else
        NICK(s_HelpServ, desc_HelpServ, "+h");
#endif
    if (!user || stricmp(user, s_OperServ) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_OperServ, desc_OperServ, "+iS");
#elif defined(IRC_RAGE2)
        NICK(s_OperServ, desc_OperServ, "+diS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_OperServ, desc_OperServ, "+ioS");
#else
        NICK(s_OperServ, desc_OperServ, "+io");
#endif
    if (s_DevNull && (!user || stricmp(user, s_DevNull) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3)
        NICK(s_DevNull, desc_DevNull, "+iS");
#elif defined(IRC_RAGE2)
        NICK(s_DevNull, desc_DevNull, "+diS");
#else
        NICK(s_DevNull, desc_DevNull, "+i");
#endif
    if (!user || stricmp(user, s_GlobalNoticer) == 0)
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        NICK(s_GlobalNoticer, desc_GlobalNoticer, "+iS");
#elif defined(IRC_RAGE2)
        NICK(s_GlobalNoticer, desc_GlobalNoticer, "+diS");
#elif defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_GlobalNoticer, desc_GlobalNoticer, "+ioS");
#else
        NICK(s_GlobalNoticer, desc_GlobalNoticer, "+io");
#endif

/* We make aliases go online */
    if (s_NickServAlias && (!user || stricmp(user, s_NickServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_NickServAlias, desc_NickServAlias, "+oS");
#else
        NICK(s_NickServAlias, desc_NickServAlias, "+o");
#endif
    if (s_ChanServAlias && (!user || stricmp(user, s_ChanServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_ChanServAlias, desc_ChanServAlias, "+oS");
#else
        NICK(s_ChanServAlias, desc_ChanServAlias, "+o");
#endif
    if (s_MemoServAlias && (!user || stricmp(user, s_MemoServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_MemoServAlias, desc_MemoServAlias, "+oS");
#else
        NICK(s_MemoServAlias, desc_MemoServAlias, "+o");
#endif
    if (s_BotServAlias && (!user || stricmp(user, s_BotServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_BotServAlias, desc_BotServAlias, "+oS");
#else
        NICK(s_BotServAlias, desc_BotServAlias, "+o");
#endif
    if (s_HelpServAlias && (!user || stricmp(user, s_HelpServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_HelpServAlias, desc_HelpServAlias, "+oS");
#else
        NICK(s_HelpServAlias, desc_HelpServAlias, "+h");
#endif
    if (s_OperServAlias && (!user || stricmp(user, s_OperServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_OperServAlias, desc_OperServAlias, "+ioS");
#else
        NICK(s_OperServAlias, desc_OperServAlias, "+io");
#endif
    if (s_DevNullAlias && (!user || stricmp(user, s_DevNullAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_DevNullAlias, desc_DevNullAlias, "+iS");
#else
        NICK(s_DevNullAlias, desc_DevNullAlias, "+i");
#endif
    if (s_HostServAlias && (!user || stricmp(user, s_HostServAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)
        NICK(s_HostServAlias, desc_HostServAlias, "+ioS");
#else
        NICK(s_HostServAlias, desc_HostServAlias, "+io");
#endif
    if (s_GlobalNoticerAlias
        && (!user || stricmp(user, s_GlobalNoticerAlias) == 0))
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL)
        NICK(s_GlobalNoticerAlias, desc_GlobalNoticerAlias, "+ioS");
#else
        NICK(s_GlobalNoticerAlias, desc_GlobalNoticerAlias, "+io");
#endif

    /* We make the bots go online */
    if (s_BotServ) {
        BotInfo *bi;
        int i;

        for (i = 0; i < 256; i++)
            for (bi = botlists[i]; bi; bi = bi->next)
                if (!user || !stricmp(user, bi->nick))
#if defined(IRC_UNREAL) || defined(IRC_VIAGRA)
                    NEWNICK(bi->nick, bi->user, bi->host, bi->real, "+qS",
                            1);
#elif defined(IRC_ULTIMATE)
                    NEWNICK(bi->nick, bi->user, bi->host, bi->real, "+pS",
                            1);
#elif defined(IRC_ULTIMATE3) || defined(IRC_RAGE2)
                    NEWNICK(bi->nick, bi->user, bi->host, bi->real, "+S",
                            1);
#else
                    NEWNICK(bi->nick, bi->user, bi->host, bi->real, "+",
                            1);
#endif
    }
}

#undef NICK

/*************************************************************************/

/* Set GID if necessary.  Return 0 if successful (or if RUNGROUP not
 * defined), else print an error message to logfile and return -1.
 */

static int set_group(void)
{
#if defined(RUNGROUP) && defined(HAVE_SETGRENT)
    struct group *gr;

    setgrent();
    while ((gr = getgrent()) != NULL) {
        if (strcmp(gr->gr_name, RUNGROUP) == 0)
            break;
    }
    endgrent();
    if (gr) {
        setgid(gr->gr_gid);
        return 0;
    } else {
        alog("Unknown group `%s'\n", RUNGROUP);
        return -1;
    }
#else
    return 0;
#endif
}

/*************************************************************************/

/* Parse command-line options for the "-dir" option only.  Return 0 if all
 * went well or -1 for a syntax error.
 */

/* XXX this could fail if we have "-some-option-taking-an-argument -dir" */

static int parse_dir_options(int ac, char **av)
{
    int i;
    char *s;

    for (i = 1; i < ac; i++) {
        s = av[i];
        if (*s == '-') {
            s++;
            if (strcmp(s, "dir") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-dir requires a parameter\n");
                    return -1;
                }
                services_dir = av[i];
            } else if (strcmp(s, "log") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-log requires a parameter\n");
                    return -1;
                }
                log_filename = av[i];
            }
        }
    }
    return 0;
}

/*************************************************************************/

/* Parse command-line options.  Return 0 if all went well, -1 for an error
 * with an option, or 1 for -help.
 */

static int parse_options(int ac, char **av)
{
    int i;
    char *s, *t;

    for (i = 1; i < ac; i++) {
        s = av[i];
        if (*s == '-') {
            s++;
            if (strcmp(s, "remote") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-remote requires hostname[:port]\n");
                    return -1;
                }
                s = av[i];
                t = strchr(s, ':');
                if (t) {
                    *t++ = 0;
                    if (atoi(t) > 0)
                        RemotePort = atoi(t);
                    else {
                        fprintf(stderr,
                                "-remote: port number must be a positive integer.  Using default.");
                        return -1;
                    }
                }
                RemoteServer = s;
            } else if (strcmp(s, "local") == 0) {
                if (++i >= ac) {
                    fprintf(stderr,
                            "-local requires hostname or [hostname]:[port]\n");
                    return -1;
                }
                s = av[i];
                t = strchr(s, ':');
                if (t) {
                    *t++ = 0;
                    if (atoi(t) >= 0)
                        LocalPort = atoi(t);
                    else {
                        fprintf(stderr,
                                "-local: port number must be a positive integer or 0.  Using default.");
                        return -1;
                    }
                }
                LocalHost = s;
            } else if (strcmp(s, "name") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-name requires a parameter\n");
                    return -1;
                }
                ServerName = av[i];
            } else if (strcmp(s, "desc") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-desc requires a parameter\n");
                    return -1;
                }
                ServerDesc = av[i];
            } else if (strcmp(s, "user") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-user requires a parameter\n");
                    return -1;
                }
                ServiceUser = av[i];
            } else if (strcmp(s, "host") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-host requires a parameter\n");
                    return -1;
                }
                ServiceHost = av[i];
            } else if (strcmp(s, "dir") == 0) {
                /* Handled by parse_dir_options() */
                i++;            /* Skip parameter */
            } else if (strcmp(s, "log") == 0) {
                /* Handled by parse_dir_options(), too */
                i++;            /* Skip parameter */
            } else if (strcmp(s, "update") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-update requires a parameter\n");
                    return -1;
                }
                s = av[i];
                if (atoi(s) <= 0) {
                    fprintf(stderr,
                            "-update: number of seconds must be positive");
                    return -1;
                } else
                    UpdateTimeout = atol(s);
            } else if (strcmp(s, "expire") == 0) {
                if (++i >= ac) {
                    fprintf(stderr, "-expire requires a parameter\n");
                    return -1;
                }
                s = av[i];
                if (atoi(s) <= 0) {
                    fprintf(stderr,
                            "-expire: number of seconds must be positive");
                    return -1;
                } else
                    ExpireTimeout = atol(s);
            } else if (strcmp(s, "debug") == 0) {
                debug++;
            } else if (strcmp(s, "readonly") == 0) {
                readonly = 1;
                skeleton = 0;
            } else if (strcmp(s, "skeleton") == 0) {
                readonly = 0;
                skeleton = 1;
            } else if (strcmp(s, "nofork") == 0) {
                nofork = 1;
            } else if (strcmp(s, "logchan") == 0) {
                logchan = 1;
#ifdef IRC_HYBRID
                fprintf(stderr,
                        "LogChan will only work if your logchannel is not set to +n\n");
#endif
            } else if (strcmp(s, "forceload") == 0) {
                forceload = 1;
            } else if (!strcmp(s, "noexpire")) {
                noexpire = 1;
#ifdef IS44_CONVERTER
            } else if (!strcmp(s, "is44")) {
                is44 = 1;
#endif
            } else if (!strcmp(s, "version")) {
                fprintf(stdout, "Anope-%s %s -- %s\n", version_number,
                        version_flags, version_build);
                exit(EXIT_SUCCESS);
            } else if (!strcmp(s, "help")) {
                fprintf(stdout, "Anope-%s %s -- %s\n", version_number,
                        version_flags, version_build);
                fprintf(stdout,
                        "Anope IRC Services (http://www.anope.org)\n");
                fprintf(stdout, "Usage ./services [options] ...\n");
                fprintf(stdout,
                        "-remote        -remote hostname[:port]\n");
                fprintf(stdout, "-local         -local hostname[:port]\n");
                fprintf(stdout, "-name          -name servername\n");
                fprintf(stdout, "-desc          -desc serverdesc\n");
                fprintf(stdout, "-user          -user serviceuser\n");
                fprintf(stdout, "-host          -host servicehost\n");
                fprintf(stdout,
                        "-update        -update updatetime(secs)\n");
                fprintf(stdout,
                        "-expire        -expire expiretime(secs)\n");
                fprintf(stdout, "-debug         -debug\n");
                fprintf(stdout, "-nofork        -nofork\n");
                fprintf(stdout, "-logchan       -logchan channelname\n");
                fprintf(stdout, "-skeleton      -skeleton\n");
                fprintf(stdout, "-forceload     -forceload\n");
                fprintf(stdout, "-readonly      -readonly\n");
                fprintf(stdout, "-noexpire      -noexpire\n");
                fprintf(stdout, "-is44          -is44\n");
                fprintf(stdout, "-version       -version\n");
                fprintf(stdout, "-help          -help\n");
                fprintf(stdout, "-log           -log logfilename\n");
                fprintf(stdout,
                        "-dir           -dir servicesdirectory\n\n");
                fprintf(stdout,
                        "Further support is available from http://www.anope.org\n");
                fprintf(stdout,
                        "Or visit US on IRC at irc.anope.org #anope\n");
                exit(EXIT_SUCCESS);
            } else {
                fprintf(stderr, "Unknown option -%s\n", s);
                return -1;
            }
        } else {
            fprintf(stderr, "Non-option arguments not allowed\n");
            return -1;
        }
    }
    return 0;
}

/*************************************************************************/

/* Remove our PID file.  Done at exit. */

static void remove_pidfile(void)
{
    remove(PIDFilename);
}

/*************************************************************************/

/* Create our PID file and write the PID to it. */

static void write_pidfile(void)
{
    FILE *pidfile;

    pidfile = fopen(PIDFilename, "w");
    if (pidfile) {
        fprintf(pidfile, "%d\n", (int) getpid());
        fclose(pidfile);
        atexit(remove_pidfile);
    } else {
        log_perror("Warning: cannot write to PID file %s", PIDFilename);
    }
}

/*************************************************************************/

/* Overall initialization routine.  Returns 0 on success, -1 on failure. */

int init(int ac, char **av)
{
    int i;
    int openlog_failed = 0, openlog_errno = 0;
    int started_from_term = isatty(0) && isatty(1) && isatty(2);

    /* Imported from main.c */
    extern void sighandler(int signum);


    /* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
    umask(DEFUMASK);
#endif
    if (set_group() < 0)
        return -1;

    /* Parse command line for -dir option. */
    parse_dir_options(ac, av);

    /* Chdir to Services data directory. */
    if (chdir(services_dir) < 0) {
        fprintf(stderr, "chdir(%s): %s\n", services_dir, strerror(errno));
        return -1;
    }

    /* Open logfile, and complain if we didn't. */
    if (open_log() < 0) {
        openlog_errno = errno;
        if (started_from_term) {
            fprintf(stderr, "Warning: unable to open log file %s: %s\n",
                    log_filename, strerror(errno));
        } else {
            openlog_failed = 1;
        }
    }

    /* Read configuration file; exit if there are problems. */
    if (!read_config(0))
        return -1;

    /* Add Core MSG handles */
    moduleAddMsgs();

    /* Parse all remaining command-line options. */
    parse_options(ac, av);

    /* Detach ourselves if requested. */
    if (!nofork) {
        if ((i = fork()) < 0) {
            perror("fork()");
            return -1;
        } else if (i != 0) {
            exit(0);
        }
        if (started_from_term) {
            close(0);
            close(1);
            close(2);
        }
        if (setpgid(0, 0) < 0) {
            perror("setpgid()");
            return -1;
        }
    }

    /* Write our PID to the PID file. */
    write_pidfile();

    /* Announce ourselves to the logfile. */
    if (debug || readonly || skeleton) {
        alog("Anope %s (compiled for %s) starting up (options:%s%s%s)",
             version_number, version_protocol,
             debug ? " debug" : "", readonly ? " readonly" : "",
             skeleton ? " skeleton" : "");
    } else {
        alog("Anope %s (compiled for %s) starting up",
             version_number, version_protocol);
    }
    start_time = time(NULL);

    /* If in read-only mode, close the logfile again. */
    if (readonly)
        close_log();

    /* Set signal handlers.  Catch certain signals to let us do things or
     * panic as necessary, and ignore all others.
     */

#if defined(NSIG) && !defined(LINUX20) && !defined(LINUX22)
    for (i = 1; i <= NSIG - 1; i++) {
#else
    for (i = 1; i <= 31; i++) {
#endif
#if defined(USE_THREADS) && defined(LINUX20)
        if (i != SIGUSR1)
#endif
            signal(i, SIG_IGN);
    }

#ifndef USE_THREADS
    signal(SIGINT, sighandler);
#else
    signal(SIGINT, SIG_DFL);
#endif
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);
    if (!DumpCore) {
        signal(SIGSEGV, sighandler);
        signal(SIGBUS, sighandler);
        signal(SIGILL, sighandler);
        signal(SIGTRAP, sighandler);
    } else {
        signal(SIGSEGV, SIG_DFL);
        signal(SIGBUS, SIG_DFL);
        signal(SIGILL, SIG_DFL);
        signal(SIGTRAP, SIG_DFL);
    }
    signal(SIGQUIT, sighandler);
    signal(SIGHUP, sighandler);
    signal(SIGUSR2, sighandler);

#ifdef SIGIOT
    signal(SIGIOT, sighandler);
#endif
    signal(SIGFPE, sighandler);

#if !defined(USE_THREADS) || !defined(LINUX20)
    signal(SIGUSR1, sighandler);        /* This is our "out-of-memory" panic switch */
#endif

    /* Initialize multi-language support */
    lang_init();
    if (debug)
        alog("debug: Loaded languages");

    /* Initialize subservices */
    ns_init();
    cs_init();
    ms_init();
    bs_init();
    os_init();
    hostserv_init();
    helpserv_init();

#ifdef USE_RDB
    rdb_init();
#endif

    /* Initialize proxy detection */
#ifdef USE_THREADS
    if (ProxyDetect && !proxy_init()) {
        perror("proxy_init()");
        return -1;
    }
#endif

    /* load any custom modules */
    modules_init();

#ifdef USE_CONVERTER
    /* Convert the databases NOW! */
# ifdef IS44_CONVERTER
    if (is44) {
        convert_ircservices_44();
        alog("debug: Databases converted");
    }
# endif
#endif

    /* Load up databases */
#ifdef USE_RDB
    if (UseRDB)
        rdb_load_dbases();
    /* Need a better way to handle this -dane */
    if (!UseRDB) {
#endif
        if (!skeleton) {
            load_ns_dbase();
            if (debug)
                alog("debug: Loaded %s database (1/9)", s_NickServ);
            if (s_HostServ) {
                load_hs_dbase();
                if (debug)
                    alog("debug: Loaded %s database (2/9)", s_HostServ);
            }
            if (s_BotServ) {
                load_bs_dbase();
                if (debug)
                    alog("debug: Loaded %s database (3/9)", s_BotServ);
            } else if (debug)
                alog("debug: BotServ database (4/9) not loaded because BotServ is disabled");
            load_cs_dbase();
            if (debug)
                alog("debug: Loaded %s database (5/9)", s_ChanServ);
        }
        load_os_dbase();
        if (debug)
            alog("debug: Loaded %s database (6/9)", s_OperServ);
        load_news();
        if (debug)
            alog("debug: Loaded news database (7/9)");
        load_exceptions();
        if (debug)
            alog("debug: Loaded exception database (8/9)");
        if (PreNickDBName) {
            load_ns_req_db();
            if (debug)
                alog("debug: Loaded PreNick database (9/9)");
        }
#ifdef USE_RDB
    }
#endif
    alog("Databases loaded");

    /* Save the databases back to file/mysql to reflect any changes */
#ifdef USE_RDB
    if (!UseRDB) {              /* Only save if we are not using remote databases
                                 * to avoid floods. As a side effects our nice
                                 * FFF databases won't get overwritten if the
                                 * mysql db is broken (empty etc.) */
#endif
        alog("Info: Reflecting database records.");
        save_databases();
#ifdef USE_RDB
    } else {
        alog("Info: Not reflecting database records.");
    }
#endif
    /* Connect to the remote server */
    servsock = conn(RemoteServer, RemotePort, LocalHost, LocalPort);
    if (servsock < 0 && RemoteServer2) {
        servsock = conn(RemoteServer2, RemotePort2, LocalHost, LocalPort);
        if (servsock < 0 && RemoteServer3) {
            servsock =
                conn(RemoteServer3, RemotePort3, LocalHost, LocalPort);
            if (servsock < 0) {
                fatal_perror("Can't connect to server");
            } else {
                servernum = 3;
                alog("Connected to Server %d (%s:%d)", servernum,
                     RemoteServer3, RemotePort3);
            }
        } else {
            if (servsock < 0) {
                fatal_perror("Can't connect to server");
            }
            servernum = 2;
            alog("Connected to Server %d (%s:%d)", servernum,
                 RemoteServer2, RemotePort2);
        }
    } else {
        if (servsock < 0) {
            fatal_perror("Can't connect to server");
        }
        servernum = 1;
        alog("Connected to Server %d (%s:%d)", servernum, RemoteServer,
             RemotePort);
    }

#ifdef IRC_UNREAL
    send_cmd(NULL, "PROTOCTL NICKv2 VHP");
#endif
#if defined(IRC_ULTIMATE3)
    if (servernum == 1)
        send_cmd(NULL, "PASS %s :TS", RemotePassword);
    else if (servernum == 2)
        send_cmd(NULL, "PASS %s :TS", RemotePassword2);
    else if (servernum == 3)
        send_cmd(NULL, "PASS %s :TS", RemotePassword3);
    send_cmd(NULL, "CAPAB NICKIP SSJ5 TS5 CLIENT");
#elif defined(IRC_RAGE2)
    if (servernum == 1)
        send_cmd(NULL, "PASS %s :TS", RemotePassword);
    else if (servernum == 2)
        send_cmd(NULL, "PASS %s :TS", RemotePassword2);
    else if (servernum == 3)
        send_cmd(NULL, "PASS %s :TS", RemotePassword3);
    send_cmd(NULL, "CAPAB SSJ3 SN2 VHOST");
#elif defined(IRC_BAHAMUT)
    if (servernum == 1)
        send_cmd(NULL, "PASS %s :TS", RemotePassword);
    else if (servernum == 2)
        send_cmd(NULL, "PASS %s :TS", RemotePassword2);
    else if (servernum == 3)
        send_cmd(NULL, "PASS %s :TS", RemotePassword3);
    send_cmd(NULL, "CAPAB NICKIP SSJOIN TS3");
#elif defined(IRC_HYBRID)
    if (servernum == 1)
        send_cmd(NULL, "PASS %s :TS", RemotePassword);
    else if (servernum == 2)
        send_cmd(NULL, "PASS %s :TS", RemotePassword2);
    else if (servernum == 3)
        send_cmd(NULL, "PASS %s :TS", RemotePassword3);
    send_cmd(NULL, "CAPAB TS5 EX IE HOPS HUB AOPS");
#elif defined(IRC_PTLINK)
    if (servernum == 1)
        send_cmd(NULL, "PASS %s :TS", RemotePassword);
    else if (servernum == 2)
        send_cmd(NULL, "PASS %s :TS", RemotePassword2);
    else if (servernum == 3)
        send_cmd(NULL, "PASS %s :TS", RemotePassword3);
#else
    if (servernum == 1)
        send_cmd(NULL, "PASS :%s", RemotePassword);
    if (servernum == 2)
        send_cmd(NULL, "PASS :%s", RemotePassword2);
    if (servernum == 3)
        send_cmd(NULL, "PASS :%s", RemotePassword3);
#endif
#ifdef IRC_PTLINK
    send_cmd(NULL, "SERVER %s 1 Anope.Services%s :%s",
             ServerName, version_number, ServerDesc);
#else
    send_cmd(NULL, "SERVER %s 1 :%s", ServerName, ServerDesc);
#endif
#ifdef IRC_RAGE2
    send_cmd(NULL, "SVINFO 5 5 0 %ld bluemoon 0", time(NULL));
#endif
#if defined(IRC_BAHAMUT) && !defined(IRC_RAGE2)
    send_cmd(NULL, "SVINFO 3 1 0 :%ld", time(NULL));
#endif
#ifdef IRC_HYBRID
    send_cmd(NULL, "SVSINFO 5 5 0 :%ld", time(NULL));
#endif
#ifdef IRC_PTLINK
    send_cmd(NULL, "SVINFO 3 6 %lu", time(NULL));
    send_cmd(NULL, "SVSINFO %lu %d", time(NULL), maxusercnt);
#endif
    sgets2(inbuf, sizeof(inbuf), servsock);
    if (strnicmp(inbuf, "ERROR", 5) == 0) {
        /* Close server socket first to stop wallops, since the other
         * server doesn't want to listen to us anyway */
        disconn(servsock);
        servsock = -1;
        fatal("Remote server returned: %s", inbuf);
    }

    /* Announce a logfile error if there was one */
    if (openlog_failed) {
        wallops(NULL, "Warning: couldn't open logfile: %s",
                strerror(openlog_errno));
    }

    /* Bring in our pseudo-clients */
    introduce_user(NULL);

    /**
      * Load our delayed modeles - modules that are planing on making clients need to wait till now
      * where as modules wanting to modify our ircd connection messages need to load eariler :|
      **/
    modules_delayed_init();

    /* Write the StartGlobal */
    if (GlobalOnCycle) {
        if (GlobalOnCycleUP)
            oper_global(NULL, GlobalOnCycleUP);
    }

    /* Success! */
    return 0;
}

/*************************************************************************/
