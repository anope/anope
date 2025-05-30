/* ChanServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Original Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send any bug reports to the Anope Coder, as they will be able
 * to deal with it best.
 */

#include "module.h"

class CommandCSEnforce final
	: public Command
{
private:
	void DoSecureOps(CommandSource &source, ChannelInfo *ci)
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

		for (const auto &[_, uc] : ci->c->users)
		{
			ci->c->SetCorrectModes(uc->user, false);
		}

		if (!hadsecureops)
			ci->Shrink<bool>("SECUREOPS");

		source.Reply(_("Secureops enforced on %s."), ci->name.c_str());
	}

	void DoRestricted(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce restricted";

		std::vector<User *> users;

		for (const auto &[_, uc] : ci->c->users)
		{
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (ci->AccessFor(user).empty())
				users.push_back(user);
		}

		for (auto *user : users)
		{
			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("RESTRICTED enforced by ")) + source.GetNick();
			ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, reason);
		}

		source.Reply(_("Restricted enforced on %s."), ci->name.c_str());
	}

	void DoRegOnly(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce registered only";

		std::vector<User *> users;
		for (const auto &[_, uc] : ci->c->users)
		{
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (!user->IsIdentified())
				users.push_back(user);
		}

		for (auto *user : users)
		{
			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("REGONLY enforced by ")) + source.GetNick();
			if (!ci->c->HasMode("REGISTEREDONLY"))
				ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, reason);
		}

		source.Reply(_("Registered only enforced on %s."), ci->name.c_str());
	}

	void DoSSLOnly(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce SSL only";

		std::vector<User *> users;
		for (auto &[_, uc] : ci->c->users)
		{
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (!user->IsSecurelyConnected())
				users.push_back(user);
		}

		for (auto *user : users)
		{
			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, _("SSLONLY enforced by ")) + source.GetNick();
			if (!ci->c->HasMode("SSL"))
				ci->c->SetMode(NULL, "BAN", mask);
			ci->c->Kick(NULL, user, reason);
		}

		source.Reply(_("SSL only enforced on %s."), ci->name.c_str());
	}

	void DoBans(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce bans";

		std::vector<User *> users;
		for (const auto &[_, uc] : ci->c->users)
		{
			User *user = uc->user;

			if (user->IsProtected())
				continue;

			if (ci->c->MatchesList(user, "BAN") && !ci->c->MatchesList(user, "EXCEPT"))
				users.push_back(user);
		}

		for (auto *user : users)
		{
			Anope::string reason = Language::Translate(user, _("BANS enforced by ")) + source.GetNick();
			ci->c->Kick(NULL, user, reason);
		}

		source.Reply(_("Bans enforced on %s."), ci->name.c_str());
	}

	void DoLimit(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK") && source.HasPriv("chanserv/access/modify");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enforce limit";

		Anope::string l_str;
		if (!ci->c->GetParam("LIMIT", l_str))
		{
			source.Reply(_("No limit is set on %s."), ci->name.c_str());
			return;
		}

		auto l = Anope::Convert<int>(l_str, -1);
		if (l < 0)
		{
			source.Reply(_("The limit on %s is not valid."), ci->name.c_str());
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

		for (auto *user : users)
		{
			Anope::string reason = Language::Translate(user, _("LIMIT enforced by ")) + source.GetNick();
			ci->c->Kick(NULL, user, reason);
		}

		source.Reply(_("LIMIT enforced on %s, %zu users removed."), ci->name.c_str(), users.size());
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

		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (!ci)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (!ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!source.AccessFor(ci).HasPriv("AKICK") && !source.HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
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
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enforce various channel modes and set options. The \037channel\037 "
			"option indicates what channel to enforce the modes and options "
			"on. The \037what\037 option indicates what modes and options to "
			"enforce, and can be any of \002SECUREOPS\002, \002RESTRICTED\002, \002REGONLY\002, \002SSLONLY\002, "
			"\002BANS\002, or \002LIMIT\002."
			"\n\n"
			"Use \002SECUREOPS\002 to enforce the SECUREOPS option, even if it is not "
			"enabled. Use \002RESTRICTED\002 to enforce the RESTRICTED option, also "
			"if it's not enabled. Use \002REGONLY\002 to kick all unregistered users "
			"from the channel. Use \002SSLONLY\002 to kick all users not using a secure "
			"connection from the channel. \002BANS\002 will enforce bans on the channel by "
			"kicking users affected by them, and \002LIMIT\002 will kick users until the "
			"user count drops below the channel limit, if one is set."
		));
		return true;
	}
};

class CSEnforce final
	: public Module
{
	CommandCSEnforce commandcsenforce;

public:
	CSEnforce(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsenforce(this)
	{

	}
};

MODULE_INIT(CSEnforce)
