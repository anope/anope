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
		ChannelInfo *ci = c ? c->ci : NULL;
		std::string modebuf;

		ChannelMode *owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
		ChannelMode *admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);
		ChannelMode *op = ModeManager::FindChannelModeByName(CMODE_OP);
		ChannelMode *halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);
		ChannelMode *voice = ModeManager::FindChannelModeByName(CMODE_VOICE);

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
				if (op)
				{
					modebuf = "-";
					modebuf += op->ModeChar;

					ircdproto->SendSVSModeChan(c, modebuf.c_str(), NULL);
				}
			}
			else
			{
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				{
					UserContainer *uc = *it;

					if (uc->Status->HasFlag(CMODE_OWNER))
						c->RemoveMode(NULL, owner, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_PROTECT))
						c->RemoveMode(NULL, admin, uc->user->nick);
					if (uc->Status->HasFlag(CMODE_OP))
						c->RemoveMode(NULL, op, uc->user->nick);
				}
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_OPS, chan);
		}
		else if ((halfop && what == "hops") || (voice && what == "voices"))
		{
			ChannelMode *cm = what == "hops" ? halfop : voice;

			if (ircd->svsmode_ucmode)
			{
				modebuf = "-";
				modebuf += cm->ModeChar;

				ircdproto->SendSVSModeChan(c, modebuf.c_str(), NULL);
			}
			else
			{
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				{
					UserContainer *uc = *it;

					if (uc->Status->HasFlag(cm->Name))
						c->RemoveMode(NULL, cm, uc->user->nick);
				}
			}
		}
		else if (what == "users")
		{
			std::string buf = "CLEAR USERS command from " + u->nick;

			for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
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

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_CLEAR);
	}
};

class CSClear : public Module
{
 public:
	CSClear(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->AddCommand(ChanServ, new CommandCSClear());
	}
};

MODULE_INIT(CSClear)
