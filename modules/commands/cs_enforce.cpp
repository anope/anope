/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Original Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send any bug reports to the Anope Coder, as he will be able
 * to deal with it best.
 */

#include "module.h"

class CommandCSEnforce : public Command
{
 private:
	void DoSecureOps(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce secureops";

		/* Dirty hack to allow Channel::SetCorrectModes to work ok.
		 * We pretend like SECUREOPS is on so it doesn't ignore that
		 * part of the code. This way we can enforce SECUREOPS even
		 * if it's off.
		 */
		bool hadsecureops = ci->HasExt("SECUREOPS");
		ci->Extend<bool>("SECUREOPS");

		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;

			ci->c->SetCorrectModes(uc->user, false);
		}

		if (!hadsecureops)
			ci->Shrink<bool>("SECUREOPS");

		source.Reply(_("\002Secureops\002 enforced on \002{0}\002."), ci->name);
	}

	void DoRestricted(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce restricted";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (ci->AccessFor(user).empty())
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("RESTRICTED enforced by ")) + source.GetNick();
			ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, "%s", reason.c_str());
		}

		source.Reply(_("\002Restricted\002 enforced on \002{0}\002."), ci->name);
	}

	void DoRegOnly(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce registered only";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (!user->IsIdentified())
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("REGONLY enforced by ")) + source.GetNick();
			if (!ci->c->HasMode("REGISTEREDONLY"))
				ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, "%s", reason.c_str());
		}

		source.Reply(_("\002Registered only\002 enforced on \002{0}\002."), ci->name);
	}

	void DoSSLOnly(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce SSL only";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (!user->HasMode("SSL") && !user->HasExt("ssl"))
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("SSLONLY enforced by ")) + source.GetNick();
			if (!ci->c->HasMode("SSL"))
				ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, "%s", reason.c_str());
		}

		source.Reply(_("\002SSL only\002 enforced on %s."), ci->name.c_str());
	}

	void DoBans(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce bans";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (ci->c->MatchesList(user, "BAN") && !ci->c->MatchesList(user, "EXCEPT"))
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string reason = Language::Translate(user, _("BANS enforced by ")) + source.GetNick();
			ci->c->Kick(NULL, user, "%s", reason.c_str());
		}

		source.Reply(_("\002Bans\002 enforced on %s."), ci->name.c_str());
	}

	void DoLimit(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce limit";

		Anope::string l_str;
		if (!ci->c->GetParam("LIMIT", l_str))
		{
			source.Reply(_("There is no limit is set on \002{0}\002."), ci->name);
			return;
		}

		int l;
		try
		{
			l = convertTo<int>(l_str);
			if (l < 0)
				throw ConvertException();
		}
		catch (const ConvertException &)
		{
			source.Reply(_("The limit on \002{0}\002 is not valid."), ci->name);
			return;
		}

		std::vector<User *> users;
		/* The newer users are at the end of the list, so kick users starting from the end */
		for (Channel::ChanUserList::reverse_iterator it = ci->c->users.rbegin(), it_end = ci->c->users.rend(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (!ci->AccessFor(user).empty())
				continue;

			if (ci->c->users.size() - users.size() <= static_cast<unsigned>(l))
				continue;

			users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string reason = Language::Translate(user, _("LIMIT enforced by ")) + source.GetNick();
			ci->c->Kick(NULL, user, "%s", reason.c_str());
		}

		source.Reply(_("LIMIT enforced on \002{0}\002, \002{1]\002 users removed."), ci->name, users.size());
	}

 public:
	CommandCSEnforce(Module *creator) : Command(creator, "chanserv/enforce", 2, 2)
	{
		this->SetDesc(_("Enforce various channel modes and set options"));
		this->SetSyntax(_("\037channel\037 \037what\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &what = params.size() > 1 ? params[1] : "";

		ChanServ::Channel *ci = ChanServ::Find(params[0]);

		if (!ci)
			source.Reply(_("Channel \002{0}\002 isn't registered."), params[0]);
		else if (!ci->c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->name);
		else if (!source.AccessFor(ci).HasPriv("AKICK") && !source.HasPriv("chanserv/access/modify"))
			source.Reply("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002.", "AKICK", ci->name);
		else if (what.equals_ci("SECUREOPS"))
			this->DoSecureOps(source, ci);
		else if (what.equals_ci("RESTRICTED"))
			this->DoRestricted(source, ci);
		else if (what.equals_ci("REGONLY"))
			this->DoRegOnly(source, ci);
		else if (what.equals_ci("SSLONLY"))
			this->DoSSLOnly(source, ci);
		else if (what.equals_ci("BANS"))
			this->DoBans(source, ci);
		else if (what.equals_ci("LIMIT"))
			this->DoLimit(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Enforce various channel modes and set options on \037channel\037."
		               "\n"
		               "The \002SECUREOPS\002 option enforces SECUREOPS, even if it is not enabled.\n"
		               "See \002/msg ChanServ HELP SET SECUREOPS\002 for information about SECUREOPS.\n" //XXX
		               "\n"
		               "The \002RESTRICTED\002 option enforces RESTRICTED, even if it is not enabled.\n"
		               "See \002/msg ChanServ HELP SET RESTRICTED\002 for information about RESTRICTED.\n" //XXX
		               "\n"
		               "The \002REGONLY\002 option removes all unregistered users.\n"
		               "\n"
		               "The \002SSLONLY\002 option removes all users not using a secure connection.\n"
		               "\n"
		               "The \002BANS\002 option enforces bans on the channel by removing users affected by then.\n"
		               "\n"
		               "The \002LIMIT\002 option will remove users until the user count drops below the channel limit, if one is set.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege."),
		               "AKICK");
		return true;
	}
};

class CSEnforce : public Module
{
	CommandCSEnforce commandcsenforce;

 public:
	CSEnforce(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsenforce(this)
	{

	}
};

MODULE_INIT(CSEnforce)
