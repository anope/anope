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
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSecure");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECURE);
			source.Reply(CHAN_SET_SECURE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECURE);
			source.Reply(CHAN_SET_SECURE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_SECURE, "SET", Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET SECURE", CHAN_SET_SECURE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_SECURE);
	}
};

class CommandCSSASetSecure : public CommandCSSetSecure
{
 public:
	CommandCSSASetSecure() : CommandCSSetSecure("chanserv/saset/secure")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_SECURE, "SASET", Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET SECURE", CHAN_SASET_SECURE_SYNTAX);
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
