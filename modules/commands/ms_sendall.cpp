/* MemoServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/memoserv.h"

class CommandMSSendAll : public Command
{
 public:
	CommandMSSendAll(Module *creator) : Command(creator, "memoserv/sendall", 1, 1)
	{
		this->SetDesc(_("Send a memo to all registered users"));
		this->SetSyntax(_("\037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!MemoServ::service)
			return;

		const Anope::string &text = params[0];

		Log(LOG_ADMIN, source, this) << "to send " << text;

		for (NickServ::Account *nc : NickServ::service->GetAccountList())
			if (nc != source.nc)
				MemoServ::service->Send(source.GetNick(), nc->GetDisplay(), text);

		source.Reply(_("A mass memo has been sent to all registered users."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sends all registered users a memo containing \037memo-text\037."));
		return true;
	}
};

class MSSendAll : public Module
{
	CommandMSSendAll commandmssendall;

 public:
	MSSendAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmssendall(this)
	{
		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSSendAll)
