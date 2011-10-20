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

class CommandCSClearUsers : public Command
{
 public:
	CommandCSClearUsers(Module *creator) : Command(creator, "chanserv/clearusers", 1, 1)
	{
		this->SetDesc(_("Kicks all users on a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		Channel *c = findchan(chan);
		Anope::string modebuf;

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}
		else if (!c->ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, c->name.c_str());
			return;
		}
		else if (!c->ci->AccessFor(u).HasPriv("FOUNDER"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}
		
		Anope::string buf = "CLEARUSERS command from " + u->nick + " (" + u->Account()->display + ")";

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			c->Kick(NULL, uc->user, "%s", buf.c_str());
		}

		source.Reply(_("All users have been kicked from \2%s\2."), chan.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Tells %s to clear (kick) all users certain settings on a channel."
				" \n"
				"By default, limited to those with founder access on the\n"
				"channel."), source.owner->nick.c_str());
		return true;
	}
};

class CSClearUsers : public Module
{
	CommandCSClearUsers commandcsclearusers;

 public:
	CSClearUsers(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsclearusers(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSClearUsers)
