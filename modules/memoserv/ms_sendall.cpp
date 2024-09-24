/* MemoServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

namespace
{
	ServiceReference<MemoServService> memoserv("MemoServService", "MemoServ");
}

class CommandMSSendAll final
	: public Command
{
public:
	CommandMSSendAll(Module *creator) : Command(creator, "memoserv/sendall", 1, 1)
	{
		this->SetDesc(_("Send a memo to all registered users"));
		this->SetSyntax(_("\037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!memoserv)
			return;

		const Anope::string &text = params[0];

		Log(LOG_ADMIN, source, this) << "to send " << text;

		for (const auto &[_, nc] : *NickCoreList)
		{
			if (nc != source.nc)
				memoserv->Send(source.GetNick(), nc->display, text);
		}

		source.Reply(_("A massmemo has been sent to all registered users."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends all registered users a memo containing \037memo-text\037."));
		return true;
	}
};

class MSSendAll final
	: public Module
{
	CommandMSSendAll commandmssendall;

public:
	MSSendAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmssendall(this)
	{
		if (!memoserv)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSSendAll)
