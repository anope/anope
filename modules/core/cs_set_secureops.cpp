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

class CommandCSSetSecureOps : public Command
{
 public:
	CommandCSSetSecureOps(const Anope::string &cpermission = "") : Command("SECUREOPS", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSecureIos");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET \037%s\037 SECUREOPS {ON | OFF}\002\n"
				" \n"
				"Enables or disables the \002secure ops\002 option for a channel.\n"
				"When \002secure ops\002 is set, users who are not on the userlist\n"
				"will not be allowed chanop status."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SECUREOPS", _("SET \037channel\037 SECUREOPS {ON | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SECUREOPS     Stricter control of chanop status"));
	}
};

class CommandCSSASetSecureOps : public CommandCSSetSecureOps
{
 public:
	CommandCSSASetSecureOps() : CommandCSSetSecureOps("chanserv/saset/secureops")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SECUREOPS", _("SASET \002channel\002 SECUREOPS {ON | OFF}"));
	}
};

class CSSetSecureOps : public Module
{
	CommandCSSetSecureOps commandcssetsecureops;
	CommandCSSASetSecureOps commandcssasetsecureops;

 public:
	CSSetSecureOps(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetsecureops);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetsecureops);
	}

	~CSSetSecureOps()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetsecureops);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetsecureops);
	}
};

MODULE_INIT(CSSetSecureOps)
