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

class CommandCSSetSecureFounder : public Command
{
 public:
	CommandCSSetSecureFounder(const Anope::string &cpermission = "") : Command("SECUREFOUNDER", 2, 2, cpermission)
	{
		this->SetDesc("Stricter control of channel founder status");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSecureFounder");

		if (this->permission.empty() && ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREFOUNDER");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 SECUREFOUNDER {ON | OFF}\002\n"
			" \n"
			"Enables or disables the \002secure founder\002 option for a channel.\n"
			"When \002secure founder\002 is set, only the real founder will be\n"
			"able to drop the channel, change its password, its founder and its\n"
			"successor, and not those who have founder level access through\n"
			"the access/qop command."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SECUREFOUNDER", _("SET \037channel\037 SECUREFOUNDER {ON | OFF}"));
	}
};

class CommandCSSASetSecureFounder : public CommandCSSetSecureFounder
{
 public:
	CommandCSSASetSecureFounder() : CommandCSSetSecureFounder("chanserv/saset/securefounder")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SECUREFOUNDER", _("SASET \002channel\002 SECUREFOUNDER {ON | OFF}"));
	}
};

class CSSetSecureFounder : public Module
{
	CommandCSSetSecureFounder commandcssetsecurefounder;
	CommandCSSASetSecureFounder commandcssasetsecurefounder;

 public:
	CSSetSecureFounder(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetsecurefounder);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetsecurefounder);
	}

	~CSSetSecureFounder()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetsecurefounder);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetsecurefounder);
	}
};

MODULE_INIT(CSSetSecureFounder)
