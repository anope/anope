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

void myChanServHelp(User * u);


/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_BAN);
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_UNBAN);
}

class CommandCSBan : public Command
{
 public:
	CommandCSBan() : Command("BAN", 1, 3)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *target = params[1].c_str();
		params[2].resize(200);
		const char *reason = params[2].c_str();

		Channel *c;
		ChannelInfo *ci;
		User *u2;

		if (!chan)
			return MOD_CONT;

		int is_same;

		if (!reason)
			reason = "Requested";

		if (!target)
			target = u->nick;

		is_same = (stricmp(target, u->nick) == 0);

		if (!(c = findchan(chan))) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (!(ci = c->ci)) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
		} else if (ci->flags & CI_FORBIDDEN) {
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
		} else if (is_same ? !(u2 = u) : !(u2 = finduser(target))) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, target);
		} else if (!is_same ? !check_access(u, ci, CA_BAN) :
					 !check_access(u, ci, CA_BANME)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else if (!is_same && (ci->flags & CI_PEACE)
					 && (get_access(u2, ci) >= get_access(u, ci))) {
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			/*
			 * Dont ban/kick the user on channels where he is excepted
			 * to prevent services <-> server wars.
			 */
		} else if (ircd->except && is_excepted(ci, u2)) {
			notice_lang(s_ChanServ, u, CHAN_EXCEPTED, u2->nick, ci->name);
		} else if (ircd->protectedumode && is_protected(u2)) {
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
		} else {
			const char *av[3];
			char mask[BUFSIZE];

			av[0] = "+b";
			get_idealban(ci, u2, mask, sizeof(mask));
			av[1] = mask;
			ircdproto->SendMode(whosends(ci), c->name, "+b %s", av[1]);
			chan_set_modes(s_ChanServ, c, 2, av, 1);

			/* We still allow host banning while not allowing to kick */
			if (!is_on_chan(c, u2))
				return MOD_CONT;

			if ((ci->flags & CI_SIGNKICK)
				|| ((ci->flags & CI_SIGNKICK_LEVEL)
					&& !check_access(u, ci, CA_SIGNKICK)))
				ircdproto->SendKick(whosends(ci), ci->name, target, "%s (%s)",
								 reason, u->nick);
			else
				ircdproto->SendKick(whosends(ci), ci->name, target, "%s", reason);

			const char *kav[4];
			 kav[0] = ci->name;
			 kav[1] = target;
			 kav[2] = reason;
			do_kick(s_ChanServ, 3, kav);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_BAN);
		return true;
	}
};



class CommandCSUnban : public Command
{
 public:
	CommandCSUnban() : Command("UNBAN", 1, 1)
	{

	}

	CommandReturn Execute(User *u,  std::vector<std::string> &params)
	{
		char *chan = strtok(NULL, " ");
		Channel *c;
		ChannelInfo *ci;

		if (!(c = findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}

		if (!(ci = c->ci))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (ci->flags & CI_FORBIDDEN)
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_UNBAN))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		common_unban(ci, u->nick);
		notice_lang(s_ChanServ, u, CHAN_UNBANNED, chan);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_UNBAN);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "UNBAN", CHAN_UNBAN_SYNTAX);
	}
};



class CSBan : public Module
{
 public:
	CSBan(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSBan(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSUnban(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};


MODULE_INIT("cs_ban", CSBan)
