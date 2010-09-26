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
			u->SendMessage(ChanServ, CHAN_STATUS_NOT_REGGED, temp.c_str());
		else if ((u2 = finduser(nick)))
			u->SendMessage(ChanServ, CHAN_STATUS_INFO, chan.c_str(), nick.c_str(), get_access(u2, ci));
		else /* !u2 */
			u->SendMessage(ChanServ, CHAN_STATUS_NOTONLINE, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_SERVADMIN_HELP_STATUS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "STATUS", CHAN_STATUS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_STATUS);
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
