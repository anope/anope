/* Routines to handle `listnicks' and `listchans' invocations.
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

/**
 * List all register nicks
 * @param ac Number of Arguments
 * @param av Array if Arguments
 * @return void
 */
void do_listnicks(int ac, char **av)
{
    int count = 0;              /* Count only rather than display? */
    int usage = 0;              /* Display command usage?  (>0 also indicates error) */
    int i;

    i = 1;
    while (i < ac) {
        if (av[i][0] == '-') {
            switch (av[i][1]) {
            case 'h':
                usage = -1;
                break;
            case 'c':
                if (i > 1)
                    usage = 1;
                count = 1;
                break;
            case 'd':
                if (av[i][2]) {
                    services_dir = av[i] + 2;
                } else {
                    if (i >= ac - 1) {
                        usage = 1;
                        break;
                    }
                    ac--;
                    memmove(av + i, av + i + 1, sizeof(char *) * ac - i);
                    services_dir = av[i];
                }
            default:
                usage = 1;
                break;
            }                   /* switch */
            ac--;
            if (i < ac)
                memmove(av + i, av + i + 1, sizeof(char *) * ac - i);
        } else {
            if (count)
                usage = 1;
            i++;
        }
    }
    if (usage) {
        fprintf(stderr, "\
\n\
Usage: listnicks [-c] [-d data-dir] [nick [nick...]]\n\
     -c: display only count of registered nicks\n\
            (cannot be combined with nicks)\n\
   nick: nickname(s) to display information for\n\
\n\
If no nicks are given, the entire nickname database is printed out in\n\
compact format followed by the number of registered nicks (with -c, the\n\
list is suppressed and only the count is printed).  If one or more nicks\n\
are given, detailed information about those nicks is displayed.\n\
\n");
        exit(usage > 0 ? 1 : 0);
    }

    if (chdir(services_dir) < 0) {
        fprintf(stderr, "chdir(%s): %s\n", services_dir, strerror(errno));
        ModuleRunTimeDirCleanUp();
        exit(1);
    }
    if (!read_config(0)) {
        ModuleRunTimeDirCleanUp();
        exit(1);
    }
    load_ns_dbase();

    lang_init();

    if (ac > 1) {
        for (i = 1; i < ac; i++)
            listnicks(0, av[i]);
    } else {
        listnicks(count, NULL);
    }
    exit(0);
}

/*************************************************************************/

/**
 * List all register channels
 * @param ac Number of Arguments
 * @param av Array if Arguments
 * @return void
 */
void do_listchans(int ac, char **av)
{
    int count = 0;              /* Count only rather than display? */
    int usage = 0;              /* Display command usage?  (>0 also indicates error) */
    int i;

    i = 1;
    while (i < ac) {
        if (av[i][0] == '-') {
            switch (av[i][1]) {
            case 'h':
                usage = -1;
                break;
            case 'c':
                if (i > 1)
                    usage = 1;
                count = 1;
                break;
            case 'd':
                if (av[i][2]) {
                    services_dir = av[i] + 2;
                } else {
                    if (i >= ac - 1) {
                        usage = 1;
                        break;
                    }
                    ac--;
                    memmove(av + i, av + i + 1, sizeof(char *) * ac - i);
                    services_dir = av[i];
                }
            default:
                usage = 1;
                break;
            }                   /* switch */
            ac--;
            if (i < ac)
                memmove(av + i, av + i + 1, sizeof(char *) * ac - i);
        } else {
            if (count)
                usage = 1;
            i++;
        }
    }
    if (usage) {
        fprintf(stderr, "\
\n\
Usage: listchans [-c] [-d data-dir] [channel [channel...]]\n\
     -c: display only count of registered channels\n\
            (cannot be combined with channels)\n\
channel: channel(s) to display information for\n\
\n\
If no channels are given, the entire channel database is printed out in\n\
compact format followed by the number of registered channels (with -c, the\n\
list is suppressed and only the count is printed).  If one or more channels\n\
are given, detailed information about those channels is displayed.\n\
\n\
Channel names will need to be escaped with a backslash.\n\
For example: ./listchans \\#Anope\n\
\n");
        exit(usage > 0 ? 1 : 0);
    }

    if (chdir(services_dir) < 0) {
        fprintf(stderr, "chdir(%s): %s\n", services_dir, strerror(errno));
        ModuleRunTimeDirCleanUp();
        exit(1);
    }
    if (!read_config(0)) {
        ModuleRunTimeDirCleanUp();
        exit(1);
    }
    load_ns_dbase();
    load_cs_dbase();

    lang_init();

    if (ac > 1) {
        for (i = 1; i < ac; i++)
            listchans(0, av[i]);
    } else {
        listchans(count, NULL);
    }
    exit(0);
}

/*************************************************************************/
