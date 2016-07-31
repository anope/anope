/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class NSMaxEmail : public Module
	, public EventHook<Event::PreCommand>
{
	bool CheckLimitReached(CommandSource &source, const Anope::string &email)
	{
		int NSEmailMax = Config->GetModule(this)->Get<int>("maxemails");

		if (NSEmailMax < 1 || email.empty())
			return false;

		if (this->CountEmail(email, source.nc) < NSEmailMax)
			return false;

		if (NSEmailMax == 1)
			source.Reply(_("The email address \002{0}\002 has reached its usage limit of \0021\002 user."), email);
		else
			source.Reply(_("The email address \002{0}\002 has reached its usage limit of \002{1}\002 users."), email, NSEmailMax);

		return true;
	}

	int CountEmail(const Anope::string &email, NickServ::Account *unc)
	{
		int count = 0;

		if (email.empty())
			return 0;

		for (NickServ::Account *nc : NickServ::service->GetAccountList())
			if (unc != nc && !nc->GetEmail().empty() && nc->GetEmail().equals_ci(email))
				++count;

		return count;
	}

 public:
	NSMaxEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::PreCommand>(this)
	{
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		if (source.IsOper())
			return EVENT_CONTINUE;

		if (command->GetName() == "nickserv/register")
		{
			if (this->CheckLimitReached(source, params.size() > 1 ? params[1] : ""))
				return EVENT_STOP;
		}
		else if (command->GetName() == "nickserv/set/email")
		{
			if (this->CheckLimitReached(source, params.size() > 0 ? params[0] : ""))
				return EVENT_STOP;
		}
		else if (command->GetName() == "nickserv/ungroup" && source.GetAccount())
		{
			if (this->CheckLimitReached(source, source.GetAccount()->GetEmail()))
				return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSMaxEmail)
