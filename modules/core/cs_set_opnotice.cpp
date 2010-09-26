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

class CommandCSSetOpNotice : public Command
{
 public:
	CommandCSSetOpNotice(const Anope::string &cpermission = "") : Command("OPNOTICE", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetOpNotice");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_OPNOTICE);
			u->SendMessage(ChanServ, CHAN_SET_OPNOTICE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_OPNOTICE);
			u->SendMessage(ChanServ, CHAN_SET_OPNOTICE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "OPNOTICE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_OPNOTICE, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SET OPNOTICE", CHAN_SET_OPNOTICE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SET_OPNOTICE);
	}
};

class CommandCSSASetOpNotice : public CommandCSSetOpNotice
{
 public:
	CommandCSSASetOpNotice() : CommandCSSetOpNotice("chanserv/saset/opnotice")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_OPNOTICE, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(ChanServ, u, "SET OPNOTICE", CHAN_SASET_OPNOTICE_SYNTAX);
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
			c->AddSubcommand(&commandcssetopnotice);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetopnotice);
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
