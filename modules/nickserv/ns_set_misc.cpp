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

struct NSMiscData;
static Anope::map<ExtensibleItem<NSMiscData> *> items;

static ExtensibleItem<NSMiscData> *GetItem(const Anope::string &name)
{
	ExtensibleItem<NSMiscData> *&it = items[name];
	if (!it)
		try
		{
			it = new ExtensibleItem<NSMiscData>(me, name);
		}
		catch (const ModuleException &) { }
	return it;
}

struct NSMiscData final
	: MiscData
	, Serializable
{
	NSMiscData(Extensible *) : Serializable("NSMiscData") { }

	NSMiscData(NickCore *ncore, const Anope::string &n, const Anope::string &d) : Serializable("NSMiscData")
	{
		object = ncore->display;
		name = n;
		data = d;
	}
};

struct NSMiscDataType final
	: Serialize::Type
{
	NSMiscDataType()
		: Serialize::Type("NSMiscData")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &sdata) const override
	{
		const auto *d = static_cast<const NSMiscData *>(obj);
		sdata.Store("nc", d->object);
		sdata.Store("name", d->name);
		sdata.Store("data", d->data);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string snc, sname, sdata;

		data["nc"] >> snc;
		data["name"] >> sname;
		data["data"] >> sdata;

		NickCore *nc = NickCore::Find(snc);
		if (nc == NULL)
			return NULL;

		NSMiscData *d = NULL;
		if (obj)
		{
			d = anope_dynamic_static_cast<NSMiscData *>(obj);
			d->object = nc->display;
			data["name"] >> d->name;
			data["data"] >> d->data;
		}
		else
		{
			ExtensibleItem<NSMiscData> *item = GetItem(sname);
			if (item)
				d = item->Set(nc, NSMiscData(nc, sname, sdata));
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

class CommandNSSetMisc
	: public Command
{
public:
	CommandNSSetMisc(Module *creator, const Anope::string &cname = "nickserv/set/misc", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetSyntax(_("[\037parameter\037]"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Anope::string scommand = GetAttribute(source.command);
		Anope::string key = "ns_set_misc:" + scommand;
		ExtensibleItem<NSMiscData> *item = GetItem(key);
		if (item == NULL)
			return;

		if (!param.empty())
		{
			item->Set(nc, NSMiscData(nc, key, param));
			source.Reply(CHAN_SETTING_CHANGED, scommand.c_str(), nc->display.c_str(), param.c_str());
		}
		else
		{
			item->Unset(nc);
			source.Reply(CHAN_SETTING_UNSET, scommand.c_str(), nc->display.c_str());
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, !params.empty() ? params[0] : "");
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

class CommandNSSASetMisc final
	: public CommandNSSetMisc
{
public:
	CommandNSSASetMisc(Module *creator) : CommandNSSetMisc(creator, "nickserv/saset/misc", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 [\037parameter\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}
};

class NSSetMisc final
	: public Module
{
	CommandNSSetMisc commandnssetmisc;
	CommandNSSASetMisc commandnssasetmisc;
	NSMiscDataType nsmiscdata_type;

public:
	NSSetMisc(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetmisc(this)
		, commandnssasetmisc(this)
	{
		me = this;
	}

	~NSSetMisc() override
	{
		for (const auto &[_, data] : items)
			delete data;
	}

	void OnReload(Configuration::Conf &conf) override
	{
		descriptions.clear();

		for (int i = 0; i < conf.CountBlock("command"); ++i)
		{
			const auto &block = conf.GetBlock("command", i);

			const Anope::string &cmd = block.Get<const Anope::string>("command");

			if (cmd != "nickserv/set/misc" && cmd != "nickserv/saset/misc")
				continue;

			Anope::string cname = block.Get<const Anope::string>("name");
			Anope::string desc = block.Get<const Anope::string>("misc_description");

			if (cname.empty() || desc.empty())
				continue;

			descriptions[cname] = desc;

			// Force creation of the extension item.
			GetItem("ns_set_misc:" + GetAttribute(cname));
		}
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool) override
	{
		for (const auto &[_, e] : items)
		{
			NSMiscData *data = e->Get(na->nc);

			if (data != NULL)
				info[e->name.substr(12).replace_all_cs("_", " ")] = data->data;
		}
	}
};

MODULE_INIT(NSSetMisc)
