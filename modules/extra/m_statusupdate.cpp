/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

static struct ModeInfo
{
	ChannelAccess priv;
	ChannelModeName name;
} modeInfo[] = {
	{ CA_AUTOOWNER, CMODE_OWNER },
	{ CA_AUTOPROTECT, CMODE_PROTECT },
	{ CA_AUTOOP, CMODE_OP },
	{ CA_AUTOHALFOP, CMODE_HALFOP },
	{ CA_AUTOVOICE, CMODE_VOICE },
	{ CA_SIZE, CMODE_END }
};

class StatusUpdate : public Module
{
 public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnAccessAdd, I_OnAccessDel };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnAccessAdd(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		if (ci->c)
			for (CUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = (*it)->user;

				if (access->Matches(user, user->Account()))
				{
					for (int i = 0; modeInfo[i].priv != CA_SIZE; ++i)
						if (access->HasPriv(modeInfo[i].priv))
							ci->c->SetMode(NULL, modeInfo[i].name, user->nick);
						else
							ci->c->RemoveMode(NULL, modeInfo[i].name, user->nick);
				}
			}
	}

	void OnAccessDel(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		if (ci->c)
			for (CUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = (*it)->user;

				if (access->Matches(user, user->Account()))
				{
					for (int i = 0; modeInfo[i].priv != CA_SIZE; ++i)
						if (access->HasPriv(modeInfo[i].priv))
							ci->c->RemoveMode(NULL, modeInfo[i].name, user->nick);
				}
			}
	}
};

MODULE_INIT(StatusUpdate)
