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
	CommandNSSetMisc(Module *creator, const Anope::string &cname, const Anope::string &cdesc, const Anope::string &cpermission = "", size_t min = 1) : Command(owner, "nickserv/set/" + cname, min, min + 1, cpermission)
	{
		this->SetDesc(cdesc);
		this->SetSyntax(_("\037parameter\037"));
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

		nc->Shrink("nickserv:" + this->name);
		if (!param.empty())
		{
			nc->Extend("nickserv:" + this->name, new ExtensibleItemRegular<Anope::string>(param));
			source.Reply(CHAN_SETTING_CHANGED, this->name.c_str(), nc->display.c_str(), param.c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, this->name.c_str(), nc->display.c_str());

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, source.u->Account()->display, params[0]);
	}
};

class CommandNSSASetMisc : public CommandNSSetMisc
{
 public:
	CommandNSSASetMisc(Module *creator, const Anope::string &cname, const Anope::string &cdesc) : CommandNSSetMisc(creator, "nickserv/saset/" + cname, cdesc, "nickserv/saset/" + cname, 2)
	{
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params[1]);
	}
};

class NSSetMisc : public Module
{
	struct CommandInfo
	{
		Anope::string Name;
		Anope::string Desc;
		bool ShowHidden;

		CommandInfo(const Anope::string &name, const Anope::string &cdesc, bool showhidden) : Name(name), Desc(cdesc), ShowHidden(showhidden) { }
	};

	std::map<Anope::string, CommandInfo *> Commands;

	void RemoveAll()
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
			delete it->second;
		this->Commands.clear();
	}

 public:
	NSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNickInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, 4);

		OnReload();
	}

	~NSSetMisc()
	{
		RemoveAll();
	}

	void OnReload()
	{
		RemoveAll();

		ConfigReader config;

		for (int i = 0, num = config.Enumerate("ns_set_misc"); i < num; ++i)
		{
			Anope::string cname = config.ReadValue("ns_set_misc", "name", "", i);
			if (cname.empty())
				continue;
			Anope::string desc = config.ReadValue("ns_set_misc", "desc", "", i);
			bool showhidden = config.ReadFlag("ns_set_misc", "privileged", "no", i);

			CommandInfo *info = new CommandInfo(cname, desc, showhidden);
			if (!this->Commands.insert(std::make_pair(cname, info)).second)
			{
				Log() << "ns_set_misc: Warning, unable to add duplicate entry " << cname;
				delete info;
				continue;
			}

			ModuleManager::RegisterService(new CommandNSSetMisc(this, cname, desc));
			ModuleManager::RegisterService(new CommandNSSASetMisc(this, cname, desc));
		}
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, bool ShowHidden)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (!ShowHidden && it->second->ShowHidden)
				continue;

			Anope::string value;
			if (na->nc->GetExtRegular("nickserv:" + it->first, value))
				source.Reply("      %s: %s", it->first.c_str(), value.c_str());
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickCore *nc)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			Anope::string value;
			if (nc->GetExtRegular("nickserv:" + it->first, value))
				WriteMetadata(it->first, ":" + value);
		}
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
			if (key == it->first)
				nc->Extend("nickserv:" + it->first, new ExtensibleItemRegular<Anope::string>(params[0]));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSSetMisc)
