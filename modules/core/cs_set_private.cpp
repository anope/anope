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

class CommandCSSetPrivate : public Command
{
 public:
	CommandCSSetPrivate(const Anope::string &cpermission = "") : Command("PRIVATE", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetPrivate");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 PRIVATE {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002private\002 option for a channel.\n"
				"When \002private\002 is set, a \002%R%s LIST\002 will not\n"
				"include the channel in any lists."), this->name.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PRIVATE", _("SET \037channel\037 PRIVATE {ON | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    PRIVATE       Hide channel from LIST command"));
	}
};

class CommandCSSASetPrivate : public CommandCSSetPrivate
{
 public:
	CommandCSSASetPrivate() : CommandCSSetPrivate("chanserv/saset/private")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PRIVATE", _("SASET \002channel\002 PRIVATE {ON | OFF}"));
	}
};

class CSSetPrivate : public Module
{
	CommandCSSetPrivate commandcssetprivate;
	CommandCSSASetPrivate commandcssasetprivate;

 public:
	CSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetprivate);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetprivate);
	}

	~CSSetPrivate()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetprivate);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetprivate);
	}
};

MODULE_INIT(CSSetPrivate)
