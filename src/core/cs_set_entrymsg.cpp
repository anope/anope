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
	CommandCSSetEntryMsg(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 1, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (ci->entry_message)
			delete [] ci->entry_message;
		if (params.size() > 1)
		{
			ci->entry_message = sstrdup(params[1].c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name.c_str(), ci->entry_message);
		}
		else
		{
			ci->entry_message = NULL;
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG, "SEt");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
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
	CommandCSSASetEntryMsg(const ci::string &cname) : CommandCSSetEntryMsg(cname, "/chanserv/saset/entrymsg")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetEntryMsg : public Module
{
 public:
	CSSetEntryMsg(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetEntryMsg("ENTRYMSG"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetEntryMsg("ENTRYMSG"));
	}

	~CSSetEntryMsg()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("ENTRYMSG");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("ENTRYMSG");
	}
};

MODULE_INIT(CSSetEntryMsg)
