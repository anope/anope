/* ns_maxemail.c - Limit the amount of times an email address
 *                 can be used for a NickServ account.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send any bug reports to the Anope Coder, as he will be able
 * to deal with it best.
 */

#include "module.h"

class NSMaxEmail : public Module
{
	int NSEmailMax;

	bool CheckLimitReached(const Anope::string &email, User *u)
	{
		if (this->NSEmailMax < 1 || email.empty())
			return false;

		if (this->CountEmail(email, u) < this->NSEmailMax)
			return false;

		if (this->NSEmailMax == 1)
			this->SendMessage(NickServ, u, _("The given email address has reached it's usage limit of 1 user."));
		else
			this->SendMessage(NickServ, u, _("The given email address has reached it's usage limit of %d users."), this->NSEmailMax);

		return true;
	}

	int CountEmail(const Anope::string &email, User *u)
	{
		int count = 0;

		if (email.empty())
			return 0;

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (!(u->Account() && u->Account() == nc) && !nc->email.empty() && nc->email.equals_ci(email))
				++count;
		}

		return count;
	}

 public:
	NSMaxEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);

		ModuleManager::Attach(I_OnReload, this);
		ModuleManager::Attach(I_OnPreCommand, this);

		OnReload(false);
	}

	void OnReload(bool)
	{
		ConfigReader config;
		this->NSEmailMax = config.ReadInteger("ns_maxemail", "maxemails", "0", 0, false);
		Log(LOG_DEBUG) << "[ns_maxemail] NSEmailMax set to " << NSEmailMax;
	}

	EventReturn OnPreCommand(User *u, BotInfo *service, const Anope::string &command, const std::vector<Anope::string> &params)
	{
		if (service == findbot(Config->s_NickServ))
		{
			if (command.equals_ci("REGISTER"))
			{
				if (CheckLimitReached(params.size() > 1 ? params[1] : "", u))
					return EVENT_STOP;
			}
			else if (command.equals_ci("SET"))
			{
				Anope::string set = params[0];
				Anope::string email = params.size() > 1 ? params[1] : "";

				if (set.equals_ci("email") && CheckLimitReached(email, u))
					return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSMaxEmail)
