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

class CommandHSDel : public Command
{
 public:
	CommandHSDel() : Command("DEL", 1, 1, "hostserv/del")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];
		NickAlias *na = findnick(nick);
		if (na)
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(LanguageString::NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}
			Log(LOG_ADMIN, u, this) << "for user " << na->nick;
			FOREACH_MOD(I_OnDeleteVhost, OnDeleteVhost(na));
			na->hostinfo.RemoveVhost();
			source.Reply(_("vhost for \002%s\002 removed."), nick.c_str());
		}
		else
			source.Reply(LanguageString::NICK_X_NOT_REGISTERED, nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEL\002 \002<nick>\002\n"
				"Deletes the vhost assigned to the given nick from the\n"
				"database."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEL", _("DEL \002<nick>\002."));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    DEL         Delete the vhost of another user"));
	}
};

class HSDel : public Module
{
	CommandHSDel commandhsdel;

 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsdel);
	}
};

MODULE_INIT(HSDel)
