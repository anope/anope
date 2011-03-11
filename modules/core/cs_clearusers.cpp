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
	CommandCSClearUsers() : Command("CLEARUSERS", 1, 1)
	{
		this->SetDesc(_("Tells ChanServ to clear (kick) all users on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		Anope::string modebuf;

		if (!c)
			source.Reply(_(CHAN_X_NOT_IN_USE), chan.c_str());
		else if (!check_access(u, ci, CA_FOUNDER))
			source.Reply(_(ACCESS_DENIED));
		
		Anope::string buf = "CLEARUSERS command from " + u->nick + " (" + u->Account()->display + ")";

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			c->Kick(NULL, uc->user, "%s", buf.c_str());
		}

		source.Reply(_("All users have been kicked from \2%s\2."), chan.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002CLEARUSERS \037channel\037\002\n"
				" \n"
				"Tells %s to clear (kick) all users certain settings on a channel."
				" \n"
				"By default, limited to those with founder access on the\n"
				"channel."), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CLEAR", _("CLEARUSERS \037channel\037"));
	}
};

class CSClearUsers : public Module
{
	CommandCSClearUsers commandcsclearusers;

 public:
	CSClearUsers(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsclearusers);
	}
};

MODULE_INIT(CSClearUsers)
