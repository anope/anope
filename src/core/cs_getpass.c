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

class CommandCSGetPass : public Command
{
 public:
	CommandCSGetPass() : Command("GETPASS", 1, 1, "chanserv/getpass")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		char tmp_pass[PASSMAX];
		ChannelInfo *ci;

		ci = cs_findchan(chan);

		if (!enc_decrypt(ci->founderpass, tmp_pass, PASSMAX - 1))
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

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_GETPASS);
		return true;
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

		char tmp_pass[PASSMAX];
		if (!enc_decrypt("tmp", tmp_pass, PASSMAX - 1))
			throw ModuleException("Incompatible with the encryption module being used");
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_GETPASS);
	}
};

MODULE_INIT("cs_getpass", CSGetPass)
