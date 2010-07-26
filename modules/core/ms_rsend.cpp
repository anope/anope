/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandMSRSend : public Command
{
 public:
	CommandMSRSend() : Command("RSEND", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string text = params[1];
		NickAlias *na = NULL;

		/* prevent user from rsend to themselves */
		if ((na = findnick(nick)) && na->nc == u->Account())
		{
			notice_lang(Config.s_MemoServ, u, MEMO_NO_RSEND_SELF);
			return MOD_CONT;
		}

		if (Config.MSMemoReceipt == 1)
		{
			/* Services opers and above can use rsend */
			if (u->Account()->IsServicesOper())
				memo_send(u, nick, text, 3);
			else
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
		}
		else if (Config.MSMemoReceipt == 2)
			/* Everybody can use rsend */
			memo_send(u, nick, text, 3);
		else
		{
			/* rsend has been disabled */
			Alog(LOG_DEBUG) << "MSMemoReceipt is set misconfigured to " << Config.MSMemoReceipt;
			notice_lang(Config.s_MemoServ, u, MEMO_RSEND_DISABLED);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_RSEND);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "RSEND", MEMO_RSEND_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_RSEND);
	}
};

class MSRSend : public Module
{
 public:
	MSRSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!Config.MSMemoReceipt)
			throw ModuleException("Don't like memo reciepts, or something.");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, new CommandMSRSend());
	}
};

MODULE_INIT(MSRSend)
