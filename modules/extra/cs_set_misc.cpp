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

class CommandCSSetMisc : public Command
{
	std::string Desc;
 public:
	CommandCSSetMisc(const ci::string &cname, const std::string &desc, const ci::string &cpermission = "") : Command(cname, 1, 2, cpermission), Desc(desc)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		ci->Shrink("chanserv:" + std::string(this->name.c_str()));
		if (params.size() > 1)
		{
			ci->Extend("chanserv:" + std::string(this->name.c_str()), new ExtensibleItemRegular<ci::string>(params[1]));
			notice_lang(Config.s_ChanServ, u, CHAN_SETTING_CHANGED, this->name.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SETTING_UNSET, this->name.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(Config.s_ChanServ, "    %-10s    %s", this->name.c_str(), this->Desc.c_str());
	}
};

class CommandCSSASetMisc : public CommandCSSetMisc
{
 public:
	CommandCSSASetMisc(const ci::string &cname, const std::string &desc) : CommandCSSetMisc(cname, desc, "chanserv/saset/" + cname)
	{
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetMisc : public Module
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

		Command *set = FindCommand(ChanServ, "SET");
		Command *saset = FindCommand(ChanServ, "SASET");

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
	CSSetMisc(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Implementation i[] = { I_OnReload, I_OnChanInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, 4);

		OnReload(true);
	}

	~CSSetMisc()
	{
		RemoveAll();
	}

	void OnReload(bool)
	{
		RemoveAll();

		Command *set = FindCommand(ChanServ, "SET");
		Command *saset = FindCommand(ChanServ, "SASET");
		if (!set && !saset)
			return;

		ConfigReader config;

		for (int i = 0; true; ++i)
		{
			std::string cname = config.ReadValue("cs_set_misc", "name", "", i);
			if (cname.empty())
				break;
			std::string desc = config.ReadValue("cs_set_misc", "desc", "", i);
			bool showhidden = config.ReadFlag("cs_set_misc", "operonly", "no", i);

			CommandInfo *info = new CommandInfo(cname, desc, showhidden);
			if (!this->Commands.insert(std::make_pair(cname, info)).second)
			{
				Alog() << "cs_set_misc: Warning, unable to add duplicate entry " << cname;
				delete info;
				continue;
			}

			if (set)
				set->AddSubcommand(new CommandCSSetMisc(cname.c_str(), desc));
			if (saset)
				saset->AddSubcommand(new CommandCSSASetMisc(cname.c_str(), desc));
		}
	}

	void OnChanInfo(User *u, ChannelInfo *ci, bool ShowHidden)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (!ShowHidden && it->second->ShowHidden)
				continue;

			ci::string value;
			if (ci->GetExtRegular("chanserv:" + it->first, value))
			{
				u->SendMessage(Config.s_ChanServ, "      %s: %s", it->first.c_str(), value.c_str());
			}
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), ChannelInfo *ci)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			ci::string value;

			if (ci->GetExtRegular("chanserv:" + it->first, value))
			{
				WriteMetadata(it->first, ":" + std::string(value.c_str()));
			}
		}
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const std::string &key, const std::vector<std::string> &params)
	{
		for (std::map<std::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (key == it->first)
			{
				ci->Extend("chanserv:" + it->first, new ExtensibleItemRegular<ci::string>(params[0].c_str()));
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSetMisc)
