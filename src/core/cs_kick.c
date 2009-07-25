/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSKick : public Command
{
 public:
	CommandCSKick() : Command("KICK", 2, 3)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *target = params[1].c_str();
		const char *reason = NULL;

		if (params.size() > 2)
		{
			params[2].resize(200);
			reason = params[2].c_str();
		}

		Channel *c = findchan(chan);
		ChannelInfo *ci;
		User *u2;

		int is_same;

		if (!reason) {
			reason = "Requested";
		}

		is_same = (target == u->nick) ? 1 : (stricmp(target, u->nick) == 0);

		if (c)
			ci = c->ci;

		if (!c) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (is_same ? !(u2 = u) : !(u2 = finduser(target))) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, target);
		} else if (!is_on_chan(c, u2)) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
		} else if (!is_same ? !check_access(u, ci, CA_KICK) :
					 !check_access(u, ci, CA_KICKME)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else if (!is_same && (ci->flags & CI_PEACE)
					 && (get_access(u2, ci) >= get_access(u, ci))) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else if (is_protected(u2)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else {
			const char *av[3];

			if ((ci->flags & CI_SIGNKICK)
				|| ((ci->flags & CI_SIGNKICK_LEVEL)
					&& !check_access(u, ci, CA_SIGNKICK)))
				ircdproto->SendKick(whosends(ci), ci->name, target, "%s (%s)",
								 reason, u->nick);
			else
				ircdproto->SendKick(whosends(ci), ci->name, target, "%s", reason);
			av[0] = ci->name;
			av[1] = target;
			av[2] = reason;
			do_kick(s_ChanServ, 3, av);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "KICK", CHAN_KICK_SYNTAX);
	}
};

class CSKick : public Module
{
 public:
	CSKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSKick(), MOD_UNIQUE);
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_KICK);
	}
};

MODULE_INIT("cs_kick", CSKick)
