/* MemoServ core functions
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

void myMemoServHelp(User *u);

class CommandMSRSend : public Command
{
 public:
	CommandMSRSend() : Command("RSEND", 2, 2)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		const char *name = params[0].c_str();
		const char *text = params[1].c_str();
		NickAlias *na = NULL;
		int z = 3;

		/* prevent user from rsend to themselves */
		if ((na = findnick(name)))
		{
			if (u->na)
			{
				if (!stricmp(na->nc->display, u->na->nc->display))
				{
					notice_lang(s_MemoServ, u, MEMO_NO_RSEND_SELF);
					return MOD_CONT;
				}
			}
			else
			{
				notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, name);
				return MOD_CONT;
			}
		}

		if (MSMemoReceipt == 1)
		{
			/* Services opers and above can use rsend */
			if (is_services_oper(u))
				memo_send(u, name, text, z);
			else
				notice_lang(s_MemoServ, u, ACCESS_DENIED);
		}
		else if (MSMemoReceipt == 2)
			/* Everybody can use rsend */
			memo_send(u, name, text, z);
		else
		{
			/* rsend has been disabled */
			if (debug)
				alog("debug: MSMemoReceipt is set misconfigured to %d", MSMemoReceipt);
			notice_lang(s_MemoServ, u, MEMO_RSEND_DISABLED);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_RSEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "RSEND", MEMO_RSEND_SYNTAX);
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
		this->AddCommand(MEMOSERV, new CommandMSRSend(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);

		if (!MSMemoReceipt)
			throw ModuleException("Don't like memo reciepts, or something.");
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	if ((MSMemoReceipt == 1 && is_services_oper(u)) || MSMemoReceipt == 2)
		notice_lang(s_MemoServ, u, MEMO_HELP_CMD_RSEND);
}

MODULE_INIT("ms_rsend", MSRSend)
