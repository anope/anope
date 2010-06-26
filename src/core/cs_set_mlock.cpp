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

class CommandCSSetMLock : public Command
{
 public:
	CommandCSSetMLock(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 1, 0, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		int add = -1; /* 1 if adding, 0 if deleting, -1 if neither */
		unsigned char mode;
		ChannelMode *cm;
		unsigned paramcount = 2;

		ci->ClearMLock();
		ci->ClearParams();

		if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
			ci->SetMLock(CMODE_REGISTERED, true);

		const char *modes = params[1].c_str();
		while (modes && (mode = *modes++))
		{
			switch (mode)
			{
				case '+':
					add = 1;
					continue;
				case '-':
					add = 0;
					continue;
				default:
					if (add < 0)
						continue;
			}

			if ((cm = ModeManager::FindChannelModeByChar(mode)))
			{
				if (cm->Type == MODE_STATUS || cm->Type == MODE_LIST || !cm->CanSet(u))
					notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_IMPOSSIBLE_CHAR, mode);
				else if (add)
				{
					ci->RemoveMLock(cm->Name);

					if (cm->Type == MODE_PARAM)
					{
						if (paramcount >= params.size())
							continue;

						std::string param = params[paramcount].c_str();

						ChannelModeParam *cmp = dynamic_cast<ChannelModeParam *>(cm);

						if (!cmp || !cmp->IsValid(param))
							continue;

						ci->SetMLock(cmp->Name, true, param);
					}
					else
						ci->SetMLock(cm->Name, true);
				}
				else
					ci->SetMLock(cm->Name, false);
			}
			else
				notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_UNKNOWN_CHAR, mode);
		} /* while (*modes) */

		/* We can't mlock +L if +l is not mlocked as well. */
		if (ModeManager::FindChannelModeByName(CMODE_REDIRECT) && ci->HasMLock(CMODE_REDIRECT, true) && !ci->HasMLock(CMODE_LIMIT, true))
		{
			ci->RemoveMLock(CMODE_REDIRECT);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_L_REQUIRED);
		}

		/* Some ircd we can't set NOKNOCK without INVITE */
		/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
		if (ModeManager::FindChannelModeByName(CMODE_NOKNOCK) && ircd->knock_needs_i && ci->HasMLock(CMODE_NOKNOCK, true) && !ci->HasMLock(CMODE_INVITE, true))
		{
			ci->RemoveMLock(CMODE_NOKNOCK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_K_REQUIRED);
		}

		/* Since we always enforce mode r there is no way to have no
		 * mode lock at all.
		 */
		if (get_mlock_modes(ci, 0))
			notice_lang(Config.s_ChanServ, u, CHAN_MLOCK_CHANGED, ci->name.c_str(), get_mlock_modes(ci, 0));

		/* Implement the new lock. */
		if (ci->c)
			check_modes(ci->c);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_MLOCK, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_MLOCK);
	}
};

class CommandCSSASetMLock : public CommandCSSetMLock
{
 public:
	CommandCSSASetMLock(const ci::string &cname) : CommandCSSetMLock(cname, "chanserv/saset/mlock")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_MLOCK, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetMLock : public Module
{
 public:
	CSSetMLock(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetMLock("MLOCK"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetMLock("MLOCK"));
	}

	~CSSetMLock()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("MLOCK");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("MLOCK");
	}
};

MODULE_INIT(CSSetMLock)
