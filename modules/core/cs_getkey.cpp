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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (!check_access(u, ci, CA_GETKEY) && !u->Account()->HasCommand("chanserv/getkey"))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		Anope::string key;
		if (!ci->c || !ci->c->GetParam(CMODE_KEY, key))
		{
			source.Reply(CHAN_GETKEY_NOKEY, chan.c_str());
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_GETKEY);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		source.Reply(CHAN_GETKEY_KEY, chan.c_str(), key.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_GETKEY);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GETKEY", CHAN_GETKEY_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_GETKEY);
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
