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

class CommandMSStaff : public Command
{
 public:
	CommandMSStaff() : Command("STAFF", 1, 1, "memoserv/staff")
	{
		this->SetDesc(_("Send a memo to all opers/admins"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &text = params[0];

		if (readonly)
		{
			source.Reply(_(MEMO_SEND_DISABLED));
			return MOD_CONT;
		}

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (nc->IsServicesOper())
				memo_send(source, nc->display, text, 0);
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STAFF\002 \002memo-text\002\n"
			" \n"
			"Sends all services staff a memo containing \037memo-text\037."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "STAFF", _("STAFF \037memo-text\037"));
	}
};

class MSStaff : public Module
{
	CommandMSStaff commandmsstaff;

 public:
	MSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsstaff);
	}
};

MODULE_INIT(MSStaff)
