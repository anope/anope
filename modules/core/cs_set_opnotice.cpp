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

class CommandCSSetOpNotice : public Command
{
 public:
	CommandCSSetOpNotice(const Anope::string &cpermission = "") : Command("OPNOTICE", 2, 2, cpermission)
	{
		this->SetDesc(_("Send a notice when OP/DEOP commands are used"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetOpNotice");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_OPNOTICE);
			source.Reply(_("Op-notice option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_OPNOTICE);
			source.Reply(_("Op-notice option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "OPNOTICE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 OPNOTICE {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002op-notice\002 option for a channel.\n"
				"When \002op-notice\002 is set, %s will send a notice to the\n"
				"channel whenever the \002OP\002 or \002DEOP\002 commands are used for a user\n"
				"in the channel."), this->name.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET OPNOTICE", _("SET \037channel\037 OPNOTICE {ON | OFF}"));
	}
};

class CommandCSSASetOpNotice : public CommandCSSetOpNotice
{
 public:
	CommandCSSASetOpNotice() : CommandCSSetOpNotice("chanserv/saset/opnotice")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET OPNOTICE", _("SASET \002channel\002 OPNOTICE {ON | OFF}"));
	}
};

class CSSetOpNotice : public Module
{
	CommandCSSetOpNotice commandcssetopnotice;
	CommandCSSASetOpNotice commandcssasetopnotice;

 public:
	CSSetOpNotice(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetopnotice);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetopnotice);
	}

	~CSSetOpNotice()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetopnotice);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetopnotice);
	}
};

MODULE_INIT(CSSetOpNotice)
