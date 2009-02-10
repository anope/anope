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

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	if (ircd->owner) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_OWNER);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEOWNER);
	}
	if (ircd->protect) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_PROTECT);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEPROTECT);
	} else if (ircd->admin) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_ADMIN);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEADMIN);
	}

	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_OP);
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEOP);
	if (ircd->halfop) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_HALFOP);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEHALFOP);
	}
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_VOICE);
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEVOICE);
}

/* do_util: not a command, but does the job of other */

static CommandReturn do_util(User *u, CSModeUtil *util, const char *chan, const char *nick)
{
	const char *av[2];
	Channel *c;
	ChannelInfo *ci;
	User *u2;

	int is_same;

	if (!chan) {
		struct u_chanlist *uc;

		av[0] = util->mode;
		av[1] = u->nick;

		/* Sets the mode to the user on every channels he is on. */

		for (uc = u->chans; uc; uc = uc->next) {
			if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
				&& check_access(u, ci, util->levelself)) {
				ircdproto->SendMode(whosends(ci), uc->chan->name, "%s %s",
							   util->mode, u->nick);
				chan_set_modes(s_ChanServ, uc->chan, 2, av, 2);

				if (util->notice && ci->flags & util->notice)
					ircdproto->SendMessage(whosends(ci), uc->chan->name,
						   "%s command used for %s by %s", util->name,
						   u->nick, u->nick);
			}
		}

		return MOD_CONT;
	} else if (!nick) {
		nick = u->nick;
	}

	is_same = (nick == u->nick) ? 1 : (stricmp(nick, u->nick) == 0);

	if (!(c = findchan(chan))) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
	} else if (!(ci = c->ci)) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
	} else if (ci->flags & CI_VERBOTEN) {
		notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
	} else if (is_same ? !(u2 = u) : !(u2 = finduser(nick))) {
		notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
	} else if (!is_on_chan(c, u2)) {
		notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
	} else if (is_same ? !check_access(u, ci, util->levelself) :
			   !check_access(u, ci, util->level)) {
		notice_lang(s_ChanServ, u, ACCESS_DENIED);
	} else if (*util->mode == '-' && !is_same && (ci->flags & CI_PEACE)
			   && (get_access(u2, ci) >= get_access(u, ci))) {
		notice_lang(s_ChanServ, u, PERMISSION_DENIED);
	} else if (*util->mode == '-' && is_protected(u2) && !is_same) {
		notice_lang(s_ChanServ, u, PERMISSION_DENIED);
	} else {
		ircdproto->SendMode(whosends(ci), c->name, "%s %s", util->mode,
					   u2->nick);

		av[0] = util->mode;
		av[1] = u2->nick;
		chan_set_modes(s_ChanServ, c, 2, av, 3);

		if (util->notice && ci->flags & util->notice)
			ircdproto->SendMessage(whosends(ci), c->name, "%s command used for %s by %s",
				   util->name, u2->nick, u->nick);
	}
	return MOD_CONT;
}


class CommandCSOp : public Command
{
 public:
	CommandCSOp() : Command("OP", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_OP], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_OP);
		return true;
	}
};


class CommandCSDeOp : public Command
{
 public:
	CommandCSDeOp() : Command("DEOP", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_DEOP], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_DEOP);
		return true;
	}
};


class CommandCSVoice : public Command
{
 public:
	CommandCSVoice() : Command("VOICE", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_VOICE], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_VOICE);
		return true;
	}
};


class CommandCSDeVoice : public Command
{
 public:
	CommandCSDeVoice() : Command("DEVOICE", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_DEVOICE], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_DEVOICE);
		return true;
	}
};


class CommandCSHalfOp : public Command
{
 public:
	CommandCSHalfOp() : Command("HALFOP", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->halfop)
		{
			return MOD_CONT;

		}

		return do_util(u, &csmodeutils[MUT_HALFOP], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_HALFOP);
		return true;
	}
};


class CommandCSDeHalfOp : public Command
{
 public:
	CommandCSDeHalfOp() : Command("DEHALFOP", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->halfop)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_DEHALFOP], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_DEHALFOP);
		return true;
	}
};


class CommandCSProtect : public Command
{
 public:
	CommandCSProtect() : Command("PROTECT", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->protect && !ircd->admin)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_PROTECT], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_PROTECT);
		return true;
	}
};

/*************************************************************************/

class CommandCSDeProtect : public Command
{
 public:
	CommandCSDeProtect() : Command("DEPROTECT", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->protect && !ircd->admin)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_DEPROTECT], params[0].c_str(), params[1].c_str());
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_DEPROTECT);
		return true;
	}
};

/*************************************************************************/

class CommandCSOwner : public Command
{
 public:
	CommandCSOwner() : Command("OWNER", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *av[2];
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		User *u2;
		Channel *c;
		ChannelInfo *ci;

		if (!ircd->owner)
		{
			return MOD_CONT;
		}

		if (!(c = findchan(chan))) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (!(ci = c->ci)) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
		} else if (ci->flags & CI_VERBOTEN) {
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
		} else if (!(u2 = finduser(nick))) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
		} else if (!is_on_chan(c, u2)) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
		} else if (!is_founder(u, ci)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else {
			ircdproto->SendMode(whosends(ci), c->name, "%s %s", ircd->ownerset,
						u2->nick);

			av[0] = ircd->ownerset;
			av[1] = u2->nick;
			chan_set_modes(s_ChanServ, c, 2, av, 1);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_OWNER);
		return true;
	}
};

/*************************************************************************/

class CommandCSDeOwner : public Command
{
 public:
	CommandCSDeOwner() : Command("DEOWNER", 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *av[2];
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		User *u2;

		Channel *c;
		ChannelInfo *ci;

		if (!ircd->owner) {
			return MOD_CONT;
		}

		if (!(c = findchan(chan))) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (!(ci = c->ci)) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
		} else if (ci->flags & CI_VERBOTEN) {
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
		} else if (!(u2 = finduser(nick))) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
		} else if (!is_on_chan(c, u2)) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
		} else if (!is_founder(u, ci)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else {
			ircdproto->SendMode(whosends(ci), c->name, "%s %s", ircd->ownerunset, u2->nick);

			av[0] = ircd->ownerunset;
			av[1] = u2->nick;
			chan_set_modes(s_ChanServ, c, 2, av, 1);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_DEOWNER);
		return true;
	}
};


class CSModes : public Module
{
 public:
	CSModes(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSOp(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSDeOp(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSVoice(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSDeVoice(), MOD_UNIQUE);

		if (ircd->halfop)
		{
			this->AddCommand(CHANSERV, new CommandCSHalfOp(), MOD_UNIQUE);
			this->AddCommand(CHANSERV, new CommandCSDeHalfOp(), MOD_UNIQUE);
		}

		if (ircd->protect)
		{
			this->AddCommand(CHANSERV, new CommandCSProtect(), MOD_UNIQUE);
			this->AddCommand(CHANSERV, new CommandCSDeProtect(), MOD_UNIQUE);
		}

		if (ircd->admin)
		{
			this->AddCommand(CHANSERV, new CommandCSProtect(), MOD_UNIQUE);
			this->AddCommand(CHANSERV, new CommandCSDeProtect(), MOD_UNIQUE);
		}

		if (ircd->owner)
		{
			this->AddCommand(CHANSERV, new CommandCSOwner(), MOD_UNIQUE);
			this->AddCommand(CHANSERV, new CommandCSDeOwner(), MOD_UNIQUE);
		}

		this->SetChanHelp(myChanServHelp);
	}
};


MODULE_INIT("cs_modes", CSModes)
