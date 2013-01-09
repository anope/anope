/* MemoServ core functions
 *
 * (C) 2003-2013 Anope Team
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

static ServiceReference<MemoServService> MemoServService("MemoServService", "MemoServ");

class CommandMSSendAll : public Command
{
 public:
	CommandMSSendAll(Module *creator) : Command(creator, "memoserv/sendall", 1, 1)
	{
		this->SetDesc(_("Send a memo to all registered users"));
		this->SetSyntax(_("\037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!MemoServService)
			return;

		const Anope::string &text = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(MEMO_SEND_DISABLED);
			return;
		}

		for (nickcore_map::const_iterator it = NickCoreList->begin(), it_end = NickCoreList->end(); it != it_end; ++it)
		{
			const NickCore *nc = it->second;

			if (nc != source.nc)
				MemoServService->Send(source.GetNick(), nc->display, text);
		}

		source.Reply(_("A massmemo has been sent to all registered users."));
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
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

		if (!MemoServService)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSSendAll)
