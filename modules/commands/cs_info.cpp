/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSInfo : public Command
{
 public:
	CommandCSInfo(Module *creator) : Command(creator, "chanserv/info", 1, 2)
	{
		this->SetDesc(_("Lists information about the named registered channel"));
		this->SetSyntax(_("\037channel\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];

		NickCore *nc = source.nc;
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool has_auspex = source.HasPriv("chanserv/auspex");
		bool show_all = false;

		/* Should we show all fields? Only for sadmins and identified users */
		if (has_auspex || source.AccessFor(ci).HasPriv("INFO"))
			show_all = true;

		InfoFormatter info(nc);

		source.Reply(CHAN_INFO_HEADER, chan.c_str());
		if (ci->GetFounder())
			info["Founder"] = ci->GetFounder()->display;

		if (show_all && ci->GetSuccessor())
			info["Successor"] = ci->GetSuccessor()->display;

		if (!ci->desc.empty())
			info["Description"] = ci->desc;

		info["Registered"] = Anope::strftime(ci->time_registered);
		info["Last used"] = Anope::strftime(ci->last_used);

		if (show_all)
		{
			info["Ban type"] = stringify(ci->bantype);
		}

		FOREACH_MOD(OnChanInfo, (source, ci, info, show_all));

		std::vector<Anope::string> replies;
		info.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists information about the named registered channel,\n"
				"including its founder, time of registration, and last\n"
				"time used. If the user issuing the command has the\n"
				"appropriate access for it, then the description, successor,\n"
				"last topic set, settings and expiration time will also\n"
				"be displayed when applicable."));
		return true;
	}
};

class CSInfo : public Module
{
	CommandCSInfo commandcsinfo;

 public:
	CSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsinfo(this)
	{

	}
};

MODULE_INIT(CSInfo)
