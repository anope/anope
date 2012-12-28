/* cs_enforce - Add a /cs ENFORCE command to enforce various set
 *			  options and channelmodes on a channel.
 *
 * (C) 2003-2012 Anope Team
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

class CommandCSEnforce : public Command
{
 private:
	void DoSet(CommandSource &source, Channel *c)
	{
		const ChannelInfo *ci = c->ci;

		if (!ci)
			return;

		Log(LOG_COMMAND, source, this) << "to enforce set";

		if (ci->HasFlag(CI_SECUREOPS))
			this->DoSecureOps(source, c);
		if (ci->HasFlag(CI_RESTRICTED))
			this->DoRestricted(source, c);
	}

	void DoModes(CommandSource &source, Channel *c)
	{
		Log(LOG_COMMAND, source, this) << "to enforce modes";

		if (c->HasMode(CMODE_REGISTEREDONLY))
			this->DoCModeR(source, c);
	}

	void DoSecureOps(CommandSource &source, Channel *c)
	{
		ChannelInfo *ci = c->ci;

		if (!ci)
			return;

		Log(LOG_COMMAND, source, this) << "to enforce secureops";

		/* Dirty hack to allow Channel::SetCorrectModes to work ok.
		 * We pretend like SECUREOPS is on so it doesn't ignore that
		 * part of the code. This way we can enforce SECUREOPS even
		 * if it's off.
		 */
		bool hadsecureops = false;
		if (!ci->HasFlag(CI_SECUREOPS))
		{
			ci->SetFlag(CI_SECUREOPS);
			hadsecureops = true;
		}

		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = *it;

			c->SetCorrectModes(uc->user, false, false);
		}

		if (hadsecureops)
			ci->UnsetFlag(CI_SECUREOPS);
	}

	void DoRestricted(CommandSource &source, Channel *c)
	{
		ChannelInfo *ci = c->ci;
		if (ci == NULL)
			return;

		Log(LOG_COMMAND, source, this) << "to enforce restricted";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = *it;
			User *user = uc->user;

			if (ci->AccessFor(user).empty())
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{	
			User *user = users[i];

			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, CHAN_NOT_ALLOWED_TO_JOIN);
			c->SetMode(NULL, CMODE_BAN, mask);
			c->Kick(NULL, user, "%s", reason.c_str());
		}
	}

	void DoCModeR(CommandSource &source, Channel *c)
	{
		ChannelInfo *ci = c->ci;

		if (!ci)
			return;

		Log(LOG_COMMAND, source, this) << "to enforce registered only";

		std::vector<User *> users;
		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = *it;
			User *user = uc->user;

			if (!user->IsIdentified())
				users.push_back(user);
		}

		for (unsigned i = 0; i < users.size(); ++i)
		{
			User *user = users[i];

			Anope::string mask = ci->GetIdealBan(user);
			Anope::string reason = Language::Translate(user, CHAN_NOT_ALLOWED_TO_JOIN);
			if (!c->HasMode(CMODE_REGISTEREDONLY))
				c->SetMode(NULL, CMODE_BAN, mask);
			c->Kick(NULL, user, "%s", reason.c_str());
		}
	}
 public:
	CommandCSEnforce(Module *creator) : Command(creator, "chanserv/enforce", 1, 2)
	{
		this->SetDesc(_("Enforce various channel modes and set options"));
		this->SetSyntax(_("\037channel\037 [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &what = params.size() > 1 ? params[1] : "";

		Channel *c = Channel::Find(params[0]);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (!c->ci)
			source.Reply(CHAN_X_NOT_REGISTERED, c->name.c_str());
		else if (!source.AccessFor(c->ci).HasPriv("AKICK"))
			source.Reply(ACCESS_DENIED);
		else
		{
			if (what.empty() || what.equals_ci("SET"))
			{
				this->DoSet(source, c);
				source.Reply(_("Enforced %s"), !what.empty() ? what.c_str() : "SET");
			}
			else if (what.equals_ci("MODES"))
			{
				this->DoModes(source, c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("SECUREOPS"))
			{
				this->DoSecureOps(source, c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("RESTRICTED"))
			{
				this->DoRestricted(source, c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("+R"))
			{
				this->DoCModeR(source, c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else
				this->OnSyntaxError(source, "");
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enforce various channel modes and set options. The \037channel\037\n"
			"option indicates what channel to enforce the modes and options\n"
			"on. The \037what\037 option indicates what modes and options to\n"
			"enforce, and can be any of SET, SECUREOPS, RESTRICTED, MODES,\n"
			"or +R. When left out, it defaults to SET.\n"
			" \n"
			"If \037what\037 is SET, it will enforce SECUREOPS and RESTRICTED\n"
			"on the users currently in the channel, if they are set. Give\n"
			"SECUREOPS to enforce the SECUREOPS option, even if it is not\n"
			"enabled. Use RESTRICTED to enfore the RESTRICTED option, also\n"
			"if it's not enabled."));
		source.Reply(" ");
		if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
			source.Reply(_("If \037what\037 is MODES, it will enforce channelmode +R if it is\n"
				"set. If +R is specified for \037what\037, the +R channelmode will\n"
				"also be enforced, but even if it is not set. If it is not set,\n"
				"users will be banned to ensure they don't just rejoin."));
		else
			source.Reply(_("If \037what\037 is MODES, nothing will be enforced, since it would\n"
				"enforce modes that the current ircd does not support. If +R is\n"
				"specified for \037what\037, an equalivant of channelmode +R on\n"
				"other ircds will be enforced. All users that are in the channel\n"
				"but have not identified for their nickname will be kicked and\n"
				"banned from the channel."));

		return true;
	}
};

class CSEnforce : public Module
{
	CommandCSEnforce commandcsenforce;

 public:
	CSEnforce(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsenforce(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSEnforce)
