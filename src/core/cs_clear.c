/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSClear : public Command
{
 public:
	CommandCSClear() : Command("CLEAR", 2, 2)
	{

	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string what = params[1];
		Channel *c = findchan(chan);
		ChannelInfo *ci;
		ChannelMode *owner, *admin;
		std::string modebuf;

		owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
		admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (c)
			ci = c->ci;

		if (!c)
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		else if (!u || !check_access(u, ci, CA_CLEAR))
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else if (what == "bans")
		{
			c->ClearBans();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_BANS, chan);
		}
		else if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && what == "excepts")
		{
			c->ClearExcepts();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_EXCEPTS, chan);

		}
		else if (ModeManager::FindChannelModeByName(CMODE_INVITE) && what == "invites")
		{
			c->ClearInvites();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_INVITES, chan);

		}
		else if (what == "modes")
		{
			c->ClearModes();
			check_modes(c);

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_MODES, chan);
		}
		else if (what == "ops")
		{
			if (ircd->svsmode_ucmode)
			{
				ircdproto->SendSVSModeChan(c, "-o", NULL);
				if (owner)
				{
					modebuf = '-';
					modebuf += owner->ModeChar;

					ircdproto->SendSVSModeChan(c, modebuf.c_str(), NULL);
				}
				if (admin)
				{
					modebuf = '-';
					modebuf += admin->ModeChar;

					ircdproto->SendSVSModeChan(c, modebuf.c_str(), NULL);
				}
				for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
				{
					UserContainer *uc = *it;

					if (uc->Status->HasFlag(CMODE_OWNER))
						c->RemoveMode(NULL, CMODE_OWNER, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_PROTECT))
						c->RemoveMode(NULL, CMODE_PROTECT, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_OP))
						c->RemoveMode(NULL, CMODE_OP, uc->user->nick);
				}
			}
			else
			{
				for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
				{
					UserContainer *uc = *it;

					if (uc->Status->HasFlag(CMODE_OWNER))
						c->RemoveMode(NULL, CMODE_OWNER, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_PROTECT))
						c->RemoveMode(NULL, CMODE_PROTECT, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_OP))
						c->RemoveMode(NULL, CMODE_OP, uc->user->nick);
				}
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_OPS, chan);
		}
		else if (ModeManager::FindChannelModeByName(CMODE_HALFOP) && what == "hops")
		{
			for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
			{
				UserContainer *uc = *it;

				if (uc->Status->HasFlag(CMODE_HALFOP))
					c->RemoveMode(NULL, CMODE_HALFOP, uc->user->nick);
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_HOPS, chan);
		}
		else if (what == "voices")
		{
			for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
			{
				UserContainer *uc = *it;

				if (uc->Status->HasFlag(CMODE_VOICE))
					c->RemoveMode(NULL, CMODE_VOICE, uc->user->nick);
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_VOICES, chan);
		}
		else if (what == "users") {
			std::string buf = "CLEAR USERS command from " + u->nick;

			for (CUserList::iterator it = c->users.begin(); it != c->users.end();)
			{
				UserContainer *uc = *it++;

				c->Kick(NULL, uc->user, buf.c_str());
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_USERS, chan);
		}
		else
			syntax_error(Config.s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_CLEAR);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
	}
};

class CSClear : public Module
{
 public:
	CSClear(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSClear());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_CLEAR);
	}
};

MODULE_INIT(CSClear)
