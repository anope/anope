/*
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

static std::map<Anope::string, Anope::string> descriptions;

struct CSMiscData : ExtensibleItem, Serializable
{
	Serialize::Reference<ChannelInfo> ci;
	Anope::string name;
	Anope::string data;

	CSMiscData(ChannelInfo *c, const Anope::string &n, const Anope::string &d) : Serializable("CSMiscData"), ci(c), name(n), data(d)
	{
	}

	void Serialize(Serialize::Data &sdata) const anope_override
	{
		sdata["ci"] << this->ci->name;
		sdata["name"] << this->name;
		sdata["data"] << this->data;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string sci, sname, sdata;

		data["ci"] >> sci;
		data["name"] >> sname;
		data["data"] >> sdata;

		ChannelInfo *ci = ChannelInfo::Find(sci);
		if (ci == NULL)
			return NULL;

		CSMiscData *d;
		if (obj)
		{
			d = anope_dynamic_static_cast<CSMiscData *>(obj);
			d->ci = ci;
			data["name"] >> d->name;
			data["data"] >> d->data;
		}
		else
		{
			d = new CSMiscData(ci, sname, sdata);
			ci->Extend(sname, d);
		}

		return d;
	}
};

static Anope::string GetAttribute(const Anope::string &command)
{
	size_t sp = command.rfind(' ');
	if (sp != Anope::string::npos)
		return command.substr(sp + 1);
	return command;
}

class CommandCSSetMisc : public Command
{
 public:
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc") : Command(creator, cname, 1, 2)
	{
		this->SetSyntax(_("\037channel\037 [\037parameters\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		Anope::string scommand = GetAttribute(source.command);
		Anope::string key = "cs_set_misc:" + scommand;
		ci->Shrink(key);
		if (params.size() > 1)
		{
			ci->Extend(key, new CSMiscData(ci, key, params[1]));
			source.Reply(CHAN_SETTING_CHANGED, scommand.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, scommand.c_str(), ci->name.c_str());
	}

	void OnServHelp(CommandSource &source) anope_override
	{
		if (descriptions.count(source.command))
		{
			this->SetDesc(descriptions[source.command]);
			Command::OnServHelp(source);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		if (descriptions.count(source.command))
		{
			source.Reply("%s", Language::Translate(source.nc, descriptions[source.command].c_str()));
			return true;
		}
		return false;
	}
};

class CSSetMisc : public Module
{
	Serialize::Type csmiscdata_type;
	CommandCSSetMisc commandcssetmisc;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		csmiscdata_type("CSMiscData", CSMiscData::Unserialize), commandcssetmisc(this)
	{
		Implementation i[] = { I_OnReload, I_OnChanInfo };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnReload(ServerConfig *conf, ConfigReader &reader) anope_override
	{
		descriptions.clear();

		for (int i = 0; i < reader.Enumerate("command"); ++i)
		{
			if (reader.ReadValue("command", "command", "", i) != "chanserv/set/misc")
				continue;

			Anope::string cname = reader.ReadValue("command", "name", "", i);
			Anope::string desc = reader.ReadValue("command", "misc_description", "", i);

			if (cname.empty() || desc.empty())
				continue;

			descriptions[cname] = desc;
		}
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool ShowHidden) anope_override
	{
		std::deque<Anope::string> list;
		ci->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("cs_set_misc:") != 0)
				continue;
			
			CSMiscData *data = ci->GetExt<CSMiscData *>(list[i]);
			if (data != NULL)
				info[list[i].substr(12).replace_all_cs("_", " ")] = data->data;
		}
	}
};

MODULE_INIT(CSSetMisc)
