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

class CommandMSCheck : public Command
{
 public:
	CommandMSCheck() : Command("CHECK", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na = NULL;
		MemoInfo *mi = NULL;
		int i, found = 0;
		Anope::string recipient = params[0];
		struct tm *tm;
		char timebuf[64];

		if (!u->IsRecognized())
		{
			notice_lang(Config.s_MemoServ, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ.c_str());
			return MOD_CONT;
		}
		else if (!(na = findnick(recipient)))
		{
			notice_lang(Config.s_MemoServ, u, NICK_X_NOT_REGISTERED, recipient.c_str());
			return MOD_CONT;
		}

		if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config.s_MemoServ, u, NICK_X_FORBIDDEN, recipient.c_str());
			return MOD_CONT;
		}

		mi = &na->nc->memos;

		/* Okay, I know this looks strange but we wanna get the LAST memo, so we
			have to loop backwards */

		for (i = mi->memos.size() - 1; i >= 0; --i)
		{
			if (u->Account()->display.equals_ci(mi->memos[i]->sender))
			{
				found = 1; /* Yes, we've found the memo */

				tm = localtime(&mi->memos[i]->time);
				strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);

				if (mi->memos[i]->HasFlag(MF_UNREAD))
					notice_lang(Config.s_MemoServ, u, MEMO_CHECK_NOT_READ, na->nick.c_str(), timebuf);
				else
					notice_lang(Config.s_MemoServ, u, MEMO_CHECK_READ, na->nick.c_str(), timebuf);
				break;
			}
		}

		if (!found)
			notice_lang(Config.s_MemoServ, u, MEMO_CHECK_NO_MEMO, na->nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_CHECK);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "CHECK", MEMO_CHECK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_CHECK);
	}
};

class MSCheck : public Module
{
	CommandMSCheck commandmscheck;

 public:
	MSCheck(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmscheck);
	}
};

MODULE_INIT(MSCheck)
