/*
 *
 * (C) 2003-2010 Anope Team
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
 private:
	std::string Desc;

 protected:
	CommandReturn RealExecute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		nc->Shrink("nickserv:" + std::string(this->name.c_str()));
		if (params.size() > 1)
		{
			nc->Extend("nickserv:" + std::string(this->name.c_str()), new ExtensibleItemRegular<ci::string>(params[1]));
			notice_lang(Config.s_NickServ, u, CHAN_SETTING_CHANGED, this->name.c_str(), nc->display, params[1].c_str());
		}
		else
		{
			notice_lang(Config.s_NickServ, u, CHAN_SETTING_UNSET, this->name.c_str(), nc->display);
		}

		return MOD_CONT;
	}

 public:
	CommandNSSetMisc(const ci::string &cname, const std::string &desc, const ci::string &cpermission = "") : Command(cname, 0, 1, cpermission), Desc(desc)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		std::vector<ci::string> realparams = params;
		realparams.insert(realparams.begin(), u->Account()->display);
		return RealExecute(u, realparams);
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(Config.s_NickServ, "    %-10s    %s", this->name.c_str(), this->Desc.c_str());
	}
};

class CommandNSSASetMisc : public CommandNSSetMisc
{
 public:
	CommandNSSASetMisc(const ci::string &cname, const std::string &desc) : CommandNSSetMisc(cname, desc, "nickserv/saset/" + cname)
	{
		this->MinParams = 1;
		this->MaxParams = 2;
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return RealExecute(u, params);
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}
};

class NSSetMisc : public Module
{
	struct CommandInfo
	{
		std::string Name;
		std::string Desc;
		bool ShowHidden;

		CommandInfo(const std::string &name, const std::string &desc, bool showhidden) : Name(name), Desc(desc), ShowHidden(showhidden) { }
	};

	std::map<std::string, CommandInfo *> Commands;

	void RemoveAll()
	{
		if (Commands.empty())
			return;

		Command *set = FindCommand(NickServ, "SET");
		Command *saset = FindCommand(NickServ, "SASET");

		if (!set && !saset)
			return;

		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (set)
				set->DelSubcommand(it->first.c_str());
			if (saset)
				saset->DelSubcommand(it->first.c_str());
			delete it->second;
		}

		this->Commands.clear();
	}

 public:
	NSSetMisc(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Implementation i[] = { I_OnReload, I_OnNickInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, 4);

		OnReload(true);
	}

	~NSSetMisc()
	{
		RemoveAll();
	}

	void OnReload(bool)
	{
		RemoveAll();

		Command *set = FindCommand(NickServ, "SET");
		Command *saset = FindCommand(NickServ, "SASET");
		if (!set && !saset)
			return;

		ConfigReader config;

		for (int i = 0; true; ++i)
		{
			std::string cname = config.ReadValue("ns_set_misc", "name", "", i);
			if (cname.empty())
				break;
			std::string desc = config.ReadValue("ns_set_misc", "desc", "", i);
			bool showhidden = config.ReadFlag("ns_set_misc", "operonly", "no", i);

			CommandInfo *info = new CommandInfo(cname, desc, showhidden);
			if (!this->Commands.insert(std::make_pair(cname, info)).second)
			{
				Alog() << "ns_set_misc: Warning, unable to add duplicate entry " << cname;
				delete info;
				continue;
			}

			if (set)
				set->AddSubcommand(new CommandNSSetMisc(cname.c_str(), desc));
			if (saset)
				saset->AddSubcommand(new CommandNSSASetMisc(cname.c_str(), desc));
		}
	}

	void OnNickInfo(User *u, NickAlias *na, bool ShowHidden)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (!ShowHidden && it->second->ShowHidden)
				continue;

			ci::string value;
			if (na->nc->GetExtRegular("nickserv:" + it->first, value))
			{
				u->SendMessage(Config.s_NickServ, "      %s: %s", it->first.c_str(), value.c_str());
			}
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), NickCore *nc)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			ci::string value;

			if (nc->GetExtRegular("nickserv:" + it->first, value))
			{
				WriteMetadata(it->first, ":" + std::string(value.c_str()));
			}
		}
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const std::string &key, const std::vector<std::string> &params)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (key == it->first)
			{
				nc->Extend("nickserv:" + it->first, new ExtensibleItemRegular<ci::string>(params[0].c_str()));
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSSetMisc)
