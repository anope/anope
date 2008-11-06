/* BotServ core functions
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
/*************************************************************************/

#include "module.h"

int do_act(User * u);
void myBotServHelp(User * u);

class BSAct : public Module
{
 public:
	BSAct(const std::string &creator) : Module(creator)
	{
	    Command *c;

	    moduleAddAuthor("Anope");
	    moduleAddVersion("$Id$");
	    moduleSetType(CORE);
	    c = createCommand("ACT", do_act, NULL, BOT_HELP_ACT, -1, -1, -1, -1);
	    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);

	    moduleSetBotHelp(myBotServHelp);
	}
};


/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
    notice_lang(s_BotServ, u, BOT_HELP_CMD_ACT);
}

/**
 * The /bs act command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_act(User * u)
{
    ChannelInfo *ci;

    char *chan = strtok(NULL, " ");
    char *text = strtok(NULL, "");

    if (!chan || !text)
        syntax_error(s_BotServ, u, "ACT", BOT_ACT_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!ci->bi)
        notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
    else if (!ci->c || ci->c->usercount < BSMinUsers)
        notice_lang(s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name);
    else if (!check_access(u, ci, CA_SAY))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        strnrepl(text, BUFSIZE, "\001", "");
        ircdproto->SendAction(ci->bi, ci->name, "%s", text);
        ci->bi->lastmsg = time(NULL);
        if (LogBot && LogChannel && logchan && !debug && findchan(LogChannel))
            ircdproto->SendPrivmsg(ci->bi, LogChannel, "ACT %s %s %s",
                              u->nick, ci->name, text);
    }
    return MOD_CONT;
}

MODULE_INIT("bs_act", BSAct)
