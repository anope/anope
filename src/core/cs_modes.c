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

/* do_util: not a command, but does the job of other */

static CommandReturn do_util(User *u, CSModeUtil *util, const char *chan, const char *nick)
{
	const char *av[2];
	Channel *c;
	ChannelInfo *ci;
	User *u2;

	int is_same;

	if (!nick) {
		nick = u->nick;
	}

	is_same = (nick == u->nick) ? 1 : (stricmp(nick, u->nick) == 0);

	if (!(c = findchan(chan))) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
	} else if (!(ci = c->ci)) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
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

// XXX: Future enhancement. Default these to the sender, with an optional target arg.

class CommandCSOp : public Command
{
 public:
	CommandCSOp() : Command("OP", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_OP], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_OP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "OP", CHAN_OP_SYNTAX);
	}
};


class CommandCSDeOp : public Command
{
 public:
	CommandCSDeOp() : Command("DEOP", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_DEOP], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_DEOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DEOP", CHAN_DEOP_SYNTAX);
	}
};


class CommandCSVoice : public Command
{
 public:
	CommandCSVoice() : Command("VOICE", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_VOICE], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_VOICE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "VOICE", CHAN_VOICE_SYNTAX);
	}
};


class CommandCSDeVoice : public Command
{
 public:
	CommandCSDeVoice() : Command("DEVOICE", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_DEVOICE], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_DEVOICE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DEVOICE", CHAN_DEVOICE_SYNTAX);
	}
};


class CommandCSHalfOp : public Command
{
 public:
	CommandCSHalfOp() : Command("HALFOP", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->halfop)
		{
			return MOD_CONT;

		}

		return do_util(u, &csmodeutils[MUT_HALFOP], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_HALFOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "HALFOP", CHAN_HALFOP_SYNTAX);
	}
};


class CommandCSDeHalfOp : public Command
{
 public:
	CommandCSDeHalfOp() : Command("DEHALFOP", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->halfop)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_DEHALFOP], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_DEHALFOP);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DEHALFOP", CHAN_DEHALFOP_SYNTAX);
	}
};


class CommandCSProtect : public Command
{
 public:
	CommandCSProtect() : Command("PROTECT", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->protect && !ircd->admin)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_PROTECT], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_PROTECT);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "PROTECT", CHAN_PROTECT_SYNTAX);
	}
};

/*************************************************************************/

class CommandCSDeProtect : public Command
{
 public:
	CommandCSDeProtect() : Command("DEPROTECT", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!ircd->protect && !ircd->admin)
		{
			return MOD_CONT;
		}

		return do_util(u, &csmodeutils[MUT_DEPROTECT], (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL));
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_DEPROTECT);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DEPROTECT", CHAN_DEPROTECT_SYNTAX);
	}
};

/*************************************************************************/

class CommandCSOwner : public Command
{
 public:
	CommandCSOwner() : Command("OWNER", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_OWNER], (params.size() > 0 ? params[0].c_str() : NULL), NULL);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_OWNER);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "OWNER", CHAN_OWNER_SYNTAX);
	}
};

/*************************************************************************/

class CommandCSDeOwner : public Command
{
 public:
	CommandCSDeOwner() : Command("DEOWNER", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return do_util(u, &csmodeutils[MUT_DEOWNER], (params.size() > 0 ? params[0].c_str() : NULL), NULL);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_DEOWNER);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DEOWNER", CHAN_DEOWNER_SYNTAX);
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
	}
	void ChanServHelp(User *u)
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
};


MODULE_INIT("cs_modes", CSModes)
