/* Services -- main source file.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 *
 */

#include "services.h"
#include "timeout.h"
#include "version.h"
#include "datafiles.h"

/******** Global variables! ********/

/* Command-line options: (note that configuration variables are in config.c) */
char *services_dir = SERVICES_DIR;      /* -dir dirname */
char *log_filename = LOG_FILENAME;      /* -log filename */
int debug = 0;                  /* -debug */
int readonly = 0;               /* -readonly */
int logchan = 0;                /* -logchan */
int skeleton = 0;               /* -skeleton */
int nofork = 0;                 /* -nofork */
int forceload = 0;              /* -forceload */
int noexpire = 0;               /* -noexpire */
#ifdef IS44_CONVERTER
int is44 = 0;                   /* -is44 */
#endif

#ifdef USE_RDB
int do_mysql = 0;               /* use mysql ? */
#endif

/* Set to 1 if we are to quit */
int quitting = 0;

/* Set to 1 if we are to quit after saving databases */
int delayed_quit = 0;

/* Contains a message as to why services is terminating */
char *quitmsg = NULL;

/* Input buffer - global, so we can dump it if something goes wrong */
char inbuf[BUFSIZE];

/* Socket for talking to server */
int servsock = -1;

/* Should we update the databases now? */
int save_data = 0;

/* At what time were we started? */
time_t start_time;

/* Parameters and environment */
char **my_av, **my_envp;

/* Moved here from version.h */
const char version_number[] = VERSION_STRING;
const char version_build[] =
    "build #" BUILD ", compiled " __DATE__ " " __TIME__;
/* the space is needed cause if you build with nothing it will complain */
const char version_flags[] =
    " " VER_DEBUG VER_ENCRYPTION VER_THREAD VER_OS VER_GHBNR VER_MYSQL
    VER_MODULE;

/******** Local variables! ********/

/* Set to 1 if we are waiting for input */
static int waiting = 0;

/* Set to 1 after we've set everything up */
static int started = 0;

/*************************************************************************/

/* Run expiration routines */

extern void expire_all(void)
{
    waiting = -3;
    if (debug)
        alog("debug: Running expire routines");
    if (!skeleton) {
        waiting = -21;
        expire_nicks();
        waiting = -22;
        expire_chans();
        waiting = -23;
        expire_requests();
    }
    waiting = -25;
    expire_akills();
    if (ircd->sgline) {
        waiting = -26;
        expire_sglines();
    }
    if (ircd->sqline) {
        waiting = -28;
        expire_sqlines();
    }
    if (ircd->szline) {
        waiting = -27;
        expire_szlines();
    }
#ifndef STREAMLINED
    expire_exceptions();
#endif
#ifdef USE_THREADS
    if (ProxyDetect)
        proxy_expire();
#endif
}

/*************************************************************************/

void save_databases(void)
{
    waiting = -2;
    if (debug)
        alog("debug: Saving FFF databases");
    waiting = -10;
    backup_databases();
    if (!skeleton) {
        waiting = -11;
        save_ns_dbase();
        waiting = -12;
        if (PreNickDBName) {
            save_ns_req_dbase();
            waiting = -13;
        }
        save_cs_dbase();
        if (s_BotServ) {
            waiting = -14;
            save_bs_dbase();
        }
        if (s_HostServ) {
            waiting = -15;
            save_hs_dbase();
        }
    }
    waiting = -16;
    save_os_dbase();
    waiting = -17;
    save_news();
    waiting = -18;
    save_exceptions();

#ifdef USE_RDB
    if (do_mysql) {
        if (debug)
            alog("debug: Saving RDB databases");
        waiting = -10;
        if (!skeleton) {
            waiting = -11;
            save_ns_rdb_dbase();
            anope_cmd_pong(ServerName, ServerName);
            waiting = -12;
            save_cs_rdb_dbase();
            anope_cmd_pong(ServerName, ServerName);
            if (PreNickDBName) {
                save_ns_req_rdb_dbase();
                anope_cmd_pong(ServerName, ServerName);
                waiting = -13;
            }
            if (s_BotServ) {
                waiting = -14;
                save_bs_rdb_dbase();
                anope_cmd_pong(ServerName, ServerName);
            }
            if (s_HostServ) {
                waiting = -15;
                save_hs_rdb_dbase();
                anope_cmd_pong(ServerName, ServerName);
            }
            waiting = -16;
            save_os_rdb_dbase();
            anope_cmd_pong(ServerName, ServerName);
            waiting = -17;
            save_rdb_news();
            anope_cmd_pong(ServerName, ServerName);
            waiting = -18;
            save_rdb_exceptions();
            anope_cmd_pong(ServerName, ServerName);

        }
    }
#endif
}

/*************************************************************************/

/* Restarts services */

static void services_restart(void)
{
    alog("Restarting");
    if (!quitmsg)
        quitmsg = "Restarting";
    anope_cmd_squit(ServerName, quitmsg);
    disconn(servsock);
    close_log();
#if defined(LINUX20) || defined(LINUX22)
    pthread_kill_other_threads_np();
#endif
    execve(SERVICES_BIN, my_av, my_envp);
    if (!readonly) {
        open_log();
        log_perror("Restart failed");
        close_log();
    }
}

/*************************************************************************/
/**
 * Added to allow do_restart from operserv access to the static functions without making them
 * fair game to every other function - not exactly ideal :|
 **/
void do_restart_services(void)
{
    expire_all();
    save_databases();
    services_restart();
    exit(1);
}

/*************************************************************************/

/* Terminates services */

static void services_shutdown(void)
{
    if (!quitmsg)
        quitmsg = "Terminating, reason unknown";
    alog("%s", quitmsg);
    if (started)
        anope_cmd_squit(ServerName, quitmsg);
    disconn(servsock);
}

/*************************************************************************/

/* If we get a weird signal, come here. */

void sighandler(int signum)
{
    if (started) {
        if (signum == SIGHUP) { /* SIGHUP = save databases and restart */
            signal(SIGHUP, SIG_IGN);
            signal(SIGUSR2, SIG_IGN);
            alog("Received SIGHUP, restarting.");

            expire_all();
            save_databases();

            if (!quitmsg)
                quitmsg = "Restarting on SIGHUP";

#ifdef SERVICES_BIN
            services_restart();
            exit(1);
#else
            quitmsg =
                "Restart attempt failed--SERVICES_BIN not defined (rerun configure)";
#endif

        } else if (signum == SIGUSR2) {

            alog("Received SIGUSR2: Saving Databases & Rehash Configuration");

            expire_all();
            save_databases();

            if (!read_config(1)) {
                sprintf(quitmsg,
                        "Error Reading Configuration File (Received SIGUSR2)");
                quitting = 1;
            }
            return;

        } else if (signum == SIGTERM) {
            signal(SIGTERM, SIG_IGN);
            signal(SIGHUP, SIG_IGN);

            alog("Received SIGTERM, exiting.");

            expire_all();
            save_databases();
            quitmsg = "Shutting down on SIGTERM";
            services_shutdown();
            exit(0);
        } else if (signum == SIGINT || signum == SIGQUIT) {
            /* nothing -- terminate below */
        } else if (!waiting) {
            alog("PANIC! buffer = %s", inbuf);
            /* Cut off if this would make IRC command >510 characters. */
            if (strlen(inbuf) > 448) {
                inbuf[446] = '>';
                inbuf[447] = '>';
                inbuf[448] = 0;
            }
            anope_cmd_global(NULL, "PANIC! buffer = %s\r\n", inbuf);
        } else if (waiting < 0) {
            /* This is static on the off-chance we run low on stack */
            static char buf[BUFSIZE];
            switch (waiting) {
            case -1:
                snprintf(buf, sizeof(buf), "in timed_update");
                break;
            case -10:
                snprintf(buf, sizeof(buf), "backing up databases");
                break;
            case -11:
                snprintf(buf, sizeof(buf), "saving %s", NickDBName);
                break;
            case -12:
                snprintf(buf, sizeof(buf), "saving %s", ChanDBName);
                break;
            case -13:
                snprintf(buf, sizeof(buf), "saving %s", PreNickDBName);
                break;
            case -14:
                snprintf(buf, sizeof(buf), "saving %s", BotDBName);
                break;
            case -15:
                snprintf(buf, sizeof(buf), "saving %s", HostDBName);
                break;
            case -16:
                snprintf(buf, sizeof(buf), "saving %s", OperDBName);
                break;
            case -17:
                snprintf(buf, sizeof(buf), "saving %s", NewsDBName);
                break;
            case -18:
                snprintf(buf, sizeof(buf), "saving %s", ExceptionDBName);
                break;
            case -21:
                snprintf(buf, sizeof(buf), "expiring nicknames");
                break;
            case -22:
                snprintf(buf, sizeof(buf), "expiring channels");
                break;
            case -25:
                snprintf(buf, sizeof(buf), "expiring autokills");
                break;
            case -26:
                snprintf(buf, sizeof(buf), "expiring SGLINEs");
                break;
            case -27:
                snprintf(buf, sizeof(buf), "expiring SZLINEs");
                break;
            case -28:
                snprintf(buf, sizeof(buf), "expiring SQLINEs");
                break;
            default:
                snprintf(buf, sizeof(buf), "waiting=%d", waiting);
            }
            anope_cmd_global(NULL, "PANIC! %s (%s)", buf,
                             strsignal(signum));
            alog("PANIC! %s (%s)", buf, strsignal(signum));
        }
    }

    if (
#if !defined(USE_THREADS) || !defined(LINUX20)
           signum == SIGUSR1 ||
#endif
           !(quitmsg = calloc(BUFSIZE, 1))) {
        quitmsg = "Out of memory!";
    } else {
#if HAVE_STRSIGNAL
        snprintf(quitmsg, BUFSIZE, "Services terminating: %s",
                 strsignal(signum));
#else
        snprintf(quitmsg, BUFSIZE, "Services terminating on signal %d",
                 signum);
#endif
    }
    if (started) {
        services_shutdown();
        exit(0);
    } else {
        alog("%s", quitmsg);
        if (isatty(2))
            fprintf(stderr, "%s\n", quitmsg);
        exit(1);
    }
}

/*************************************************************************/

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
{
    volatile time_t last_update;        /* When did we last update the databases? */
    volatile time_t last_expire;        /* When did we last expire nicks/channels? */
    volatile time_t last_check; /* When did we last check timeouts? */
    volatile time_t last_DefCon;        /* When was DefCon last checked? */

    int i;
    char *progname;

    my_av = av;
    my_envp = envp;

    /* Find program name. */
    if ((progname = strrchr(av[0], '/')) != NULL)
        progname++;
    else
        progname = av[0];

    /* Were we run under "listnicks" or "listchans"?  Do appropriate stuff
     * if so. */
    if (strcmp(progname, "listnicks") == 0) {
        do_listnicks(ac, av);
        return 0;
    } else if (strcmp(progname, "listchans") == 0) {
        do_listchans(ac, av);
        return 0;
    }


    /* Initialization stuff. */
    if ((i = init(ac, av)) != 0)
        return i;


    /* We have a line left over from earlier, so process it first. */
    process();

    /* Set up timers. */
    last_update = time(NULL);
    last_expire = time(NULL);
    last_check = time(NULL);
    last_DefCon = time(NULL);

    started = 1;

    /*** Main loop. ***/

    while (!quitting) {
        time_t t = time(NULL);

        if (debug >= 2)
            alog("debug: Top of main loop");

        if (!noexpire && !readonly
            && (save_data || t - last_expire >= ExpireTimeout)) {
            expire_all();
            last_expire = t;
        }

        if (!readonly && (save_data || t - last_update >= UpdateTimeout)) {
            if (delayed_quit)
                anope_cmd_global(NULL,
                                 "Updating databases on shutdown, please wait.");

            save_databases();

            if (save_data < 0)
                break;          /* out of main loop */

            save_data = 0;
            last_update = t;
        }

        if ((DefConTimeOut) && (t - last_DefCon >= dotime(DefConTimeOut))) {
            resetDefCon(5);
            last_DefCon = t;
        }

        if (delayed_quit)
            break;

        moduleCallBackRun();

        waiting = -1;
        if (t - last_check >= TimeoutCheck) {
            check_timeouts();
            last_check = t;
        }

        waiting = 1;
        i = (int) (long) sgets2(inbuf, sizeof(inbuf), servsock);
        waiting = 0;
        if (i > 0) {
            process();
        } else if (i == 0) {
            int errno_save = errno;
            quitmsg = scalloc(BUFSIZE, 1);
            if (quitmsg) {
                snprintf(quitmsg, BUFSIZE,
                         "Read error from server: %s (error num: %d)",
                         strerror(errno_save), errno_save);
            } else {
                quitmsg = "Read error from server";
            }
            quitting = 1;
        }
        waiting = -4;
    }


    /* Check for restart instead of exit */
    if (save_data == -2) {
#ifdef SERVICES_BIN
        alog("Restarting");
        if (!quitmsg)
            quitmsg = "Restarting";
        anope_cmd_squit(ServerName, quitmsg);
        disconn(servsock);
        close_log();
#if defined(LINUX20) || defined(LINUX22)
        pthread_kill_other_threads_np();
#endif
        execve(SERVICES_BIN, av, envp);
        if (!readonly) {
            open_log();
            log_perror("Restart failed");
            close_log();
        }
        return 1;
#else
        quitmsg =
            "Restart attempt failed--SERVICES_BIN not defined (rerun configure)";
#endif
    }

    /* Disconnect and exit */
    services_shutdown();
    return 0;
}

/*************************************************************************/
