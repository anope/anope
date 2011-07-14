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
#include "memoserv.h"

class CommandMSSendAll : public Command
{
 public:
	CommandMSSendAll(Module *creator) : Command(creator, "memoserv/sendall", 1, 1, "memoserv/sendall")
	{
		this->SetDesc(_("Send a memo to all registered users"));
		this->SetSyntax(_("\037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &text = params[0];

		if (readonly)
		{
			source.Reply(MEMO_SEND_DISABLED);
			return;
		}

		NickAlias *na = findnick(u->nick);

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if ((na && na->nc == nc) || !nc->display.equals_ci(u->nick))
				memoserv->Send(u->nick, nc->display, text);
		}

		source.Reply(_("A massmemo has been sent to all registered users."));
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends all registered users a memo containing \037memo-text\037."));
		return true;
	}
};

class MSSendAll : public Module
{
	CommandMSSendAll commandmssendall;

 public:
	MSSendAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmssendall(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmssendall);
	}
};

MODULE_INIT(MSSendAll)
