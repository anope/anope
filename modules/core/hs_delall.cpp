/* HostServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSDelAll : public Command
{
 public:
	CommandHSDelAll() : Command("DELALL", 1, 1, "hostserv/del")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[0];
		User *u = source.u;
		NickAlias *na = findnick(nick);
		if (na)
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			NickCore *nc = na->nc;
			for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
			{
				na = *it;
				na->hostinfo.RemoveVhost();
			}
			Log(LOG_ADMIN, u, this) << "for all nicks in group " << nc->display;
			source.Reply(HOST_DELALL, nc->display.c_str());
		}
		else
			source.Reply(HOST_NOREG, nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(HOST_HELP_DELALL);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DELALL", HOST_DELALL_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(HOST_HELP_CMD_DELALL);
	}
};

class HSDelAll : public Module
{
	CommandHSDelAll commandhsdelall;

 public:
	HSDelAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsdelall);
	}
};

MODULE_INIT(HSDelAll)
