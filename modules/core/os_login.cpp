/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"
#include "operserv.h"

class CommandOSLogin : public Command
{
 public:
	CommandOSLogin() : Command("LOGIN", 1, 1)
	{
		this->SetDesc(Anope::printf(_("Login to %s"), Config->s_OperServ.c_str()));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &password = params[0];

		Oper *o = source.u->Account()->o;
		if (o == NULL)
			source.Reply(_("No oper block for your nick."));
		else if (o->password.empty())
			source.Reply(_("Your oper block doesn't require logging in."));
		else if (source.u->GetExt("os_login_password_correct"))
			source.Reply(_("You are already identified."));
		else if (o->password != password)
		{
			source.Reply(_(PASSWORD_INCORRECT));
			bad_password(source.u);
		}
		else
		{
			Log(LOG_ADMIN, source.u, this) << "and succesfully identified to " << Config->s_OperServ;
			source.u->Extend("os_login_password_correct");
			source.Reply(_("Password accepted."));
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002LOGIN\002 \037password\037\n"
				" \n"
				"Logs you in to %s so you gain Services Operator privileges.\n"
				"This command may be unnecessary if your oper block is\n"
				"configured without a password."), Config->s_OperServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LOGIN", _("LOGIN \037password\037"));
	}
};

class OSLogin : public Module
{
	CommandOSLogin commandoslogin;

 public:
	OSLogin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandoslogin);

		ModuleManager::Attach(I_IsServicesOper, this);
	}

	~OSLogin()
	{
		for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			it->second->Shrink("os_login_password_correct");
	}

	EventReturn IsServicesOper(User *u)
	{
		if (!u->Account()->o->password.empty())
		{
			if (u->GetExt("os_login_password_correct"))
				return EVENT_ALLOW;
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSLogin)
