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

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban() : Command("UNBAN", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_UNBAN))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		User *u2 = u;
		if (params.size() > 1)
			u2 = finduser(params[1]);

		if (!u2)
		{
			source.Reply(NICK_X_NOT_IN_USE, params[1].c_str());
			return MOD_CONT;
		}

		common_unban(ci, u2->nick);
		if (u2 == u)
			source.Reply(CHAN_UNBANNED, c->name.c_str());
		else
			source.Reply(CHAN_UNBANNED_OTHER, u2->nick.c_str(), c->name.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_UNBAN);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNBAN", CHAN_UNBAN_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_UNBAN);
	}
};

class CSUnban : public Module
{
	CommandCSUnban commandcsunban;

 public:
	CSUnban(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsunban);
	}
};

MODULE_INIT(CSUnban)
