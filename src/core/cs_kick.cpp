/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSKick : public Command
{
 public:
	CommandCSKick(const ci::string &cname) : Command(cname, 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *target = params[1].c_str();
		const char *reason = NULL;

		if (params.size() > 2)
		{
			reason = params[2].c_str();
		}

		Channel *c = findchan(chan);
		ChannelInfo *ci;
		User *u2;

		int is_same;

		if (!reason) {
			reason = "Requested";
		}

		is_same = (target == u->nick.c_str()) ? 1 : (stricmp(target, u->nick.c_str()) == 0);

		if (c)
			ci = c->ci;

		if (!c) {
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (is_same ? !(u2 = u) : !(u2 = finduser(target))) {
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, target);
		} else if (!is_same ? !check_access(u, ci, CA_KICK) : !check_access(u, ci, CA_KICKME)) {
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		} else if (!is_same && (ci->HasFlag(CI_PEACE))
					 && (get_access(u2, ci) >= get_access(u, ci))) {
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		} else if (u2->IsProtected())
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else if (!c->FindUser(u2)) {
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
		} else {
			if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(u, ci, CA_SIGNKICK)))
				ci->c->Kick(whosends(ci), u2, "%s (%s)", reason, u->nick.c_str());
			else
				ci->c->Kick(whosends(ci), u2, "%s", reason);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_KICK);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "KICK", CHAN_KICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_KICK);
	}
};

class CSKick : public Module
{
 public:
	CSKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(ChanServ, new CommandCSKick("KICK"));
		this->AddCommand(ChanServ, new CommandCSKick("K"));
	}
};

MODULE_INIT(CSKick)
