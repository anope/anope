/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSClearModes : public Command
{
 public:
	CommandOSClearModes() : Command("CLEARMODES", 1, 2, "operserv/clearmodes")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		Channel *c;
		int all = 0;
		ChannelMode *cm;
		std::string buf;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config.s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(Config.s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		else
		{
			ci::string s = params.size() > 1 ? params[1] : "";
			if (!s.empty()) {
				if (s == "ALL")
					all = 1;
				else {
					this->OnSyntaxError(u, "");
					return MOD_CONT;
				}
			}

			if (Config.WallOSClearmodes)
				ircdproto->SendGlobops(OperServ, "%s used CLEARMODES%s on %s", u->nick.c_str(), all ? " ALL" : "", chan);
			if (all)
			{
				/* Clear mode +o */
				if (ircd->svsmode_ucmode)
					ircdproto->SendSVSModeChan(c, "-o", NULL);
				else
				{
					for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
					{
						UserContainer *uc = *it;

						if (uc->Status->HasFlag(CMODE_OP))
							c->RemoveMode(NULL, CMODE_OP, uc->user->nick);
					}
				}

				/* Clear mode +v */
				if (ircd->svsmode_ucmode)
					ircdproto->SendSVSModeChan(c, "-v", NULL);
				else
				{
					for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
					{
						UserContainer *uc = *it;

						if (uc->Status->HasFlag(CMODE_VOICE))	
							c->RemoveMode(NULL, CMODE_VOICE, uc->user->nick);
					}
				}

				/* Clear mode +h */
				if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
				{
					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c, "-h", NULL);
					else
					{
						for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
						{
							UserContainer *uc = *it;

							if (uc->Status->HasFlag(CMODE_HALFOP))
								c->RemoveMode(NULL, CMODE_HALFOP, uc->user->nick);
						}
					}
				}

				/* Clear mode Owners */
				if ((cm = ModeManager::FindChannelModeByName(CMODE_OWNER)))
				{
					buf = '-';
					buf += cm->ModeChar;

					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c, buf.c_str(), NULL);
					else
					{
						for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
						{
							UserContainer *uc = *it;

							if (uc->Status->HasFlag(CMODE_OWNER))
								c->RemoveMode(NULL, CMODE_OWNER, uc->user->nick);
						}
					}
				}

				/* Clear mode protected or admins */
				if ((cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
				{
					buf = '-';
					buf += cm->ModeChar;

					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c, buf.c_str(), NULL);
					else
					{
						for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
						{
							UserContainer *uc = *it;

							if (uc->Status->HasFlag(CMODE_PROTECT))
								c->RemoveMode(NULL, CMODE_PROTECT, uc->user->nick);
						}
					}
				}
			}


			c->ClearModes();
			c->ClearBans();
			c->ClearExcepts();
			c->ClearInvites();
		}

		if (all)
			notice_lang(Config.s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
		else
			notice_lang(Config.s_OperServ, u, OPER_CLEARMODES_DONE, chan);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_CLEARMODES);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_CLEARMODES);
	}
};

class OSClearModes : public Module
{
 public:
	OSClearModes(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSClearModes());
	}
};

MODULE_INIT(OSClearModes)
