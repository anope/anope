/* NickServ core functions
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

class CommandNSIdentify : public Command
{
 public:
	CommandNSIdentify() : Command("IDENTIFY", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Identify yourself with your password"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params.size() == 2 ? params[0] : u->nick;
		Anope::string pass = params[params.size() - 1];

		NickAlias *na = findnick(nick);
		if (na && na->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
		else if (na && na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(_(NICK_X_SUSPENDED), na->nick.c_str());
		else if (u->Account() && na && u->Account() == na->nc)
			source.Reply(_("You are already identified."));
		else
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(u, this, params, nick, pass));
			if (MOD_RESULT == EVENT_STOP)
				return MOD_CONT;

			if (!na)
				source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());
			else if (MOD_RESULT != EVENT_ALLOW)
			{
				Log(LOG_COMMAND, u, this) << "and failed to identify";
				source.Reply(_(PASSWORD_INCORRECT));
				if (bad_password(u))
					return MOD_STOP;
			}
			else
			{
				if (u->IsIdentified())
					Log(LOG_COMMAND, u, this) << "to log out of account " << u->Account()->display;

				Log(LOG_COMMAND, u, this) << "and identified for account " << na->nc->display;
				source.Reply(_("Password accepted - you are now recognized."));
				u->Identify(na);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002IDENTIFY [account] \037password\037\002\n"
				" \n"
				"Tells %s that you are really the owner of this\n"
				"nick.  Many commands require you to authenticate yourself\n"
				"with this command before you use them.  The password\n"
				"should be the same one you sent with the \002REGISTER\002\n"
				"command."), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "IDENTIFY", _("IDENTIFY [account] \037password\037"));
	}
};

class NSIdentify : public Module
{
	CommandNSIdentify commandnsidentify;

 public:
	NSIdentify(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsidentify);
	}
};

MODULE_INIT(NSIdentify)
