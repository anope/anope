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
	if (is_services_admin(u)) {
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_GETPASS);
	}
}

class CommandCSGetPass : public Command
{
 public:
	CommandCSGetPass() : Command("GETPASS", 1, 1)
	{
		this->has_priv = is_services_admin;
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		char tmp_pass[PASSMAX];
		ChannelInfo *ci;

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}
		if (ci->flags & CI_VERBOTEN)
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}
		if (CSRestrictGetPass && !is_services_root(u))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}
		if(!enc_decrypt(ci->founderpass, tmp_pass, PASSMAX - 1)==1)
		{
			notice_lang(s_ChanServ, u, CHAN_GETPASS_UNAVAILABLE);
			return MOD_CONT;
		}

		alog("%s: %s!%s@%s used GETPASS on %s", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ci->name);
		if (WallGetpass)
		{
			ircdproto->SendGlobops(s_ChanServ, "\2%s\2 used GETPASS on channel \2%s\2", u->nick, chan);
		}
		notice_lang(s_ChanServ, u, CHAN_GETPASS_PASSWORD_IS, chan, tmp_pass);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (is_services_admin(u))
		{
			notice_lang(s_ChanServ, u, CHAN_SERVADMIN_HELP_GETPASS);
			return true;
		}

		return false;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "GETPASS",  CHAN_GETPASS_SYNTAX);
	}
};

class CSGetPass : public Module
{
 public:
	CSGetPass(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSGetPass(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

MODULE_INIT("cs_getpass", CSGetPass)
