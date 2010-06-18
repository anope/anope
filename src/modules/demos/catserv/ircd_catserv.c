/**
 * Simple module to load up a client called CatServ and process commands for it
 * This module is an example, and has no useful purpose!
 *
 * Please visit http://modules.anope.org for useful modules! 
 *
 **/

#include "module.h"
#include "catserv_messages.h"

#define AUTHOR "Anope"
#define VERSION VERSION_STRING

int my_privmsg(char *source, int ac, char **av);

void addClient(char *nick, char *realname);
void delClient(void);
void catserv(User * u, char *buf);

char *s_CatServ = "CatServ";

int AnopeInit(int argc, char **argv)
{
    Message *msg = NULL;
    int status;
    if (UseTokens) {
     msg = createMessage("!", my_privmsg);
    } else {
     msg = createMessage("PRIVMSG", my_privmsg);
    }
    status = moduleAddMessage(msg, MOD_HEAD);
    if (status == MOD_ERR_OK) {
        addClient(s_CatServ, "meow!");
        addMessageList();
    }
    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    alog("ircd_catserv.so: loaded, message status [%d]", status);
    return MOD_CONT;
}

void AnopeFini(void)
{
    delClient();
}

int my_privmsg(char *source, int ac, char **av)
{
    User *u;
    Uid *ud = NULL;
    char *s;

    /* First, some basic checks */
    if (ac != 2)
        return MOD_CONT;        /* bleh */
    if (ircd->ts6)
        u = find_byuid(source); // If this is a ts6 ircd, find the user by uid
    if (!u && !(u = finduser(source))) { // If user isn't found and we cant find them by nick, return
        return MOD_CONT;
    }                           /* non-user source */
    if (*av[0] == '#') {
        return MOD_CONT;
    }
    /* Channel message */
    /* we should prolly honour the ignore list here, but i cba for this... */
    s = strchr(av[0], '@');
    if (s) {
        *s++ = 0;
        if (stricmp(s, ServerName) != 0)
            return MOD_CONT;
    }
    if (ircd->ts6)
        ud = find_uid(s_CatServ); // Find CatServs UID
    if (stricmp(av[0], s_CatServ) == 0 || (ud && strcmp(av[0], ud->uid) == 0)) {  /* If the nick or the uid matches its for us */
        catserv(u, av[1]);
        return MOD_STOP;
    } else {                    /* ok it isnt us, let the old code have it */
        return MOD_CONT;
    }
}

void addClient(char *nick, char *realname)
{
    anope_cmd_bot_nick(nick, "catserv", "meow.meow.land", realname, "+");
}

void delClient(void)
{
    anope_cmd_quit(s_CatServ, "QUIT :Module Unloaded!");
}

/*****************************************************************************/
/* Main CatServ routine. */
void catserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, "")))
            s = "\1";
        notice(s_CatServ, u->nick, "\1PING %s", s);
    } else if (skeleton) {
        notice_lang(s_CatServ, u, SERVICE_OFFLINE, s_CatServ);
    } else {
        mod_run_cmd(s_CatServ, u, Catserv_cmdTable, cmd);
    }
}

