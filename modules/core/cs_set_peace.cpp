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

class CommandCSSetPeace : public Command
{
 public:
	CommandCSSetPeace(const Anope::string &cpermission = "") : Command("PEACE", 2, 2, cpermission)
	{
		this->SetDesc(_("Regulate the use of critical commands"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetPeace");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PEACE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 PEACE {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002peace\002 option for a channel.\n"
				"When \002peace\002 is set, a user won't be able to kick,\n"
				"ban or remove a channel status of a user that has\n"
				"a level superior or equal to his via %s commands."), this->name.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PEACE", _("SET \037channel\037 PEACE {ON | OFF}"));
	}
};

class CommandCSSASetPeace : public CommandCSSetPeace
{
 public:
	CommandCSSASetPeace() : CommandCSSetPeace("chanserv/saset/peace")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PEACE", _("SASET \002channel\002 PEACE {ON | OFF}"));
	}
};

class CSSetPeace : public Module
{
	CommandCSSetPeace commandcssetpeace;
	CommandCSSASetPeace commandcssasetpeace;

 public:
	CSSetPeace(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetpeace);
	}

	~CSSetPeace()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetpeace);
	}
};

MODULE_INIT(CSSetPeace)
