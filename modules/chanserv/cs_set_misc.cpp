/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/set_misc.h"

static Module *me;

static Anope::map<Anope::string> descriptions;
static Anope::map<uint16_t> numerics;

struct CSMiscData;
static Anope::map<ExtensibleItem<CSMiscData> *> items;

static ExtensibleItem<CSMiscData> *GetItem(const Anope::string &name)
{
	ExtensibleItem<CSMiscData> *&it = items[name];
	if (!it)
		try
		{
			it = new ExtensibleItem<CSMiscData>(me, name);
		}
		catch (const ModuleException &) { }
	return it;
}

struct CSMiscData final
	: MiscData
	, Serializable
{
	CSMiscData(Extensible *obj) : Serializable("CSMiscData") { }

	CSMiscData(ChannelInfo *c, const Anope::string &n, const Anope::string &d) : Serializable("CSMiscData")
	{
		object = c->name;
		name = n;
		data = d;
	}
};

struct CSMiscDataType
	: Serialize::Type
{
	CSMiscDataType()
		: Serialize::Type("CSMiscData")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &sdata) const override
	{
		const auto *d = static_cast<const CSMiscData *>(obj);
		sdata.Store("ci", d->object);
		sdata.Store("name", d->name);
		sdata.Store("data", d->data);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string sci, sname, sdata;

		data["ci"] >> sci;
		data["name"] >> sname;
		data["data"] >> sdata;

		ChannelInfo *ci = ChannelInfo::Find(sci);
		if (ci == NULL)
			return NULL;

		CSMiscData *d = NULL;
		if (obj)
		{
			d = anope_dynamic_static_cast<CSMiscData *>(obj);
			d->object = ci->name;
			data["name"] >> d->name;
			data["data"] >> d->data;
		}
		else
		{
			ExtensibleItem<CSMiscData> *item = GetItem(sname);
			if (item)
				d = item->Set(ci, CSMiscData(ci, sname, sdata));
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

class CommandCSSetMisc final
	: public Command
{
public:
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc") : Command(creator, cname, 1, 2)
	{
		this->SetSyntax(_("\037channel\037 [\037parameters\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &param = params.size() > 1 ? params[1] : "";
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		Anope::string scommand = GetAttribute(source.command);
		Anope::string key = "cs_set_misc:" + scommand;
		ExtensibleItem<CSMiscData> *item = GetItem(key);
		if (item == NULL)
			return;

		if (!param.empty())
		{
			item->Set(ci, CSMiscData(ci, key, param));
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change it to " << param;
			source.Reply(CHAN_SETTING_CHANGED, scommand.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
		{
			item->Unset(ci);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset it";
			source.Reply(CHAN_SETTING_UNSET, scommand.c_str(), ci->name.c_str());
		}
	}

	void OnServHelp(CommandSource &source, HelpWrapper &help) override
	{
		if (descriptions.count(source.command))
		{
			this->SetDesc(descriptions[source.command]);
			Command::OnServHelp(source, help);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (descriptions.count(source.command))
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply("%s", Language::Translate(source.nc, descriptions[source.command].c_str()));
			return true;
		}
		return false;
	}
};

class CSSetMisc final
	: public Module
{
	CommandCSSetMisc commandcssetmisc;
	CSMiscDataType csmiscdata_type;

public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandcssetmisc(this)
	{
		me = this;
	}

	~CSSetMisc() override
	{
		for (const auto &[_, item] : items)
			delete item;
	}

	void OnReload(Configuration::Conf &conf) override
	{
		descriptions.clear();
		numerics.clear();

		for (int i = 0; i < conf.CountBlock("command"); ++i)
		{
			const auto &block = conf.GetBlock("command", i);

			if (block.Get<const Anope::string>("command") != "chanserv/set/misc")
				continue;

			Anope::string cname = block.Get<const Anope::string>("name");
			Anope::string desc = block.Get<const Anope::string>("misc_description");

			if (cname.empty() || desc.empty())
				continue;

			descriptions[cname] = desc;

			// Force creation of the extension item.
			const auto extname = "cs_set_misc:" + GetAttribute(cname);
			GetItem(extname);

			auto numeric = block.Get<unsigned>("misc_numeric");
			if (numeric >= 1 && numeric <= 999)
				numerics[extname] = numeric;
		}
	}

	void OnJoinChannel(User *user, Channel *c) override
	{
		if (!c->ci || !user->server->IsSynced() || user->server == Me || numerics.empty())
			return;

		for (const auto &[name, ext] : items)
		{
			auto *data = ext->Get(c->ci);
			if (!data)
				continue;

			auto numeric = numerics.find(name);
			if (numeric != numerics.end())
				IRCD->SendNumeric(numeric->second, user->GetUID(), c->ci->name, data->data);
		}
	}


	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool) override
	{
		for (const auto &[_, e] : items)
		{
			MiscData *data = e->Get(ci);

			if (data != NULL)
				info[e->name.substr(12).replace_all_cs("_", " ")] = data->data;
		}
	}
};

MODULE_INIT(CSSetMisc)
