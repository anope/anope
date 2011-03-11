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

class CommandOSGlobal : public Command
{
 public:
	CommandOSGlobal() : Command("GLOBAL", 1, 1, "operserv/global")
	{
		this->SetDesc(_("Send a message to all users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &msg = params[0];

		Log(LOG_ADMIN, u, this);
		oper_global(u->nick, "%s", msg.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002GLOBAL \037message\037\002\n"
				" \n"
				"Allows Administrators to send messages to all users on the \n"
				"network. The message will be sent from the nick \002%s\002."), Config->s_GlobalNoticer.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GLOBAL", _("GLOBAL \037message\037"));
	}
};

class OSGlobal : public Module
{
	CommandOSGlobal commandosglobal;

 public:
	OSGlobal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (Config->s_GlobalNoticer.empty())
			throw ModuleException("Global is disabled");

		// Maybe we should put this ON Global?
		this->AddCommand(OperServ, &commandosglobal);
	}
};

MODULE_INIT(OSGlobal)
