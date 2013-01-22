/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSUpdate : public Command
{
 public:
	CommandNSUpdate(Module *creator) : Command(creator, "nickserv/update", 0, 0)
	{
		this->SetDesc(_("Updates your current status, i.e. it checks for new memos"));
		this->SetSyntax("");
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		NickAlias *na = NickAlias::Find(u->nick);

		if (na && na->nc == source.GetAccount())
		{
			na->last_realname = u->realname;
			na->last_seen = Anope::CurTime;
		}

		FOREACH_MOD(I_OnNickUpdate, OnNickUpdate(u));

		source.Reply(_("Status updated (memos, vhost, chmodes, flags)."), Config->NickServ.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Updates your current status, i.e. it checks for new memos,\n"
				"sets needed channel modes and updates your vhost and\n"
				"your userflags (lastseentime, etc)."));
		return true;
	}
};

class NSUpdate : public Module
{
	CommandNSUpdate commandnsupdate;

 public:
	NSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsupdate(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSUpdate)
