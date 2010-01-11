/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
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

class CommandMSRSend : public Command
{
 public:
	CommandMSRSend() : Command("RSEND", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *text = params[1].c_str();
		NickAlias *na = NULL;
		int z = 3;

		/* prevent user from rsend to themselves */
		if ((na = findnick(nick)))
		{
			if (na->nc == u->nc)
			{
				notice_lang(Config.s_MemoServ, u, MEMO_NO_RSEND_SELF);
				return MOD_CONT;
			}
			else
			{
				notice_lang(Config.s_MemoServ, u, NICK_X_NOT_REGISTERED, nick);
				return MOD_CONT;
			}
		}

		if (Config.MSMemoReceipt == 1)
		{
			/* Services opers and above can use rsend */
			if (u->nc->IsServicesOper())
				memo_send(u, nick, text, z);
			else
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
		}
		else if (Config.MSMemoReceipt == 2)
			/* Everybody can use rsend */
			memo_send(u, nick, text, z);
		else
		{
			/* rsend has been disabled */
			if (debug)
				alog("debug: MSMemoReceipt is set misconfigured to %d", Config.MSMemoReceipt);
			notice_lang(Config.s_MemoServ, u, MEMO_RSEND_DISABLED);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_RSEND);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "RSEND", MEMO_RSEND_SYNTAX);
	}
};

class MSRSend : public Module
{
 public:
	MSRSend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSRSend());

		if (!Config.MSMemoReceipt)
			throw ModuleException("Don't like memo reciepts, or something.");

		ModuleManager::Attach(I_OnMemoServHelp, this);
	}
	void OnMemoServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_RSEND);
	}
};

MODULE_INIT(MSRSend)
