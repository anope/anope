/**
 * Simple module to load up a client called CatServ and process commands for it
 * This module is an example, and has no useful purpose!
 *
 * Please visit http://modules.anope.org for useful modules! 
 *
 **/

#include "module.h"

#define AUTHOR "Anope"
#define VERSION "1.1"

int my_privmsg(char *source, int ac, char **av);
CommandHash *Catserv_cmdTable[MAX_CMD_HASH];

void addClient(char *nick, char *realname);
void addMessageList(void);
void delClient(void);
char *s_CatServ = "CatServ";
void catserv(User * u, char *buf);

int do_meow(User * u);
int do_purr(User * u);

int AnopeInit(int argc, char **argv)
{
    Message *msg = NULL;
    int status;
    msg = createMessage("PRIVMSG", my_privmsg);
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
    NEWNICK(nick, "catserv", "meow.meow.land", realname, "+", 1);
}

void delClient(void)
{
    send_cmd(s_CatServ, "QUIT :Module Unloaded!");
}

void addMessageList(void)
{
    Command *c;
    c = createCommand("meow", do_meow, NULL, -1, -1, -1, -1, -1);
    addCommand(Catserv_cmdTable, c, MOD_UNIQUE);
    c = createCommand("purr", do_purr, NULL, -1, -1, -1, -1, -1);
    addCommand(Catserv_cmdTable, c, MOD_UNIQUE);
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

int do_meow(User * u)
{
    notice(s_CatServ, u->nick, "MEOW!");
    return MOD_STOP;
}

int do_purr(User * u)
{
    notice(s_CatServ, u->nick, "PURR!");
    return MOD_STOP;
}

