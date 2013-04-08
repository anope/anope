/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

static struct ModeInfo
{
	Anope::string priv;
	Anope::string name;
} modeInfo[] = {
	{ "AUTOOWNER", "OWNER" },
	{ "AUTOPROTECT", "PROTECT" },
	{ "AUTOOP", "OP" },
	{ "AUTOHALFOP", "HALFOP" },
	{ "AUTOVOICE", "VOICE" },
	{ "", "" }
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

	void OnAccessAdd(ChannelInfo *ci, CommandSource &, ChanAccess *access) anope_override
	{
		if (ci->c)
			for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = it->second->user;

				if (access->Matches(user, user->Account()))
				{
					for (int i = 0; !modeInfo[i].priv.empty(); ++i)
						if (!access->HasPriv(modeInfo[i].priv))
							ci->c->RemoveMode(NULL, modeInfo[i].name, user->nick);
					ci->c->SetCorrectModes(user, true, false);
				}
			}
	}

	void OnAccessDel(ChannelInfo *ci, CommandSource &, ChanAccess *access) anope_override
	{
		if (ci->c)
			for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = it->second->user;

				if (access->Matches(user, user->Account()))
				{
					for (int i = 0; !modeInfo[i].priv.empty(); ++i)
						if (access->HasPriv(modeInfo[i].priv))
							ci->c->RemoveMode(NULL, modeInfo[i].name, user->nick);
				}
			}
	}
};

MODULE_INIT(StatusUpdate)
