/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSKick : public Command
{
 public:
	CommandCSKick(const Anope::string &cname) : Command(cname, 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string target = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "Requested";

		Channel *c = findchan(chan);
		ChannelInfo *ci;
		User *u2;

		int is_same;

		is_same = target.equals_cs(u->nick) ? 1 : target.equals_ci(u->nick);

		if (c)
			ci = c->ci;

		if (!c)
			u->SendMessage(ChanServ, CHAN_X_NOT_IN_USE, chan.c_str());
		else if (is_same ? !(u2 = u) : !(u2 = finduser(target)))
			u->SendMessage(ChanServ, NICK_X_NOT_IN_USE, target.c_str());
		else if (!is_same ? !check_access(u, ci, CA_KICK) : !check_access(u, ci, CA_KICKME))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (!is_same && (ci->HasFlag(CI_PEACE)) && get_access(u2, ci) >= get_access(u, ci))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (u2->IsProtected())
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (!c->FindUser(u2))
			u->SendMessage(ChanServ, NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
		else
		{
			 // XXX
			Log(LOG_COMMAND, u, this, ci) << "for " << u2->nick;

			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(u, ci, CA_SIGNKICK)))
				ci->c->Kick(whosends(ci), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
			else
				ci->c->Kick(whosends(ci), u2, "%s", reason.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "KICK", CHAN_KICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_KICK);
	}
};

class CSKick : public Module
{
	CommandCSKick commandcskick, commandcsk;

 public:
	CSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), commandcskick("KICK"), commandcsk("K")
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcskick);
		this->AddCommand(ChanServ, &commandcsk);
	}
};

MODULE_INIT(CSKick)
