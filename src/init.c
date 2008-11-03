/* Initalization and related routines.
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
#include "pseudo.h"
int servernum = 0;

extern void moduleAddMsgs(void);
extern void moduleAddIRCDMsgs(void);

/*************************************************************************/

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
    /* We make the bots go online */
	BotInfo *bi;
	int i;

	/* XXX: it might be nice to have this inside BotInfo's constructor, or something? */
	for (i = 0; i < 256; i++)
	{
		for (bi = botlists[i]; bi; bi = bi->next)
		{
			if (!user || !stricmp(user, bi->nick))
				ircdproto->SendClientIntroduction(bi->nick, bi->user, bi->host, bi->real, ircd->pseudoclient_mode, bi->uid.c_str());
		}
	}
}

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
            } else if (strcmp(s, "version") == 0) {
                fprintf(stdout, "Anope-%s %s -- %s\n", version_number,
                        version_flags, version_build);
                exit(EXIT_SUCCESS);
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
                    int portnum;
                    *t++ = 0;
                    portnum = atoi(t);
                    if ((portnum > 0) && (portnum < 65535))
                        RemotePort = portnum;
                    else {
                        fprintf(stderr,
                                "-remote: Port numbers must be in the range 1..65535. Using default.\n");
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
                    int portnum;
                    *t++ = 0;
                    portnum = atoi(t);
                    if ((portnum >= 0) && (portnum < 65535))
                        LocalPort = portnum;
                    else {
                        fprintf(stderr,
                                "-local: Port numbers must be in the range 1..65535 or 0. Using default.\n");
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
                            "-expire: number of seconds must be positive\n");
                    return -1;
                } else
                    ExpireTimeout = atol(s);
            } else if (strcmp(s, "debug") == 0) {
                debug++;
            } else if (strcmp(s, "readonly") == 0) {
                readonly = 1;
            } else if (strcmp(s, "nofork") == 0) {
                nofork = 1;
            } else if (strcmp(s, "logchan") == 0) {
                if (!LogChannel) {
                    fprintf(stderr,
                            "-logchan: LogChannel must be defined in services.conf\n");
                } else {        /* LogChannel */

                    logchan = 1;
                }
            } else if (strcmp(s, "forceload") == 0) {
                forceload = 1;
            } else if (strcmp(s, "nothird") == 0) {
                nothird = 1;
            } else if (strcmp(s, "protocoldebug") == 0) {
                protocoldebug = 1;
            } else if (strcmp(s, "support") == 0) {
                nofork = 1;
                debug++;
                nothird = 1;
            } else if (!strcmp(s, "noexpire")) {
                noexpire = 1;
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
                fprintf(stdout, "-forceload     -forceload\n");
                fprintf(stdout, "-nothird       -nothird\n");
                fprintf(stdout, "-support       -support\n");
                fprintf(stdout, "-readonly      -readonly\n");
                fprintf(stdout, "-noexpire      -noexpire\n");
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

/* Overall initialization routines.  Return 0 on success, -1 on failure. */

int openlog_failed = 0, openlog_errno = 0;

int init_primary(int ac, char **av)
{
    int started_from_term = isatty(0) && isatty(1) && isatty(2);

    /* Set file creation mask and group ID. */
#if defined(DEFUMASK) && HAVE_UMASK
    umask(DEFUMASK);
#endif
    if (set_group() < 0)
        return -1;

    /* Parse command line for -dir and -version options. */
    parse_dir_options(ac, av);
    /* Parse all remaining command-line options. */
    parse_options(ac, av);

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
    if (!read_config(0)) {
        return -1;
    }

    /* Add IRCD Protocol Module; exit if there are errors */
    if (protocol_module_init()) {
        return -1;
    }

    /* Add Encryption Module; exit if there are errors */
    if (encryption_module_init()) {
        return -1;
    }
    return 0;
}

int init_secondary(int ac, char **av)
{
    int i;
    int started_from_term = isatty(0) && isatty(1) && isatty(2);

    /* Add Core MSG handles */
    moduleAddMsgs();

    /* Parse the defcon mode string if needed */
    if (DefConLevel) {
        if (!defconParseModeString(DefConChanModes)) {
            fprintf(stderr,
                    "services.conf: The given DefConChanModes mode string was incorrect (see log for exact errors)\n");
            return -1;
        }
    }
#ifndef _WIN32
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
#else
    /* Initialize winsocks -- codemastr */
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(1, 1), &wsa)) {
            alog("Failed to initialized WinSock library");
            return -1;
        }
    }
    if (!SupportedWindowsVersion()) {

        char *winver = GetWindowsVersion();

        alog("%s is not a supported version of Windows", winver);

        free(winver);

        return -1;

    }
    if (!nofork) {
        alog("Launching Anope into the background");
        FreeConsole();
    }
#endif

    /* Write our PID to the PID file. */
    write_pidfile();

    /* Announce ourselves to the logfile. */
    if (debug || readonly) {
        alog("Anope %s (ircd protocol: %s) starting up (options:%s%s)",
             version_number, version_protocol,
             debug ? " debug" : "", readonly ? " readonly" : "");
    } else {
        alog("Anope %s (ircd protocol: %s) starting up",
             version_number, version_protocol);
    }
    start_time = time(NULL);



    /* If in read-only mode, close the logfile again. */
    if (readonly)
        close_log();

    /* Set signal handlers.  Catch certain signals to let us do things or
     * panic as necessary, and ignore all others.
     */

#ifndef _WIN32
#if defined(NSIG)
    for (i = 1; i <= NSIG - 1; i++) {
#else
    for (i = 1; i <= 31; i++) {
#endif
        signal(i, SIG_IGN);
    }
#else
    /* work around to bug #527 */
    signal(SIGILL, SIG_IGN);
    signal(SIGBREAK, SIG_IGN);
    signal(SIGABRT, SIG_IGN);
#endif

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
#ifndef _WIN32
    signal(SIGQUIT, sighandler);
#endif
    if (!DumpCore) {
        signal(SIGSEGV, sighandler);
#ifndef _WIN32
        signal(SIGBUS, sighandler);
        signal(SIGTRAP, sighandler);
#endif
    } else {
        signal(SIGSEGV, SIG_DFL);
#ifndef _WIN32
        signal(SIGBUS, sighandler);
        signal(SIGTRAP, sighandler);
#endif
    }
#ifndef _WIN32
    signal(SIGQUIT, sighandler);
    signal(SIGHUP, sighandler);
    signal(SIGUSR2, sighandler);
#endif

#ifdef SIGIOT
    signal(SIGIOT, sighandler);
#endif
    signal(SIGFPE, sighandler);

#ifndef _WIN32
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

    /* load any custom modules */
    modules_init();

    /* Initialize random number generator */
    rand_init();
    add_entropy_userkeys();

    /* Load up databases */
	load_ns_dbase();
	if (debug)
	    alog("debug: Loaded %s database (1/%d)", s_NickServ,
		 (PreNickDBName ? 8 : 7));
	if (s_HostServ) {
	    load_hs_dbase();
	    if (debug)
		alog("debug: Loaded %s database (2/%d)", s_HostServ,
		     (PreNickDBName ? 8 : 7));
	} else if (debug) {
	    alog("debug: HostServ database (2/%d) not loaded because HostServ is disabled", (PreNickDBName ? 8 : 7));
	}
	if (s_BotServ) {
	    load_bs_dbase();
	    if (debug)
		alog("debug: Loaded %s database (3/%d)", s_BotServ,
		     (PreNickDBName ? 8 : 7));
	} else if (debug) {
	    alog("debug: BotServ database (3/%d) not loaded because BotServ is disabled", (PreNickDBName ? 8 : 7));
	}
	load_cs_dbase();
	if (debug)
	    alog("debug: Loaded %s database (4/%d)", s_ChanServ,
		 (PreNickDBName ? 8 : 7));
	load_os_dbase();
	if (debug)
	    alog("debug: Loaded %s database (5/%d)", s_OperServ,
		 (PreNickDBName ? 8 : 7));
	load_news();
	if (debug)
	    alog("debug: Loaded news database (6/%d)",
		 (PreNickDBName ? 8 : 7));
	load_exceptions();
	if (debug)
	    alog("debug: Loaded exception database (7/%d)",
		 (PreNickDBName ? 8 : 7));
	if (PreNickDBName) {
	    load_ns_req_db();
	    if (debug)
		alog("debug: Loaded PreNick database (8/8)");
	}
    alog("Databases loaded");


	/* this is only used on the first run of Anope. */
	BotInfo *bi = findbot("NickServ");
	if (!bi)
	{
		if (s_OperServ)
			bi = new BotInfo(s_OperServ, ServiceUser, ServiceHost, desc_OperServ);
		if (s_NickServ)
			bi = new BotInfo(s_NickServ, ServiceUser, ServiceHost, desc_NickServ);
		if (s_ChanServ)
			bi = new BotInfo(s_ChanServ, ServiceUser, ServiceHost, desc_ChanServ);
		if (s_HostServ)
			bi = new BotInfo(s_HostServ, ServiceUser, ServiceHost, desc_HostServ);
		if (s_MemoServ)
			bi = new BotInfo(s_MemoServ, ServiceUser, ServiceHost, desc_MemoServ);
		if (s_BotServ)
			bi = new BotInfo(s_BotServ, ServiceUser, ServiceHost, desc_BotServ);
		if (s_HelpServ)
			bi = new BotInfo(s_HelpServ, ServiceUser, ServiceHost, desc_HelpServ);
		if (s_GlobalNoticer)
			bi = new BotInfo(s_GlobalNoticer, ServiceUser, ServiceHost, desc_GlobalNoticer);
	}

    /* Save the databases back to file/mysql to reflect any changes */
        alog("Info: Reflecting database records.");
        save_databases();
    send_event(EVENT_CONNECT, 1, EVENT_START);

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

    ircdproto->SendConnect();
    send_event(EVENT_CONNECT, 1, EVENT_STOP);

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
        ircdproto->SendGlobops(NULL, "Warning: couldn't open logfile: %s",
                         strerror(openlog_errno));
    }

    /* Bring in our pseudo-clients */
    introduce_user(NULL);

    /* And hybrid needs Global joined in the logchan */
    if (logchan && ircd->join2msg) {
        /* XXX might desync */
        ircdproto->SendJoin(findbot(s_GlobalNoticer), LogChannel, time(NULL));
    }

    ircdproto->SendEOB();

    /**
      * Load our delayed modeles - modules that are planing on making clients need to wait till now
      * where as modules wanting to modify our ircd connection messages need to load eariler :|
      **/
    modules_delayed_init();

    /* Success! */
    return 0;
}

/*************************************************************************/
