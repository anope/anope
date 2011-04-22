/* ChanServ core functions
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
#include "chanserv.h"

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban() : Command("UNBAN", 1, 2)
	{
		this->SetDesc(_("Remove all bans preventing a user from entering a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
		{
			source.Reply(_(CHAN_X_NOT_IN_USE), ci->name.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_UNBAN))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		User *u2 = u;
		if (params.size() > 1)
			u2 = finduser(params[1]);

		if (!u2)
		{
			source.Reply(_(NICK_X_NOT_IN_USE), params[1].c_str());
			return MOD_CONT;
		}

		common_unban(ci, u2, u == u2);
		if (u2 == u)
			source.Reply(_("You have been unbanned from \002%s\002."), c->name.c_str());
		else
			source.Reply(_("\002%s\002 has been unbanned from \002%s\002."), u2->nick.c_str(), c->name.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002UNBAN \037channel\037 [\037nick\037]\002\n"
				" \n"
				"Tells %s to remove all bans preventing you or the given\n"
				"user from entering the given channel.  \n"
				" \n"
				"By default, limited to AOPs or those with level 5 and above\n"
				"on the channel."), Config->s_ChanServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNBAN", _("UNBAN \037channel\037 [\037nick\037]"));
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

		this->AddCommand(chanserv->Bot(), &commandcsunban);
	}
};

MODULE_INIT(CSUnban)
