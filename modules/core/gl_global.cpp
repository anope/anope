/* Global core functions
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
#include "global.h"

class CommandGLGlobal : public Command
{
 public:
	CommandGLGlobal() : Command("GLOBAL", 1, 1, "global/global")
	{
		this->SetDesc(_("Send a message to all users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &msg = params[0];

		Log(LOG_ADMIN, u, this);
		global->SendGlobal(global->Bot(), u->nick, msg);
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002GLOBAL \037message\037\002\n"
				" \n"
				"Allows Administrators to send messages to all users on the \n"
				"network. The message will be sent from the nick \002%s\002."), Config->s_Global.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GLOBAL", _("GLOBAL \037message\037"));
	}
};

class GLGlobal : public Module
{
	CommandGLGlobal commandglglobal;

 public:
	GLGlobal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (Config->s_Global.empty())
			throw ModuleException("Global is disabled");

		this->AddCommand(global->Bot(), &commandglglobal);
	}
};

MODULE_INIT(GLGlobal)
