/* ChanServ core functions
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
#include "modules/cs_info.h"

class CommandCSInfo : public Command
{
	EventHandlers<Event::ChanInfo> &eventonchaninfo;

 public:
	CommandCSInfo(Module *creator, EventHandlers<Event::ChanInfo> &event) : Command(creator, "chanserv/info", 1, 2), eventonchaninfo(event)
	{
		this->SetDesc(_("Lists information about the named registered channel"));
		this->SetSyntax(_("\037channel\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		NickServ::Account *nc = source.nc;
		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		bool has_auspex = source.HasPriv("chanserv/auspex");
		bool show_all = false;

		/* Should we show all fields? Only for sadmins and identified users */
		if (source.AccessFor(ci).HasPriv("INFO") || has_auspex)
			show_all = true;

		InfoFormatter info(nc);

		source.Reply(_("Information for channel \002{0}\002:"), ci->GetName());
		if (ci->GetFounder())
			info[_("Founder")] = ci->GetFounder()->GetDisplay();

		if (show_all && ci->GetSuccessor())
			info[_("Successor")] = ci->GetSuccessor()->GetDisplay();

		if (!ci->GetDesc().empty())
			info[_("Description")] = ci->GetDesc();

		info[_("Registered")] = Anope::strftime(ci->GetTimeRegistered(), source.GetAccount());
		info[_("Last used")] = Anope::strftime(ci->GetLastUsed(), source.GetAccount());

		if (show_all)
		{
			info[_("Ban type")] = stringify(ci->GetBanType());
		}

		this->eventonchaninfo(&Event::ChanInfo::OnChanInfo, source, ci, info, show_all);

		std::vector<Anope::string> replies;
		info.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Shows information about \037channel\037, including its founder, description, time of registration, and last time used."
		               " If the user issuing the command has \002{0}\002 privilege on \037channel\037, then the successor, last topic set, settings and expiration time will also be displayed when applicable."));
		return true;
	}
};

class CSInfo : public Module
{
	CommandCSInfo commandcsinfo;
	EventHandlers<Event::ChanInfo> eventonchaninfo;

 public:
	CSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsinfo(this, eventonchaninfo)
		, eventonchaninfo(this)
	{

	}
};

MODULE_INIT(CSInfo)
