/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct CSMiscData : ExtensibleItem, Serializable
{
	serialize_obj<ChannelInfo> ci;
	Anope::string name;
	Anope::string data;

	CSMiscData(ChannelInfo *c, const Anope::string &n, const Anope::string &d) : ci(c), name(n), data(d)
	{
	}

	const Anope::string serialize_name() const anope_override
	{
		return "CSMiscData";
	}

	Serialize::Data serialize() const anope_override
	{
		Serialize::Data sdata;

		sdata["ci"] << this->ci->name;
		sdata["name"] << this->name;
		sdata["data"] << this->data;

		return sdata;
	}

	static Serializable* unserialize(Serializable *obj, Serialize::Data &data)
	{
		ChannelInfo *ci = cs_findchan(data["ci"].astr());
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
			d = new CSMiscData(ci, data["name"].astr(), data["data"].astr());
			ci->Extend(data["name"].astr(), d);
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
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		else if (source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
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
};

class CommandCSSASetMisc : public CommandCSSetMisc
{
 public:
	CommandCSSASetMisc(Module *creator) : CommandCSSetMisc(creator, "chanserv/saset/misc")
	{
	}
};

class CSSetMisc : public Module
{
	SerializeType csmiscdata_type;
	CommandCSSetMisc commandcssetmisc;
	CommandCSSASetMisc commandcssasetmisc;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		csmiscdata_type("CSMiscData", CSMiscData::unserialize), commandcssetmisc(this), commandcssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnChanInfo };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
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
