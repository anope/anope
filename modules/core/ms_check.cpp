/* MemoServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &recipient = params[0];

		bool found = false;

		NickAlias *na = findnick(recipient);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, recipient.c_str());
			return MOD_CONT;
		}

		if (na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(NICK_X_FORBIDDEN, recipient.c_str());
			return MOD_CONT;
		}

		MemoInfo *mi = &na->nc->memos;

		/* Okay, I know this looks strange but we wanna get the LAST memo, so we
			have to loop backwards */

		for (int i = mi->memos.size() - 1; i >= 0; --i)
		{
			if (u->Account()->display.equals_ci(mi->memos[i]->sender))
			{
				found = true; /* Yes, we've found the memo */

				if (mi->memos[i]->HasFlag(MF_UNREAD))
					source.Reply(MEMO_CHECK_NOT_READ, na->nick.c_str(), do_strftime(mi->memos[i]->time).c_str());
				else
					source.Reply(MEMO_CHECK_READ, na->nick.c_str(), do_strftime(mi->memos[i]->time).c_str());
				break;
			}
		}

		if (!found)
			source.Reply(MEMO_CHECK_NO_MEMO, na->nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(MEMO_HELP_CHECK);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CHECK", MEMO_CHECK_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(MEMO_HELP_CMD_CHECK);
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
