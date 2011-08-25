/* cs_enforce - Add a /cs ENFORCE command to enforce various set
 *			  options and channelmodes on a channel.
 *
 * (C) 2003-2011 Anope Team
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
	void DoSet(Channel *c)
	{
		ChannelInfo *ci;

		if (!(ci = c->ci))
			return;

		if (ci->HasFlag(CI_SECUREOPS))
			this->DoSecureOps(c);
		if (ci->HasFlag(CI_RESTRICTED))
			this->DoRestricted(c);
	}

	void DoModes(Channel *c)
	{
		if (c->HasMode(CMODE_REGISTEREDONLY))
			this->DoCModeR(c);
	}

	void DoSecureOps(Channel *c)
	{
		ChannelInfo *ci;
		bool hadsecureops = false;

		if (!(ci = c->ci))
			return;

		/* Dirty hack to allow chan_set_correct_modes to work ok.
		 * We pretend like SECUREOPS is on so it doesn't ignore that
		 * part of the code. This way we can enforce SECUREOPS even
		 * if it's off.
		 */
		if (!ci->HasFlag(CI_SECUREOPS))
		{
			ci->SetFlag(CI_SECUREOPS);
			hadsecureops = true;
		}

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
		{
			UserContainer *uc = *it;

			chan_set_correct_modes(uc->user, c, 0);
		}

		if (hadsecureops)
			ci->UnsetFlag(CI_SECUREOPS);
	}

	void DoRestricted(Channel *c)
	{
		ChannelInfo *ci = c->ci;
		if (ci == NULL)
			return;

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;
			User *user = uc->user;

			if (ci->AccessFor(user).empty())
			{
				Anope::string mask;
				get_idealban(ci, user, mask);
				Anope::string reason = translate(user, CHAN_NOT_ALLOWED_TO_JOIN);
				c->SetMode(NULL, CMODE_BAN, mask);
				c->Kick(NULL, user, "%s", reason.c_str());
			}
		}
	}

	void DoCModeR(Channel *c)
	{
		ChannelInfo *ci;
		Anope::string mask;

		if (!(ci = c->ci))
			return;

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			if (!uc->user->IsIdentified())
			{
				get_idealban(ci, uc->user, mask);
				Anope::string reason = translate(uc->user, CHAN_NOT_ALLOWED_TO_JOIN);
				if (!c->HasMode(CMODE_REGISTEREDONLY))
					c->SetMode(NULL, CMODE_BAN, mask);
				c->Kick(NULL, uc->user, "%s", reason.c_str());
			}
		}
	}
 public:
	CommandCSEnforce(Module *creator) : Command(creator, "chanserv/enforce", 1, 2)
	{
		this->SetDesc(_("Enforce various channel modes and set options"));
		this->SetSyntax(_("\037channel\037 [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		Channel *c = findchan(params[0]);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (!c->ci)
			source.Reply(CHAN_X_NOT_REGISTERED, c->name.c_str());
		else if (!c->ci->AccessFor(u).HasPriv(CA_AKICK))
			source.Reply(ACCESS_DENIED);
		else
		{
			if (what.empty() || what.equals_ci("SET"))
			{
				this->DoSet(c);
				source.Reply(_("Enforced %s"), !what.empty() ? what.c_str() : "SET");
			}
			else if (what.equals_ci("MODES"))
			{
				this->DoModes(c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("SECUREOPS"))
			{
				this->DoSecureOps(c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("RESTRICTED"))
			{
				this->DoRestricted(c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else if (what.equals_ci("+R"))
			{
				this->DoCModeR(c);
				source.Reply(_("Enforced %s"), what.c_str());
			}
			else
				this->OnSyntaxError(source, "");
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
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
