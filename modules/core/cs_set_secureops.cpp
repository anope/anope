/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSecureIos");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREOPS);
			source.Reply(CHAN_SET_SECUREOPS_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREOPS);
			source.Reply(CHAN_SET_SECUREOPS_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECUREOPS");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_SECUREOPS, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SET SECUREOPS", CHAN_SET_SECUREOPS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SET_SECUREOPS);
	}
};

class CommandCSSASetSecureOps : public CommandCSSetSecureOps
{
 public:
	CommandCSSASetSecureOps() : CommandCSSetSecureOps("chanserv/saset/secureops")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_SECUREOPS, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SASET SECUREOPS", CHAN_SASET_SECUREOPS_SYNTAX);
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
			c->AddSubcommand(&commandcssetsecureops);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetsecureops);
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
