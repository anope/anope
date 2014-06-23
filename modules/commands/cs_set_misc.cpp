/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/set_misc.h"
#include "modules/cs_info.h"
#include "modules/cs_set.h"

static Module *me;

static std::map<Anope::string, Anope::string> descriptions;

struct CSMiscData;
static Anope::map<ExtensibleItem<CSMiscData> *> items;

static ExtensibleItem<CSMiscData> *GetItem(const Anope::string &name)
{
	ExtensibleItem<CSMiscData>* &it = items[name];
	if (!it)
		try
		{
			it = new ExtensibleItem<CSMiscData>(me, name);
		}
		catch (const ModuleException &) { }
	return it;
}

struct CSMiscData : MiscData, Serializable
{
	CSMiscData(Extensible *obj) : Serializable("CSMiscData") { }

	CSMiscData(ChanServ::Channel *c, const Anope::string &n, const Anope::string &d) : Serializable("CSMiscData")
	{
		object = c->name;
		name = n;
		data = d;
	}

	void Serialize(Serialize::Data &sdata) const override
	{
		sdata["ci"] << this->object;
		sdata["name"] << this->name;
		sdata["data"] << this->data;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string sci, sname, sdata;

		data["ci"] >> sci;
		data["name"] >> sname;
		data["data"] >> sdata;

		ChanServ::Channel *ci = ChanServ::Find(sci);
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

class CommandCSSetMisc : public Command
{
 public:
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc") : Command(creator, cname, 1, 2)
	{
		this->SetSyntax(_("\037channel\037 [\037parameters\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

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

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
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
			source.Reply(_("\002{0}\002 for \002{1}\002 set to \002{2}\002."), scommand, ci->name, param);
		}
		else
		{
			item->Unset(ci);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset it";
			source.Reply(_("\002{0}\002 for \002{1}\002 unset."), scommand, ci->name);
		}
	}

	void OnServHelp(CommandSource &source) override
	{
		if (descriptions.count(source.command))
		{
			this->SetDesc(descriptions[source.command]);
			Command::OnServHelp(source);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
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
	, public EventHook<Event::ChanInfo>
{
	CommandCSSetMisc commandcssetmisc;
	Serialize::Type csmiscdata_type;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanInfo>("OnChanInfo")
		, commandcssetmisc(this)
		, csmiscdata_type("CSMiscData", CSMiscData::Unserialize)
	{
		me = this;
	}

	~CSSetMisc()
	{
		for (Anope::map<ExtensibleItem<CSMiscData> *>::iterator it = items.begin(); it != items.end(); ++it)
			delete it->second;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		descriptions.clear();

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);

			if (block->Get<const Anope::string>("command") != "chanserv/set/misc")
				continue;

			Anope::string cname = block->Get<const Anope::string>("name");
			Anope::string desc = block->Get<const Anope::string>("misc_description");

			if (cname.empty() || desc.empty())
				continue;

			descriptions[cname] = desc;
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool) override
	{
		for (Anope::map<ExtensibleItem<CSMiscData> *>::iterator it = items.begin(); it != items.end(); ++it)
		{
			ExtensibleItem<CSMiscData> *e = it->second;
			MiscData *data = e->Get(ci);

			if (data != NULL)
				info[e->name.substr(12).replace_all_cs("_", " ")] = data->data;
		}
	}
};

MODULE_INIT(CSSetMisc)
