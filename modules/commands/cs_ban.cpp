/* ChanServ core functions
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

class CommandCSBan : public Command
{
 public:
	CommandCSBan(Module *creator) : Command(creator, "chanserv/ban", 2, 3)
	{
		this->SetDesc(_("Bans a selected nick on a channel"));
		this->SetSyntax(_("\037#channel\037 \037nick\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		User *u = source.u;
		Channel *c = ci->c;
		bool is_same = target.equals_ci(u->nick);
		User *u2 = is_same ? u : finduser(target);

		AccessGroup u_access = ci->AccessFor(u), u2_access = ci->AccessFor(u2);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!u2)
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
		else if (!ci->AccessFor(u).HasPriv(CA_BAN))
			source.Reply(ACCESS_DENIED);
		else if (!is_same && ci->HasFlag(CI_PEACE) && u2_access >= u_access)
			source.Reply(ACCESS_DENIED);
		/*
		 * Dont ban/kick the user on channels where he is excepted
		 * to prevent services <-> server wars.
		 */
		else if (matches_list(ci->c, u2, CMODE_EXCEPT))
			source.Reply(CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
		else if (u2->IsProtected())
			source.Reply(ACCESS_DENIED);
		else
		{
			Anope::string mask;
			get_idealban(ci, u2, mask);

			// XXX need a way to detect if someone is overriding
			Log(LOG_COMMAND, u, this, ci) << "for " << mask;

			c->SetMode(NULL, CMODE_BAN, mask);

			/* We still allow host banning while not allowing to kick */
			if (!c->FindUser(u2))
				return;

			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !ci->AccessFor(u).HasPriv(CA_SIGNKICK)))
				c->Kick(ci->WhoSends(), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
			else
				c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Bans a selected nick on a channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel."));
		return true;
	}
};

class CSBan : public Module
{
	CommandCSBan commandcsban;

 public:
	CSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcsban(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSBan)
