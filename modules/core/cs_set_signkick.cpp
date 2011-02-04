/* ChanServ core functions
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

class CommandCSSetSignKick : public Command
{
 public:
	CommandCSSetSignKick(const Anope::string &cpermission = "") : Command("SIGNKICK", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSignKick");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("LEVEL"))
		{
			ci->SetFlag(CI_SIGNKICK_LEVEL);
			ci->UnsetFlag(CI_SIGNKICK);
			source.Reply(_("Signed kick option for %s is now \002ON\002, but depends of the\n"
				"level of the user that is using the command."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SIGNKICK");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET \037%s\037 SIGNKICK {ON | LEVEL | OFF}\002\n"
				" \n"
				"Enables or disables signed kicks for a\n"
				"channel.  When \002SIGNKICK\002 is set, kicks issued with\n"
				"%S KICK command will have the nick that used the\n"
				"command in their reason.\n"
				" \n"
				"If you use \002LEVEL\002, those who have a level that is superior \n"
				"or equal to the SIGNKICK level on the channel won't have their \n"
				"kicks signed. See \002%R%S HELP LEVELS\002 for more information."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SIGNKICK", _("SET \037channel\037 SIGNKICK {ON | LEVEL | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SIGNKICK      Sign kicks that are done with KICK command"));
	}
};

class CommandCSSASetSignKick : public CommandCSSetSignKick
{
 public:
	CommandCSSASetSignKick() : CommandCSSetSignKick("chanserv/saset/signkick")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SIGNKICK", _("SASET \002channel\002 SIGNKICK {ON | OFF}"));
	}
};

class CSSetSignKick : public Module
{
	CommandCSSetSignKick commandcssetsignkick;
	CommandCSSASetSignKick commandcssasetsignkick;

 public:
	CSSetSignKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetsignkick);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetsignkick);
	}

	~CSSetSignKick()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetsignkick);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetsignkick);
	}
};

MODULE_INIT(CSSetSignKick)
