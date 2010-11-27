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
	CommandCSKick() : Command("KICK", 2, 3)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;
		User *u2;

		bool is_same = target.equals_ci(u->nick);

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (is_same ? !(u2 = u) : !(u2 = finduser(target)))
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
		else if (!is_same ? !check_access(u, ci, CA_KICK) : !check_access(u, ci, CA_KICKME))
			source.Reply(ACCESS_DENIED);
		else if (!is_same && (ci->HasFlag(CI_PEACE)) && get_access(u2, ci) >= get_access(u, ci))
			source.Reply(ACCESS_DENIED);
		else if (u2->IsProtected())
			source.Reply(ACCESS_DENIED);
		else if (!c->FindUser(u2))
			source.Reply(NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_KICK);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "KICK", CHAN_KICK_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_KICK);
	}
};

class CSKick : public Module
{
	CommandCSKick commandcskick;

 public:
	CSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcskick);
	}
};

MODULE_INIT(CSKick)
