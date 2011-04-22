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
	CommandMSSendAll() : Command("SENDALL", 1, 1, "memoserv/sendall")
	{
		this->SetDesc(_("Send a memo to all registered users"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &text = params[0];

		if (readonly)
		{
			source.Reply(_(MEMO_SEND_DISABLED));
			return MOD_CONT;
		}

		NickAlias *na = findnick(u->nick);

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if ((na && na->nc == nc) || !nc->display.equals_ci(u->nick))
				memoserv->Send(u->nick, nc->display, text);
		}

		source.Reply(_("A massmemo has been sent to all registered users."));
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SENDALL\002 \002memo-text\002\n \n"
				"Sends all registered users a memo containing \037memo-text\037."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SENDALL", _("SENDALL \037memo-text\037"));
	}
};

class MSSendAll : public Module
{
	CommandMSSendAll commandmssendall;

 public:
	MSSendAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!memoserv)
			throw ModuleException("MemoServ is not loaded!");

		this->AddCommand(memoserv->Bot(), &commandmssendall);
	}
};

MODULE_INIT(MSSendAll)
