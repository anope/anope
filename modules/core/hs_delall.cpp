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
#include "hostserv.h"

class CommandHSDelAll : public Command
{
 public:
	CommandHSDelAll() : Command("DELALL", 1, 1, "hostserv/del")
	{
		this->SetDesc(_("Delete the vhost for all nicks in a group"));
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
				source.Reply(_(NICK_X_FORBIDDEN), nick.c_str());
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
			source.Reply(_("vhosts for group \002%s\002 have been removed."), nc->display.c_str());
		}
		else
			source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DELALL\002 \002<nick>\002.\n"
				"Deletes the vhost for all nicks in the same group as\n"
				"that of the given nick."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DELALL", _("DELALL <nick>\002."));
	}
};

class HSDelAll : public Module
{
	CommandHSDelAll commandhsdelall;

 public:
	HSDelAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!hostserv)
			throw ModuleException("HostServ is not loaded!");

		this->AddCommand(hostserv->Bot(), &commandhsdelall);
	}
};

MODULE_INIT(HSDelAll)
