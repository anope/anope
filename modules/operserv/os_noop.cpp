/* OperServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSNOOP final
	: public Command
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
			source.Reply(_("Server %s does not exist."), server.c_str());
		else if (s == Me || s->IsJuped())
			source.Reply(_("You can not NOOP services."));
		else if (cmd.equals_ci("SET"))
		{
			/* Remove the O:lines */
			IRCD->SendSVSNOOP(s, true);
			s->Extend<Anope::string>("noop", source.GetNick());

			Log(LOG_ADMIN, source, this) << "SET on " << s->GetName();
			source.Reply(_("All operators from \002%s\002 have been removed."), s->GetName().c_str());

			Anope::string reason = "NOOP command used by " + source.GetNick();
			/* Kill all the IRCops of the server */
			for (const auto &[_, u2] : UserListByNick)
			{
				if (u2->server == s && u2->HasMode("OPER"))
					u2->Kill(*source.service, reason);
			}
		}
		else if (cmd.equals_ci("REVOKE"))
		{
			s->Shrink<Anope::string>("noop");
			IRCD->SendSVSNOOP(s, false);
			Log(LOG_ADMIN, source, this) << "REVOKE on " << s->GetName();
			source.Reply(_("All O:lines of \002%s\002 have been reset."), s->GetName().c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
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

class OSNOOP final
	: public Module
{
	CommandOSNOOP commandosnoop;
	PrimitiveExtensibleItem<Anope::string> noop;

public:
	OSNOOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosnoop(this), noop(this, "noop")
	{
		if (!IRCD || !IRCD->CanSVSNOOP)
			throw ModuleException("Your IRCd does not support SVSNOOP.");
	}

	void OnUserModeSet(const MessageSource &, User *u, const Anope::string &mname) override
	{
		Anope::string *setter;
		if (mname == "OPER" && (setter = noop.Get(u->server)))
		{
			Anope::string reason = "NOOP command used by " + *setter;
			BotInfo *OperServ = Config->GetClient("OperServ");
			u->Kill(OperServ, reason);
		}
	}
};

MODULE_INIT(OSNOOP)
