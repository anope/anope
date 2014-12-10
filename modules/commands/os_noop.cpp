/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSNOOP : public Command
{
 public:
	CommandOSNOOP(Module *creator) : Command(creator, "operserv/noop", 2, 2)
	{
		this->SetDesc(_("Remove all operators from a server remotely"));
		this->SetSyntax(_("SET \037server\037"));
		this->SetSyntax(_("REVOKE \037server\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		const Anope::string &server = params[1];

		Server *s = Server::Find(server, true);
		if (s == NULL)
			source.Reply(_("Server \002{0}\002 does not exist."), server);
		else if (s == Me || s->IsJuped() || s->IsULined())
			source.Reply(_("You can not NOOP Services."));
		else if (cmd.equals_ci("SET"))
		{
			/* Remove the O:lines */
			IRCD->SendSVSNOOP(s, true);
			s->Extend<Anope::string>("noop", source.GetNick());

			Log(LOG_ADMIN, source, this) << "SET on " << s->GetName();
			source.Reply(_("All operators from \002{0}\002 have been removed."), s->GetName());

			Anope::string reason = "NOOP command used by " + source.GetNick();
			/* Kill all the IRCops of the server */
			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u2 = it->second;

				if (u2->server == s && u2->HasMode("OPER"))
					u2->Kill(source.service->nick, reason);
			}
		}
		else if (cmd.equals_ci("REVOKE"))
		{
			s->ShrinkOK<Anope::string>("noop");
			IRCD->SendSVSNOOP(s, false);
			Log(LOG_ADMIN, source, this) << "REVOKE on " << s->GetName();
			source.Reply(_("All O:lines of \002{0}\002 have been reset."), s->GetName());
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("\002{0} SET\002 kills all operators from the given \002server\002 and prevents operators from opering up on the given server."
		               " \002{0} REVOKE\002 removes this restriction."));
		return true;
	}
};

class OSNOOP : public Module
	, public EventHook<Event::UserModeSet>
{
	CommandOSNOOP commandosnoop;
	ExtensibleItem<Anope::string> noop;

 public:
	OSNOOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserModeSet>()
		, commandosnoop(this)
		, noop(this, "noop")
	{

	}

	void OnUserModeSet(const MessageSource &, User *u, const Anope::string &mname) override
	{
		Anope::string *setter;
		if (mname == "OPER" && (setter = noop.Get(u->server)))
		{
			Anope::string reason = "NOOP command used by " + *setter;
			ServiceBot *OperServ = Config->GetClient("OperServ");
			u->Kill(OperServ ? OperServ->nick : "", reason);
		}
	}
};

MODULE_INIT(OSNOOP)
