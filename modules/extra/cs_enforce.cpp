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
#include "chanserv.h"

static Module *me;

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
		ChannelInfo *ci;
		int16 old_nojoin_level;
		Anope::string mask;

		if (!(ci = c->ci))
			return;

		old_nojoin_level = ci->levels[CA_NOJOIN];
		if (ci->levels[CA_NOJOIN] < 0)
			ci->levels[CA_NOJOIN] = 0;

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			if (check_access(uc->user, ci, CA_NOJOIN))
			{
				get_idealban(ci, uc->user, mask);
				Anope::string reason = translate(uc->user, CHAN_NOT_ALLOWED_TO_JOIN);
				c->SetMode(NULL, CMODE_BAN, mask);
				c->Kick(NULL, uc->user, "%s", reason.c_str());
			}
		}

		ci->levels[CA_NOJOIN] = old_nojoin_level;
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
	CommandCSEnforce() : Command("ENFORCE", 1, 2)
	{
		this->SetDesc(_("Enforce various channel modes and set options"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
			source.Reply(_(CHAN_X_NOT_IN_USE), ci->name.c_str());
		else if (!check_access(u, ci, CA_AKICK))
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

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002ENFORCE \037channel\037 [\037what\037]\002"));
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

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002ENFORCE \037channel\037 [\037what\037]\002"));
	}
};

class CSEnforce : public Module
{
	CommandCSEnforce commandcsenforce;

 public:
	CSEnforce(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		me = this;

		this->AddCommand(chanserv->Bot(), &commandcsenforce);
	}
};

MODULE_INIT(CSEnforce)
