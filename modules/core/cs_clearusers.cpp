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

class CommandCSClearUsers : public Command
{
 public:
	CommandCSClearUsers() : Command("CLEARUSERS", 1, 1)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		Anope::string modebuf;

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!check_access(u, ci, CA_FOUNDER))
			source.Reply(ACCESS_DENIED);
		
		Anope::string buf = "CLEARUSERS command from " + u->nick + " (" + u->Account()->display + ")";

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			c->Kick(NULL, uc->user, "%s", buf.c_str());
		}

		source.Reply(CHAN_CLEARED_USERS, chan.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_CLEARUSERS);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CLEAR", CHAN_CLEARUSERS_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_CLEARUSERS);
	}
};

class CSClearUsers : public Module
{
	CommandCSClearUsers commandcsclearusers;

 public:
	CSClearUsers(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsclearusers);
	}
};

MODULE_INIT(CSClearUsers)
