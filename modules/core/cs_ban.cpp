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
	CommandCSBan(const Anope::string &cname) : Command("BAN", 2, 3)
	{
		this->SetDesc("Bans a selected nick on a channel");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;
		bool is_same = target.equals_ci(u->nick);
		User *u2 = is_same ? u : finduser(target);

		ChanAccess *u_access = ci->GetAccess(u), *u2_access = ci->GetAccess(u2);
		uint16 u_level = u_access ? u_access->level : 0, u2_level = u2_access ? u2_access->level : 0;

		if (!c)
			source.Reply(_(CHAN_X_NOT_IN_USE), chan.c_str());
		else if (!u2)
			source.Reply(_(NICK_X_NOT_IN_USE), target.c_str());
		else if (!is_same ? !check_access(u, ci, CA_BAN) : !check_access(u, ci, CA_BANME))
			source.Reply(_(ACCESS_DENIED));
		else if (!is_same && ci->HasFlag(CI_PEACE) && u2_level >= u_level)
			source.Reply(_(ACCESS_DENIED));
		/*
		 * Dont ban/kick the user on channels where he is excepted
		 * to prevent services <-> server wars.
		 */
		else if (matches_list(ci->c, u2, CMODE_EXCEPT))
			source.Reply(_(CHAN_EXCEPTED), u2->nick.c_str(), ci->name.c_str());
		else if (u2->IsProtected())
			source.Reply(_(ACCESS_DENIED));
		else
		{
			Anope::string mask;
			get_idealban(ci, u2, mask);

			// XXX need a way to detect if someone is overriding
			Log(LOG_COMMAND, u, this, ci) << "for " << mask;

			c->SetMode(NULL, CMODE_BAN, mask);

			/* We still allow host banning while not allowing to kick */
			if (!c->FindUser(u2))
				return MOD_CONT;

			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(u, ci, CA_SIGNKICK)))
				c->Kick(whosends(ci), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
			else
				c->Kick(whosends(ci), u2, "%s", reason.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002BAN \037#channel\037 \037nick\037 [\037reason\037]\002\n"
				" \n"
				"Bans a selected nick on a channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "BAN", _("BAN \037#channel\037 \037nick\037 [\037reason\037]"));
	}
};

class CSBan : public Module
{
	CommandCSBan commandcsban;

 public:
	CSBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), commandcsban("BAN")
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsban);
	}
};

MODULE_INIT(CSBan)
