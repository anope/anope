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

class CommandMSSend : public Command
{
 public:
	CommandMSSend(Module *creator) : Command(creator, "memoserv/send", 2, 2)
	{
		this->SetDesc(_("Send a memo to a nick or channel"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];

		MemoServService::MemoResult result = memoserv->Send(source.u->nick, nick, text);
		if (result == MemoServService::MEMO_SUCCESS)
			source.Reply(_("Memo sent to \002%s\002."), nick.c_str());
		else if (result == MemoServService::MEMO_INVALID_TARGET)
			source.Reply(_("\002%s\002 is not a registered unforbidden nick or channel."), nick.c_str());
		else if (result == MemoServService::MEMO_TOO_FAST)
			source.Reply(_("Please wait %d seconds before using the SEND command again."), Config->MSSendDelay);
		else if (result == MemoServService::MEMO_TARGET_FULL)
			source.Reply(_("%s currently has too many memos and cannot receive more."), nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends the named \037nick\037 or \037channel\037 a memo containing\n"
				"\037memo-text\037. When sending to a nickname, the recipient will\n"
				"receive a notice that he/she has a new memo. The target\n"
				"nickname/channel must be registered."));
		return true;
	}
};

class MSSend : public Module
{
	CommandMSSend commandmssend;

 public:
	MSSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmssend(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmssend);
	}
};

MODULE_INIT(MSSend)
