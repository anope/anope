/* Global core functions
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
#include "modules/global.h"

class CommandGLGlobal : public Command
{
 public:
	CommandGLGlobal(Module *creator) : Command(creator, "global/global", 1, 1)
	{
		this->SetDesc(_("Send a message to all users"));
		this->SetSyntax(_("\037message\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &msg = params[0];

		if (!Global::service)
		{
			source.Reply("No global reference, is global loaded?");
			return;
		}

		Log(LOG_ADMIN, source, this);
		Global::service->SendGlobal(NULL, source.GetNick(), msg);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Administrators to send messages to all users on the\n"
				"network. The message will be sent from the nick \002%s\002."), source.service->nick.c_str());
		return true;
	}
};

class GLGlobal : public Module
{
	CommandGLGlobal commandglglobal;

 public:
	GLGlobal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandglglobal(this)
	{

	}
};

MODULE_INIT(GLGlobal)
