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
#define VERSION "$Id$"

int my_privmsg(char *source, int ac, char **av);

void addClient(char *nick, char *realname);
void delClient(void);
void catserv(User * u, char *buf);

char *s_CatServ = "CatServ";

int AnopeInit(int argc, char **argv)
{
    Message *msg = NULL;
    int status;
#ifdef IRC_UNREAL32
    if (UseTokens) {
     msg = createMessage("!", my_privmsg);
    } else {
     msg = createMessage("PRIVMSG", my_privmsg);
    }
#else
    msg = createMessage("PRIVMSG", my_privmsg);
#endif
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
    char *s;

    /* First, some basic checks */
    if (ac != 2)
        return MOD_CONT;        /* bleh */
    if (!(u = finduser(source))) {
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
    if ((stricmp(av[0], s_CatServ)) == 0) {     /* its for US! */
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
    } else {
        mod_run_cmd(s_CatServ, u, Catserv_cmdTable, cmd);
    }
}

