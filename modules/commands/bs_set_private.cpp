/* BotServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandBSSetPrivate : public Command
{
 public:
	CommandBSSetPrivate(Module *creator, const Anope::string &sname = "botserv/set/private") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned by non IRC operators"));
		this->SetSyntax(_("\037botname\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		BotInfo *bi = BotInfo::Find(params[0], true);
		const Anope::string &value = params[1];

		if (bi == NULL)
		{
			source.Reply(BOT_DOES_NOT_EXIST, params[0].c_str());
			return;
		}

		if (!source.HasCommand("botserv/set/private"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (value.equals_ci("ON"))
		{
			bi->SetFlag(BI_PRIVATE);
			source.Reply(_("Private mode of bot %s is now \002on\002."), bi->nick.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bi->UnsetFlag(BI_PRIVATE);
			source.Reply(_("Private mode of bot %s is now \002off\002."), bi->nick.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
			"This option prevents a bot from being assigned to a\n"
			"channel by users that aren't IRC operators."));
		return true;
	}
};

class BSSetPrivate : public Module
{
	CommandBSSetPrivate commandbssetprivate;

 public:
	BSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetprivate(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetPrivate)
