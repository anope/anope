/* OperServ core functions
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

class CommandOSNOOP : public Command
{
 public:
	CommandOSNOOP(Module *creator) : Command(creator, "operserv/noop", 2, 2)
	{
		this->SetDesc(_("Remove all operators from a server remotely"));
		this->SetSyntax(_("SET \037server\037"));
		this->SetSyntax(_("REVOKE \037server\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &cmd = params[0];
		const Anope::string &server = params[1];

		Server *s = Server::Find(server);
		if (s == NULL)
			source.Reply(_("Server %s does not exist."), server.c_str());
		else if (s == Me || s->IsJuped())
			source.Reply(_("You can not NOOP Services."));
		else if (cmd.equals_ci("SET"))
		{
			/* Remove the O:lines */
			IRCD->SendSVSNOOP(s, true);
			s->Extend("noop", new ExtensibleItemClass<Anope::string>(source.GetNick()));

			Log(LOG_ADMIN, source, this) << "SET on " << s->GetName();
			source.Reply(_("All operators from \002%s\002 have been removed."), s->GetName().c_str());

			Anope::string reason = "NOOP command used by " + source.GetNick();
			/* Kill all the IRCops of the server */
			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u2 = it->second;

				if (u2->server == s && u2->HasMode("OPER"))
					u2->Kill(Config->OperServ, reason);
			}
		}
		else if (cmd.equals_ci("REVOKE"))
		{
			s->Shrink("noop");
			IRCD->SendSVSNOOP(s, false);
			Log(LOG_ADMIN, source, this) << "REVOKE on " << s->GetName();
			source.Reply(_("All O:lines of \002%s\002 have been reset."), s->GetName().c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("\002SET\002 kills all operators from the given\n"
				"\002server\002 and prevents operators from opering\n"
				"up on the given server. \002REVOKE\002 removes this\n"
				"restriction."));
		return true;
	}
};

class OSNOOP : public Module
{
	CommandOSNOOP commandosnoop;

 public:
	OSNOOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosnoop(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::Attach(I_OnUserModeSet, this);
	}

	void OnUserModeSet(User *u, const Anope::string &mname) anope_override
	{
		if (mname == "OPER" && u->server->HasExt("noop"))
		{
			Anope::string *setter = u->server->GetExt<ExtensibleItemClass<Anope::string> *>("noop");
			if (setter)
			{
				Anope::string reason = "NOOP command used by " + *setter;
				u->Kill(Config->OperServ, reason);
			}
		}
	}
};

MODULE_INIT(OSNOOP)
