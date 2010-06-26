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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		int z = 0;
		const char *text = params[0].c_str();

		if (readonly)
		{
			notice_lang(Config.s_MemoServ, u, MEMO_SEND_DISABLED);
			return MOD_CONT;
		}

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (nc->IsServicesOper())
				memo_send(u, nc->display, text, z);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_STAFF);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
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
	MSStaff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(MemoServ, new CommandMSStaff());
	}
};

MODULE_INIT(MSStaff)
