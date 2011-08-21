/*
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

class CommandNSSetMisc : public Command
{
 public:
	CommandNSSetMisc(Module *creator, const Anope::string &cname = "nickserv/set/misc", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetSyntax(_("[\037parameter\037]"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		nc->Shrink("ns_set_misc:" + source.command.replace_all_cs(" ", "_"));
		if (!param.empty())
		{
			nc->Extend("ns_set_misc:" + source.command.replace_all_cs(" ", "_"), new ExtensibleItemRegular<Anope::string>(param));
			source.Reply(CHAN_SETTING_CHANGED, source.command.c_str(), nc->display.c_str(), param.c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, source.command.c_str(), nc->display.c_str());

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, source.u->Account()->display, !params.empty() ? params[0] : "");
	}
};

class CommandNSSASetMisc : public CommandNSSetMisc
{
 public:
	CommandNSSASetMisc(Module *creator) : CommandNSSetMisc(creator, "nickserv/saset/misc", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 [\037parameter\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}
};

class NSSetMisc : public Module
{
	CommandNSSetMisc commandnssetmisc;
	CommandNSSASetMisc commandnssasetmisc;

 public:
	NSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetmisc(this), commandnssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		ModuleManager::RegisterService(&this->commandnssetmisc);
		ModuleManager::RegisterService(&this->commandnssasetmisc);
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, bool ShowHidden)
	{
		std::deque<Anope::string> list;
		na->nc->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("ns_set_misc:") != 0)
				continue;

			Anope::string value;
			if (na->nc->GetExtRegular(list[i], value))
				source.Reply("      %s: %s", list[i].substr(12).replace_all_cs("_", " ").c_str(), value.c_str());
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickCore *nc)
	{
		std::deque<Anope::string> list;
		nc->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("ns_set_misc:") != 0)
				continue;

			Anope::string value;
			if (nc->GetExtRegular(list[i], value))
				WriteMetadata(list[i], ":" + value);
		}
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key.find("ns_set_misc:") == 0)
			nc->Extend(key, new ExtensibleItemRegular<Anope::string>(params[0]));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSSetMisc)
