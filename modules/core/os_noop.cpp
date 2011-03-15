/* OperServ core functions
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

class CommandOSNOOP : public Command
{
 public:
	CommandOSNOOP() : Command("NOOP", 2, 2, "operserv/noop")
	{
		this->SetDesc(_("Temporarily remove all O:lines of a server remotely"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &cmd = params[0];
		const Anope::string &server = params[1];

		if (cmd.equals_ci("SET"))
		{
			Anope::string reason;

			/* Remove the O:lines */
			ircdproto->SendSVSNOOP(server, 1);

			reason = "NOOP command used by " + u->nick;
			Log(LOG_ADMIN, u, this) << "on " << server;
			source.Reply(_("All O:lines of \002%s\002 have been removed."), server.c_str());

			/* Kill all the IRCops of the server */
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end();)
			{
				User *u2 = it->second;
				++it;

				if (u2 && u2->HasMode(UMODE_OPER) && Anope::Match(u2->server->GetName(), server, true))
					kill_user(Config->s_OperServ, u2, reason);
			}
		}
		else if (cmd.equals_ci("REVOKE"))
		{
			ircdproto->SendSVSNOOP(server, 0);
			source.Reply(_("All O:lines of \002%s\002 have been reset."), server.c_str());
		}
		else
			this->OnSyntaxError(source, "");
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002NOOP SET \037server\037\002\n"
				"          \002NOOP REVOKE \037server\037\002\n"
				"\n"
				"\002NOOP SET\002 remove all O:lines of the given\n"
				"\002server\002 and kill all IRCops currently on it to\n"
				"prevent them from rehashing the server (because this\n"
				"would just cancel the effect).\n"
				"\002NOOP REVOKE\002 makes all removed O:lines available again\n"
				"on the given \002server\002.\n"
				"\002Note:\002 The \002server\002 is not checked at all by the\n"
				"Services."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "NOOP", _("NOOP {SET|REVOKE} \037server\037"));
	}
};

class OSNOOP : public Module
{
	CommandOSNOOP commandosnoop;

 public:
	OSNOOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosnoop);
	}
};

MODULE_INIT(OSNOOP)
