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

class CommandCSKick : public Command
{
 public:
	CommandCSKick(Module *creator) : Command(creator, "chanserv/kick", 2, 3)
	{
		this->SetDesc(_("Kicks a selected nick from a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		const Anope::string &reason = params.size() > 2 ? params[2] : "Requested";

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		Channel *c = findchan(params[0]);
		bool is_same = target.equals_ci(u->nick);
		User *u2 = is_same ? u : finduser(target);

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}
		else if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		AccessGroup u_access = ci->AccessFor(u), u2_access = ci->AccessFor(u2);

		if (!u2)
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
		else if (!ci->AccessFor(u).HasPriv(CA_KICK))
			source.Reply(ACCESS_DENIED);
		else if (!is_same && (ci->HasFlag(CI_PEACE)) && u2_access >= u_access)
			source.Reply(ACCESS_DENIED);
		else if (u2->IsProtected())
			source.Reply(ACCESS_DENIED);
		else if (!c->FindUser(u2))
			source.Reply(NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
		else
		{
			 // XXX
			Log(LOG_COMMAND, u, this, ci) << "for " << u2->nick;

			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !ci->AccessFor(u).HasPriv(CA_SIGNKICK)))
				ci->c->Kick(ci->WhoSends(), u2, "%s (%s)", reason.c_str(), u->nick.c_str());
			else
				ci->c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Kicks a selected nick on a channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel."));
		return true;
	}
};

class CSKick : public Module
{
	CommandCSKick commandcskick;

 public:
	CSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcskick(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcskick);
	}
};

MODULE_INIT(CSKick)
