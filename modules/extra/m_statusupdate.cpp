/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class StatusUpdate : public Module
{
 public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnAccessAdd, I_OnAccessChange, I_OnAccessDel, I_OnAccessClear };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnAccessAdd(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		this->OnAccessChange(ci, u, access);
	}

	void OnAccessChange(ChannelInfo *ci, User *, ChanAccess *access)
	{
		if (ci->c)
			for (CUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = (*it)->user;

				if (user == ci->bi)
					continue;

				ChanAccess *highest = ci->GetAccess(user);

				if (!access || (highest == access))
					chan_set_correct_modes(user, ci->c, 1);
			}
	}

	void OnAccessDel(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		access->level = 0;
		this->OnAccessChange(ci, u, access);
	}

	void OnAccessClear(ChannelInfo *ci, User *u)
	{
		this->OnAccessChange(ci, u, NULL);
	}
};

MODULE_INIT(StatusUpdate)
