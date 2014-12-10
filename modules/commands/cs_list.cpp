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
#include "modules/cs_set.h"
#include "modules/cs_mode.h"

class CommandCSList : public Command
{
 public:
	CommandCSList(Module *creator) : Command(creator, "chanserv/list", 1, 2)
	{
		this->SetDesc(_("Lists all registered channels matching the given pattern"));
		this->SetSyntax(_("\037pattern\037 [SUSPENDED] [NOEXPIRE]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string pattern = params[0];
		unsigned nchans;
		bool is_servadmin = source.HasCommand("chanserv/list");
		int count = 0, from = 0, to = 0;
		bool suspended = false, channoexpire = false;

		if (pattern[0] == '#')
		{
			Anope::string n1, n2;
			sepstream(pattern.substr(1), '-').GetToken(n1, 0);
			sepstream(pattern, '-').GetToken(n2, 1);

			try
			{
				from = convertTo<int>(n1);
				to = convertTo<int>(n2);
			}
			catch (const ConvertException &)
			{
				source.Reply(_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002."));
				source.Reply(_("To search for channels starting with #, search for the channel name without the #-sign prepended (\002anope\002 instead of \002#anope\002)."));
				return;
			}

			pattern = "*";
		}

		nchans = 0;

		if (is_servadmin && params.size() > 1)
		{
			Anope::string keyword;
			spacesepstream keywords(params[1]);
			while (keywords.GetToken(keyword))
			{
				if (keyword.equals_ci("SUSPENDED"))
					suspended = true;
				if (keyword.equals_ci("NOEXPIRE"))
					channoexpire = true;
			}
		}

		Anope::string spattern = "#" + pattern;
		unsigned listmax = Config->GetModule(this->owner)->Get<unsigned>("listmax", "50");

		source.Reply(_("List of entries matching \002{0}\002:"), pattern);

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Name")).AddColumn(_("Description"));

		// XXX wtf
		Anope::map<ChanServ::Channel *> ordered_map;
		if (ChanServ::service)
			for (auto& it : ChanServ::service->GetChannels())
				ordered_map[it.first] = it.second;

		for (Anope::map<ChanServ::Channel *>::const_iterator it = ordered_map.begin(), it_end = ordered_map.end(); it != it_end; ++it)
		{
			ChanServ::Channel *ci = it->second;

			if (!is_servadmin)
			{
				if (ci->HasFieldS("CS_PRIVATE") || ci->HasFieldS("CS_SUSPENDED"))
					continue;
				if (ci->c && ci->c->HasMode("SECRET"))
					continue;

				if (mlocks)
				{
					ModeLock *secret = mlocks->GetMLock(ci, "SECRET");
					if (secret && secret->GetSet())
						continue;
				}
			}

			if (suspended && !ci->HasFieldS("CS_SUSPENDED"))
				continue;

			if (channoexpire && !ci->HasFieldS("CS_NO_EXPIRE"))
				continue;

			if (pattern.equals_ci(ci->GetName()) || ci->GetName().equals_ci(spattern) || Anope::Match(ci->GetName(), pattern, false, true) || Anope::Match(ci->GetName(), spattern, false, true) || Anope::Match(ci->GetDesc(), pattern, false, true) || Anope::Match(ci->GetLastTopic(), pattern, false, true))
			{
				if (((count + 1 >= from && count + 1 <= to) || (!from && !to)) && ++nchans <= listmax)
				{
					bool isnoexpire = false;
					if (is_servadmin && (ci->HasFieldS("CS_NO_EXPIRE")))
						isnoexpire = true;

					ListFormatter::ListEntry entry;
					entry["Name"] = (isnoexpire ? "!" : "") + ci->GetName();
					if (ci->HasFieldS("CS_SUSPENDED"))
						entry["Description"] = Language::Translate(source.GetAccount(), _("[Suspended]"));
					else
						entry["Description"] = ci->GetDesc();
					list.AddEntry(entry);
				}
				++count;
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of list - \002{0}/{1}\002 matches shown."), nchans > listmax ? listmax : nchans, nchans);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists all registered channels matching the given pattern."
		               " Channels with the \002PRIVATE\002 option set will only be displayed to Services Operators with the proper access."
		               " Channels with the \002NOEXPIRE\002 option set will have a \002!\002 prefixed to the channel for Services Operators to see."
		               "\n"
		               "Note that a preceding '#' specifies a range, channel names are to be written without '#'.\n"
		               "\n"
		               "If the SUSPENDED or NOEXPIRE options are given, only channels which, respectively, are SUSPENDED or have the NOEXPIRE flag set will be displayed."
		               " If multiple options are given, all channels matching at least one option will be displayed."
		               " Note that these options are limited to \037Services Operators\037.\n"
		               "\n"
		               "Examples:\n"
		               "\n"
		               "         {0} *anope*\n"
		               "         Lists all registered channels with \002anope\002 in their names (case insensitive).\n"
		               "\n"
		               "         {0} * NOEXPIRE\n"
		               "         Lists all registered channels which have been set to not expire.\n"
		               "\n"
		               "         {0} #51-100\n"
		               "         Lists all registered channels within the given range (51-100)."));

		if (!Config->GetBlock("options")->Get<const Anope::string>("regexengine").empty())
		{
			source.Reply(" ");
			source.Reply(_("Regex matches are also supported using the {0} engine. Enclose your pattern in // if this is desired."), Config->GetBlock("options")->Get<const Anope::string>("regexengine"));
		}

		return true;
	}
};

class CommandCSSetPrivate : public Command
{
 public:
	CommandCSSetPrivate(Module *creator, const Anope::string &cname = "chanserv/set/private") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Hide channel from the LIST command"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable private";
			ci->SetS<bool>("CS_PRIVATE", true);
			source.Reply(_("Private option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable private";
			ci->UnsetS<bool>("CS_PRIVATE");
			source.Reply(_("Private option for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002private\002 option for a channel."));

		ServiceBot *bi;
		Anope::string cmd;
		if (Command::FindCommandFromService("chanserv/list", bi, cmd))
			source.Reply(_("When \002private\002 is set, the channel will not appear in the \002{0}\002 command of \002{1}\002."), cmd, bi->nick);
		return true;
	}
};

class CSList : public Module
	, public EventHook<Event::ChanInfo>
{
	CommandCSList commandcslist;
	CommandCSSetPrivate commandcssetprivate;

	ExtensibleItem<bool> priv; // XXX

 public:
	CSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanInfo>()
		, commandcslist(this)
		, commandcssetprivate(this)
		, priv(this, "CS_PRIVATE")
	{
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		if (priv.HasExt(ci))
			info.AddOption(_("Private"));
	}
};

MODULE_INIT(CSList)
