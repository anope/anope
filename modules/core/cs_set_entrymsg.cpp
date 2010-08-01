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

class CommandCSSetEntryMsg : public Command
{
 public:
	CommandCSSetEntryMsg(const Anope::string &cpermission = "") : Command("ENTRYMSG", 1, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetEntryMsg");

		if (params.size() > 1)
		{
			ci->entry_message = params[1];
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name.c_str(), ci->entry_message.c_str());
		}
		else
		{
			ci->entry_message.clear();
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_ENTRYMSG);
	}
};

class CommandCSSASetEntryMsg : public CommandCSSetEntryMsg
{
 public:
	CommandCSSASetEntryMsg() : CommandCSSetEntryMsg("chanserv/saset/entrymsg")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetEntryMsg : public Module
{
	CommandCSSetEntryMsg commandcssetentrymsg;
	CommandCSSASetEntryMsg commandcssasetentrymsg;

 public:
	CSSetEntryMsg(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(&commandcssetentrymsg);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetentrymsg);
	}

	~CSSetEntryMsg()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetentrymsg);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetentrymsg);
	}
};

MODULE_INIT(CSSetEntryMsg)
