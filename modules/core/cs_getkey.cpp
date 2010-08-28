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

class CommandCSGetKey : public Command
{
 public:
	CommandCSGetKey() : Command("GETKEY", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		ChannelInfo *ci;
		Anope::string key;

		ci = cs_findchan(chan);

		if (!check_access(u, ci, CA_GETKEY) && !u->Account()->HasCommand("chanserv/getkey"))
		{
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->GetParam(CMODE_KEY, key))
		{
			notice_lang(Config->s_ChanServ, u, CHAN_GETKEY_NOKEY, chan.c_str());
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_GETKEY);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		notice_lang(Config->s_ChanServ, u, CHAN_GETKEY_KEY, chan.c_str(), key.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_ChanServ, u, CHAN_HELP_GETKEY);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "GETKEY", CHAN_GETKEY_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_GETKEY);
	}
};

class CSGetKey : public Module
{
	CommandCSGetKey commandcsgetkey;

 public:
	CSGetKey(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsgetkey);
	}
};

MODULE_INIT(CSGetKey)
