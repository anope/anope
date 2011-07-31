/*
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

class CommandCSSetMisc : public Command
{
 public:
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc", const Anope::string &cpermission = "") : Command(creator, cname, 1, 2, cpermission)
	{
		this->SetSyntax(_("\037channel\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		ci->Shrink("cs_set_misc:" + source.command.replace_all_cs(" ", "_"));
		if (params.size() > 1)
		{
			ci->Extend("cs_set_misc:" + source.command.replace_all_cs(" ", "_"), new ExtensibleItemRegular<Anope::string>(params[1]));
			source.Reply(CHAN_SETTING_CHANGED, source.command.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, source.command.c_str(), ci->name.c_str());
	}
};

class CommandCSSASetMisc : public CommandCSSetMisc
{
 public:
	CommandCSSASetMisc(Module *creator) : CommandCSSetMisc(creator, "chanserv/saset/misc", "chanserv/saset/misc")
	{
	}
};

class CSSetMisc : public Module
{
	CommandCSSetMisc commandcssetmisc;
	CommandCSSASetMisc commandcssasetmisc;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED),
		commandcssetmisc(this), commandcssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnChanInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		ModuleManager::RegisterService(&this->commandcssetmisc);
		ModuleManager::RegisterService(&this->commandcssasetmisc);
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, bool ShowHidden)
	{
		std::deque<Anope::string> list;
		ci->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("cs_set_misc:") != 0)
				continue;
			
			Anope::string value;
			if (ci->GetExtRegular(list[i], value))
				source.Reply("      %s: %s", list[i].substr(12).replace_all_cs("_", " ").c_str(), value.c_str());
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), ChannelInfo *ci)
	{
		std::deque<Anope::string> list;
		ci->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("cs_set_misc:") != 0)
				continue;

			Anope::string value;
			if (ci->GetExtRegular(list[i], value))
				WriteMetadata(list[i], ":" + value);
		}
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.find("cs_set_misc:") == 0)
			ci->Extend(key, new ExtensibleItemRegular<Anope::string>(params[0]));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSetMisc)
