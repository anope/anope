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

class CommandMSStaff : public Command
{
 public:
	CommandMSStaff(Module *creator) : Command(creator, "memoserv/staff", 1, 1)
	{
		this->SetDesc(_("Send a memo to all opers/admins"));
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

			if (source.nc != nc && nc->IsServicesOper())
				MemoServService->Send(source.GetNick(), nc->display, text, true);
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends all services staff a memo containing \037memo-text\037."));

		return true;
	}
};

class MSStaff : public Module
{
	CommandMSStaff commandmsstaff;

 public:
	MSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmsstaff(this)
	{

		if (!MemoServService)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSStaff)
