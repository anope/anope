/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "os_ignore.h"

class OSIgnoreService : public IgnoreService
{
 public:
	OSIgnoreService(Module *o, const Anope::string &n) : IgnoreService(o, n) { }

	void AddIgnore(const Anope::string &mask, const Anope::string &creator, const Anope::string &reason, time_t delta = Anope::CurTime)
	{
		/* If it s an existing user, we ignore the hostmask. */
		Anope::string realmask = mask;
		size_t user, host;

		User *u = finduser(mask);
		if (u)
			realmask = "*!*@" + u->host;
		/* Determine whether we get a nick or a mask. */
		else if ((host = mask.find('@')) != Anope::string::npos)
		{
			/* Check whether we have a nick too.. */
			if ((user = mask.find('!')) != Anope::string::npos)
			{
				/* this should never happen */
				if (user > host)
					return;
			}
			else
				/* We have user@host. Add nick wildcard. */
				realmask = "*!" + mask;
		}
		/* We only got a nick.. */
		else
			realmask = mask + "!*@*";

		/* Check if we already got an identical entry. */
		IgnoreData *ign = this->Find(realmask);
		if (ign != NULL)
		{
			if (!delta)
				ign->time = 0;
			else
				ign->time = Anope::CurTime + delta;
		}
		/* Create new entry.. */
		else
		{
			IgnoreData newign;
			newign.mask = realmask;
			newign.creator = creator;
			newign.reason = reason;
			newign.time = delta ? Anope::CurTime + delta : 0;
			this->ignores.push_back(newign);
		}
	}

	bool DelIgnore(const Anope::string &mask)
	{
		for (std::list<IgnoreData>::iterator it = this->ignores.begin(), it_end = this->ignores.end(); it != it_end; ++it)
		{
			IgnoreData &idn = *it;
			if (idn.mask.equals_ci(mask))
			{
				this->ignores.erase(it);
				return true;
			}
		}

		return false;
	}

	IgnoreData *Find(const Anope::string &mask)
	{
		User *u = finduser(mask);
		std::list<IgnoreData>::iterator ign = this->ignores.begin(), ign_end = this->ignores.end();
		
		if (u)
		{
			for (; ign != ign_end; ++ign)
			{
				Entry ignore_mask(CMODE_BEGIN, ign->mask);
				if (ignore_mask.Matches(u))
					break;
			}
		}
		else
		{
			size_t user, host;
			Anope::string tmp;
			/* We didn't get a user.. generate a valid mask. */
			if ((host = mask.find('@')) != Anope::string::npos)
			{
				if ((user = mask.find('!')) != Anope::string::npos)
				{
					/* this should never happen */
					if (user > host)
						return NULL;
					tmp = mask;
				}
				else
					/* We have user@host. Add nick wildcard. */
				tmp = "*!" + mask;
			}
			/* We only got a nick.. */
			else
				tmp = mask + "!*@*";

			for (; ign != ign_end; ++ign)
				if (Anope::Match(tmp, ign->mask))
					break;
		}

		/* Check whether the entry has timed out */
		if (ign != ign_end)// && (*ign)->time && (*ign)->time <= Anope::CurTime)
		{
			IgnoreData &id = *ign;

			if (id.time && id.time <= Anope::CurTime)
			{
				Log(LOG_DEBUG) << "Expiring ignore entry " << id.mask;
				this->ignores.erase(ign);
			}
			else
				return &id;
		}

		return NULL;
	}
};

class CommandOSIgnore : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (!ignore_service)
			return MOD_CONT;

		const Anope::string &time = params.size() > 1 ? params[1] : "";
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &reason = params.size() > 3 ? params[3] : "";

		if (time.empty() || nick.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}
		else
		{
			time_t t = dotime(time);

			if (t <= -1)
			{
				source.Reply(_("You have to enter a valid number as time."));
				return MOD_CONT;
			}

			ignore_service->AddIgnore(nick, source.u->nick, reason, t);
			if (!t)
				source.Reply(_("\002%s\002 will now permanently be ignored."), nick.c_str());
			else
				source.Reply(_("\002%s\002 will now be ignored for \002%s\002."), nick.c_str(), time.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source)
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (!ignore_service)
			return MOD_CONT;

		const std::list<IgnoreData> &ignores = ignore_service->GetIgnores();
		if (ignores.empty())
			source.Reply(_("Ignore list is empty."));
		else
		{
			source.Reply(_("Services ignore list:\n"
					"  Mask        Creator      Reason      Expires"));
			for (std::list<IgnoreData>::const_iterator ign = ignores.begin(), ign_end = ignores.end(); ign != ign_end; ++ign)
			{
				const IgnoreData &ignore = *ign;

				source.Reply("  %-11s %-11s  %-11s %s", ignore.mask.c_str(), ignore.creator.c_str(), ignore.reason.c_str(), do_strftime(ignore.time).c_str());
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (!ignore_service)
			return MOD_CONT;

		const Anope::string nick = params.size() > 1 ? params[1] : "";
		if (nick.empty())
			this->OnSyntaxError(source, "DEL");
		else if (ignore_service->DelIgnore(nick))
			source.Reply(_("\002%s\002 will no longer be ignored."), nick.c_str());
		else
			source.Reply(_("Nick \002%s\002 not found on ignore list."), nick.c_str());

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (!ignore_service)
			return MOD_CONT;

		ignore_service->ClearIgnores();
		source.Reply(_("Ignore list has been cleared."));

		return MOD_CONT;
	}

 public:
	CommandOSIgnore() : Command("IGNORE", 1, 4, "operserv/ignore")
	{
		this->SetDesc(_("Modify the Services ignore list"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002IGNORE {ADD|DEL|LIST|CLEAR} [\037time\037] [\037nick\037] [\037reason\037]\002\n"
				" \n"
				"Allows Services Operators to make Services ignore a nick or mask\n"
				"for a certain time or until the next restart. The default\n"
				"time format is seconds. You can specify it by using units.\n"
				"Valid units are: \037s\037 for seconds, \037m\037 for minutes, \n"
				"\037h\037 for hours and \037d\037 for days. \n"
				"Combinations of these units are not permitted.\n"
				"To make Services permanently ignore the	user, type 0 as time.\n"
				"When adding a \037mask\037, it should be in the format user@host\n"
				"or nick!user@host, everything else will be considered a nick.\n"
				"Wildcards are permitted.\n"
				" \n"
				"Ignores will not be enforced on IRC Operators."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "IGNORE", _("IGNORE {ADD|DEL|LIST|CLEAR} [\037time\037] [\037nick\037] [\037reason\037]\002"));
	}
};

class OSIgnore : public Module
{
	OSIgnoreService osignoreservice;
	CommandOSIgnore commandosignore;

 public:
	OSIgnore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), osignoreservice(this, "operserv/ignore")
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosignore);

		Implementation i[] = { I_OnDatabaseRead, I_OnDatabaseWrite, I_OnPreCommandRun, I_OnBotPrivmsg };
		ModuleManager::Attach(i, this, 4);

		ModuleManager::RegisterService(&this->osignoreservice);
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		if (params.size() >= 4 && params[0].equals_ci("OS") && params[1].equals_ci("IGNORE"))
		{
			service_reference<IgnoreService> ignore_service("operserv/ignore");
			if (ignore_service)
			{
				const Anope::string &mask = params[2];
				time_t time = params[3].is_pos_number_only() ? convertTo<time_t>(params[3]) : 0;
				const Anope::string &creator = params.size() > 4 ? params[4] : "";
				const Anope::string &reason = params.size() > 5 ? params[5] : "";
				ignore_service->AddIgnore(mask, creator, reason, time - Anope::CurTime);

				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const Anope::string &))
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (ignore_service)
		{
			for (std::list<IgnoreData>::iterator ign = ignore_service->GetIgnores().begin(), ign_end = ignore_service->GetIgnores().end(); ign != ign_end; )
			{
				if (ign->time && ign->time <= Anope::CurTime)
				{
					Log(LOG_DEBUG) << "[os_ignore] Expiring ignore entry " << ign->mask;
					ign = ignore_service->GetIgnores().erase(ign);
				}
				else
				{
					std::stringstream buf;
					buf << "OS IGNORE " << ign->mask << " " << ign->time << " " << ign->creator << " :" << ign->reason;
					Write(buf.str());
					++ign;
				}
			}
		}
	}

	EventReturn OnPreCommandRun(User *&u, BotInfo *&bi, Anope::string &command, Anope::string &message, ChannelInfo *&ci)
	{
		return this->OnBotPrivmsg(u, bi, message);
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, const Anope::string &message)
	{
		service_reference<IgnoreService> ignore_service("operserv/ignore");
		if (ignore_service)
		{
			if (ignore_service->Find(u->nick))
				return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSIgnore)
