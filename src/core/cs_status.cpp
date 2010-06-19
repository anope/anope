/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSStatus : public Command
{
 public:
	CommandCSStatus() : Command("STATUS", 2, 2, "chanserv/status")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci;
		User *u2;
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		const char *temp = NULL;

		if (!(ci = cs_findchan(chan)))
		{
			temp = chan;
			chan = nick;
			nick = temp;
			ci = cs_findchan(chan);
		}
		if (!ci)
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_NOT_REGGED, temp);
		else if ((u2 = finduser(nick)))
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_INFO, chan, nick, get_access(u2, ci));
		else /* !u2 */
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_NOTONLINE, nick);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_STATUS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "STATUS", CHAN_STATUS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_STATUS);
	}
};

class CSStatus : public Module
{
 public:
	CSStatus(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
	}
};

MODULE_INIT(CSStatus)
