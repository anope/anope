/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSStatus : public Command
{
 public:
	CommandCSStatus() : Command("STATUS", 2, 2, "chanserv/status")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci;
		User *u2;
		Anope::string chan = params[0];
		Anope::string nick = params[1];
		Anope::string temp;

		if (!(ci = cs_findchan(chan)))
		{
			temp = chan;
			chan = nick;
			nick = temp;
			ci = cs_findchan(chan);
		}
		if (!ci)
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_NOT_REGGED, temp.c_str());
		else if ((u2 = finduser(nick)))
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_INFO, chan.c_str(), nick.c_str(), get_access(u2, ci));
		else /* !u2 */
			notice_lang(Config.s_ChanServ, u, CHAN_STATUS_NOTONLINE, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_STATUS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
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
	CommandCSStatus commandcsstatus;

 public:
	CSStatus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsstatus);
	}
};

MODULE_INIT(CSStatus)
