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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string what = params[1];
		Channel *c = findchan(chan);
		ChannelInfo *ci = c ? c->ci : NULL;
		Anope::string modebuf;

		ChannelMode *owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
		ChannelMode *admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);
		ChannelMode *op = ModeManager::FindChannelModeByName(CMODE_OP);
		ChannelMode *halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);
		ChannelMode *voice = ModeManager::FindChannelModeByName(CMODE_VOICE);

		if (!c)
			u->SendMessage(ChanServ, CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!check_access(u, ci, CA_CLEAR))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		
		Anope::string buf = "CLEAR USERS command from " + u->nick + " (" + u->Account()->display + ")";

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			c->Kick(NULL, uc->user, "%s", buf.c_str());
		}

		u->SendMessage(ChanServ, CHAN_CLEARED_USERS, chan.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CLEAR);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_CLEAR);
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
