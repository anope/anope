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
#include "chanserv.h"

class CommandCSSetMisc : public Command
{
	Anope::string Desc;
 public:
	CommandCSSetMisc(const Anope::string &cname, const Anope::string &cdesc, const Anope::string &cpermission = "") : Command(cname, 1, 2, cpermission), Desc(cdesc)
	{
		this->SetDesc(cdesc);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetMisc");

		ci->Shrink("chanserv:" + this->name);
		if (params.size() > 1)
		{
			ci->Extend("chanserv:" + this->name, new ExtensibleItemRegular<Anope::string>(params[1]));
			source.Reply(_(CHAN_SETTING_CHANGED), this->name.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
			source.Reply(_(CHAN_SETTING_UNSET), this->name.c_str(), ci->name.c_str());

		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET", _(CHAN_SET_SYNTAX));
	}
};

class CommandCSSASetMisc : public CommandCSSetMisc
{
 public:
	CommandCSSASetMisc(const Anope::string &cname, const Anope::string &cdesc) : CommandCSSetMisc(cname, cdesc, "chanserv/saset/" + cname)
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET", _(CHAN_SASET_SYNTAX));
	}
};

class CSSetMisc : public Module
{
	struct CommandInfo
	{
		Anope::string Name;
		Anope::string Desc;
		bool ShowHidden;
		Command *c;

		CommandInfo(const Anope::string &name, const Anope::string &desc, bool showhidden) : Name(name), Desc(desc), ShowHidden(showhidden) { }
	};

	std::map<Anope::string, CommandInfo *> Commands;

	void RemoveAll()
	{
		if (!chanserv || Commands.empty())
			return;

		Command *set = FindCommand(chanserv->Bot(), "SET");
		Command *saset = FindCommand(chanserv->Bot(), "SASET");

		if (!set && !saset)
			return;

		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (set)
			{
				Command *c = set->FindSubcommand(it->second->Name);
				if (c)
				{
					set->DelSubcommand(c);
					delete c;
				}
			}
			if (saset)
			{
				Command *c = saset->FindSubcommand(it->second->Name);
				if (c)
				{
					saset->DelSubcommand(c);
					delete c;
				}
			}
		}

		this->Commands.clear();
	}

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		Implementation i[] = { I_OnReload, I_OnChanInfo, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, 4);

		OnReload();
	}

	~CSSetMisc()
	{
		RemoveAll();
	}

	void OnReload()
	{
		RemoveAll();

		Command *set = FindCommand(chanserv->Bot(), "SET");
		Command *saset = FindCommand(chanserv->Bot(), "SASET");
		if (!set && !saset)
			return;

		ConfigReader config;

		for (int i = 0, num = config.Enumerate("cs_set_misc"); i < num; ++i)
		{
			Anope::string cname = config.ReadValue("cs_set_misc", "name", "", i);
			if (cname.empty())
				continue;
			Anope::string desc = config.ReadValue("cs_set_misc", "desc", "", i);
			bool showhidden = config.ReadFlag("cs_set_misc", "privileged", "no", i);

			CommandInfo *info = new CommandInfo(cname, desc, showhidden);
			if (!this->Commands.insert(std::make_pair(cname, info)).second)
			{
				Log() << "cs_set_misc: Warning, unable to add duplicate entry " << cname;
				delete info;
				continue;
			}

			if (set)
				set->AddSubcommand(this, new CommandCSSetMisc(cname, desc));
			if (saset)
				saset->AddSubcommand(this, new CommandCSSASetMisc(cname, desc));
		}
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, bool ShowHidden)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			if (!ShowHidden && it->second->ShowHidden)
				continue;

			Anope::string value;
			if (ci->GetExtRegular("chanserv:" + it->first, value))
				source.Reply("      %s: %s", it->first.c_str(), value.c_str());
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), ChannelInfo *ci)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
		{
			Anope::string value;
			if (ci->GetExtRegular("chanserv:" + it->first, value))
				WriteMetadata(it->first, ":" + value);
		}
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		for (std::map<Anope::string, CommandInfo *>::const_iterator it = this->Commands.begin(), it_end = this->Commands.end(); it != it_end; ++it)
			if (key == it->first)
				ci->Extend("chanserv:" + it->first, new ExtensibleItemRegular<Anope::string>(params[0]));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSetMisc)
