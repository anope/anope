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
	CommandCSSetMLock(const Anope::string &cpermission = "") : Command("MLOCK", 1, 0, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetMLock");

		int add = -1; /* 1 if adding, 0 if deleting, -1 if neither */
		ChannelMode *cm;
		unsigned paramcount = 2;

		ci->ClearMLock();
		ci->ClearParams();

		if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
			ci->SetMLock(CMODE_REGISTERED, true);

		Anope::string modes = params.size() > 1 ? params[1] : "";
		for (Anope::string::const_iterator ch = modes.begin(), end = modes.end(); ch != end; ++ch)
		{
			switch (*ch)
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

			if ((cm = ModeManager::FindChannelModeByChar(*ch)))
			{
				if (cm->Type == MODE_STATUS || cm->Type == MODE_LIST || !cm->CanSet(u))
					u->SendMessage(ChanServ, CHAN_SET_MLOCK_IMPOSSIBLE_CHAR, *ch);
				else if (add)
				{
					ci->RemoveMLock(cm->Name);

					if (cm->Type == MODE_PARAM)
					{
						if (paramcount >= params.size())
							continue;

						Anope::string param = params[paramcount++];

						ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);

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
				u->SendMessage(ChanServ, CHAN_SET_MLOCK_UNKNOWN_CHAR, *ch);
		}

		/* We can't mlock +L if +l is not mlocked as well. */
		if (ModeManager::FindChannelModeByName(CMODE_REDIRECT) && ci->HasMLock(CMODE_REDIRECT, true) && !ci->HasMLock(CMODE_LIMIT, true))
		{
			ci->RemoveMLock(CMODE_REDIRECT);
			u->SendMessage(ChanServ, CHAN_SET_MLOCK_L_REQUIRED);
		}

		/* Some ircd we can't set NOKNOCK without INVITE */
		/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
		if (ModeManager::FindChannelModeByName(CMODE_NOKNOCK) && ircd->knock_needs_i && ci->HasMLock(CMODE_NOKNOCK, true) && !ci->HasMLock(CMODE_INVITE, true))
		{
			ci->RemoveMLock(CMODE_NOKNOCK);
			u->SendMessage(ChanServ, CHAN_SET_MLOCK_K_REQUIRED);
		}

		/* Since we always enforce mode r there is no way to have no
		 * mode lock at all.
		 */
		if (!get_mlock_modes(ci, 0).empty())
			u->SendMessage(ChanServ, CHAN_MLOCK_CHANGED, ci->name.c_str(), get_mlock_modes(ci, 0).c_str());

		/* Implement the new lock. */
		if (ci->c)
			check_modes(ci->c);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_MLOCK, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		SyntaxError(ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_SET_MLOCK);
	}
};

class CommandCSSASetMLock : public CommandCSSetMLock
{
 public:
	CommandCSSASetMLock() : CommandCSSetMLock("chanserv/saset/mlock")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(ChanServ, CHAN_HELP_SET_MLOCK, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		SyntaxError(ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetMLock : public Module
{
	CommandCSSetMLock commandcssetmlock;
	CommandCSSASetMLock commandcssasetmlock;

 public:
	CSSetMLock(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(&commandcssetmlock);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(&commandcssasetmlock);
	}

	~CSSetMLock()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetmlock);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetmlock);
	}
};

MODULE_INIT(CSSetMLock)
