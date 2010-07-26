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

class CommandMSStaff : public Command
{
 public:
	CommandMSStaff() : Command("STAFF", 1, 1, "memoserv/staff")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string text = params[0];

		if (readonly)
		{
			notice_lang(Config.s_MemoServ, u, MEMO_SEND_DISABLED);
			return MOD_CONT;
		}

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (nc->IsServicesOper())
				memo_send(u, nc->display, text, 0);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_STAFF);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "STAFF", MEMO_STAFF_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_STAFF);
	}
};

class MSStaff : public Module
{
 public:
	MSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, new CommandMSStaff());
	}
};

MODULE_INIT(MSStaff)
