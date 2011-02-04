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

class CommandMSSend : public Command
{
 public:
	CommandMSSend() : Command("SEND", 2, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];
		memo_send(source, nick, text, 0);
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SEND {\037nick\037 | \037channel\037} \037memo-text\037\002\n"
				" \n"
				"Sends the named \037nick\037 or \037channel\037 a memo containing\n"
				"\037memo-text\037. When sending to a nickname, the recipient will\n"
				"receive a notice that he/she has a new memo. The target\n"
				"nickname/channel must be registered."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SEND", LanguageString::MEMO_SEND_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SEND   Send a memo to a nick or channel"));
	}
};

class MSSend : public Module
{
	CommandMSSend commandmssend;

 public:
	MSSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmssend);
	}
};

MODULE_INIT(MSSend)
