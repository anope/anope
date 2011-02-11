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

class CommandCSSetSecure : public Command
{
 public:
	CommandCSSetSecure(const Anope::string &cpermission = "") : Command("SECURE", 2, 2, cpermission)
	{
		this->SetDesc("Activate " + ChanServ->nick + "'s security features");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSecure");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 SECURE {ON | OFF}\002\n"
			" \n"
			"Enables or disables %s's security features for a\n"
			"channel. When \002SECURE\002 is set, only users who have\n"
			"registered their nicknames with %s and IDENTIFY'd\n"
			"with their password will be given access to the channel\n"
			"as controlled by the access list."), this->name.c_str(), NickServ->nick.c_str(), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SECURE", _("SET \037channel\037 SECURE {ON | OFF}"));
	}
};

class CommandCSSASetSecure : public CommandCSSetSecure
{
 public:
	CommandCSSASetSecure() : CommandCSSetSecure("chanserv/saset/secure")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SECURE", _("SASET \002channel\002 SECURE {ON | OFF}"));
	}
};

class CSSetSecure : public Module
{
	CommandCSSetSecure commandcssetsecure;
	CommandCSSASetSecure commandcssasetsecure;

 public:
	CSSetSecure(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetsecure);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetsecure);
	}

	~CSSetSecure()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetsecure);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetsecure);
	}
};

MODULE_INIT(CSSetSecure)
