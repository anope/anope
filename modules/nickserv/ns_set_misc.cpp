// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/set_misc.h"

static Module *me;

#define MISC_PREFIX "ns_set_misc:"

struct CommandData final
{
	Anope::string saset_description;
	Anope::string set_description;
	Anope::string pattern;
	Anope::string syntax;
	Anope::string title;
	time_t priority = 0;
	bool swhois = false;
};

static Anope::map<CommandData> command_data;

struct NSMiscData;
static Anope::map<ExtensibleItem<NSMiscData> *> items;
static std::vector<std::pair<time_t, ExtensibleItem<NSMiscData> *>> items_by_priority;

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
		auto *nc = NickCore::Find(data.Load("nc"));
		if (nc == NULL)
			return NULL;

		const auto sname = data.Load("name");
		const auto sdata = data.Load("data");

		NSMiscData *d = NULL;
		if (obj)
		{
			d = anope_dynamic_static_cast<NSMiscData *>(obj);
			d->object = nc->display;
			d->name = sname;
			d->data =  sdata;
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
		return MISC_PREFIX + command.substr(sp + 1);
	return MISC_PREFIX + command;
}

static const char* GetTitle(ExtensibleItem<NSMiscData> *ext)
{
	auto it = command_data.find(ext->name);
	if (it == command_data.end() || it->second.title.empty())
		return ext->name.c_str() + 12;
	return it->second.title.c_str();
}

static void CheckSWhois(User* u, const Anope::string &name, ExtensibleItem<NSMiscData> *ext)
{
	auto it = command_data.find(name);
	if (it == command_data.end() || !it->second.swhois)
		return; // No swhois.

	auto *nickserv = Config->GetClient("NickServ");

	auto *nc = u->Account();
	auto *data = nc ? ext->Get(nc) : nullptr;
	if (data)
		IRCD->SendSWhois(nickserv, u, name, it->second.priority, Anope::Format("%s: %s", GetTitle(ext), data->data.c_str()));
	else
		IRCD->SendSWhoisDel(nickserv, u, name, "");
}

class CommandNSSetMisc
	: public Command
{
protected:
	bool saset = false;

private:
	bool CheckSyntax(CommandSource &source, ExtensibleItem<NSMiscData> *ext, const Anope::string &value) const
	{
		auto it = command_data.find(ext->name);
		if (it == command_data.end() || it->second.pattern.empty())
			return true; // No syntax validation.

		ServiceReference<RegexProvider> regex("Regex", Config->GetBlock("options").Get<const Anope::string>("regexengine"));
		if (!regex)
			return true; // No regex engine.

		auto ret = true;
		try
		{
			auto *pattern = regex->Compile(it->second.pattern);
			ret = pattern->Matches(value);
			delete pattern;
		}
		catch (const RegexException &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}
		return ret;
	}

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

		const auto key = GetAttribute(source.command);
		ExtensibleItem<NSMiscData> *item = GetItem(key);
		if (item == NULL)
			return;

		if (!param.empty())
		{
			if (!CheckSyntax(source, item, param))
			{
				source.Reply(CHAN_SETTING_INVALID, GetTitle(item));
				this->SendSyntax(source);
				return;
			}

			item->Set(nc, NSMiscData(nc, key, param));
			source.Reply(CHAN_SETTING_CHANGED, GetTitle(item), nc->display.c_str(), param.c_str());
		}
		else
		{
			item->Unset(nc);
			source.Reply(CHAN_SETTING_UNSET, GetTitle(item), nc->display.c_str());
		}

		for (auto *u : nc->users)
			CheckSWhois(u, key, item);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, !params.empty() ? params[0] : "");
	}

	void OnServHelp(CommandSource &source, HelpWrapper &help) override
	{
		auto it = command_data.find(GetAttribute(source.command));
		if (it == command_data.end())
			return;

		const auto &desc = saset ? it->second.saset_description : it->second.set_description;
		if (desc.empty())
			return;

		this->SetDesc(desc);
		Command::OnServHelp(source, help);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		auto it = command_data.find(GetAttribute(source.command));
		if (it == command_data.end())
			return false;

		const auto &desc = saset ? it->second.saset_description : it->second.set_description;
		if (desc.empty())
			return false;

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply("%s", source.Translate(desc.c_str()));
		return true;
	}

	void SendSyntax(CommandSource &source) override
	{
		auto *value = _("value");
		auto it = command_data.find(GetAttribute(source.command));
		if (it != command_data.end() && !it->second.syntax.empty())
			value = it->second.syntax.c_str();

		this->ClearSyntax();
		this->SetSyntax(Anope::Format("[\037%s\037]", source.Translate(value)));

		Command::SendSyntax(source);
	}
};

class CommandNSSASetMisc final
	: public CommandNSSetMisc
{
public:
	CommandNSSASetMisc(Module *creator) : CommandNSSetMisc(creator, "nickserv/saset/misc", 1)
	{
		this->saset = true;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	void SendSyntax(CommandSource &source) override
	{
		auto *value = _("value");
		auto it = command_data.find(GetAttribute(source.command));
		if (it != command_data.end() && !it->second.syntax.empty())
			value = it->second.syntax.c_str();

		this->ClearSyntax();
		this->SetSyntax(Anope::Format(
			source.Translate(_("\037nickname\037 [\037%s\037]")),
			source.Translate(value)
		));

		Command::SendSyntax(source);
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
		command_data.clear();
		items_by_priority.clear();

		time_t default_priority = 0;
		for (const auto &[_,  block] : conf.GetBlocks("command"))
		{
			const Anope::string &cmd = block.Get<const Anope::string>("command");
			if (cmd != "nickserv/set/misc" && cmd != "nickserv/saset/misc")
				continue;

			Anope::string cname = block.Get<const Anope::string>("name");
			Anope::string desc = block.Get<const Anope::string>("misc_description");
			if (cname.empty() || desc.empty())
				continue;

			// Force creation of the extension item.
			const auto extname = GetAttribute(cname);
			auto item = GetItem(extname);

			auto &data = command_data[extname];
			if (cmd == "nickserv/saset/misc")
			{
				data.saset_description = desc;
				continue;
			}

			default_priority += 1000;

			data.set_description = desc;
			data.pattern = block.Get<const Anope::string>("misc_pattern");
			data.syntax = block.Get<const Anope::string>("misc_syntax");
			data.title = block.Get<const Anope::string>("misc_title");
			data.priority = block.Get<time_t>("misc_priority", "0");
			if (data.priority <= 0)
			{
				// If no priority is specified, go by order processed
				data.priority = default_priority;
			}
			data.swhois = block.Get<bool>("misc_swhois");
			items_by_priority.emplace_back(data.priority, item);
		}
		std::sort(items_by_priority.begin(), items_by_priority.end());
	}

	void OnUserLogin(User *u) override
	{
		if (u->server == Me || command_data.empty() || !IRCD->CanSendMultipleSWhois)
			return;

		for (const auto &[name, ext] : items)
			CheckSWhois(u, name, ext);
	}

	void OnNickLogout(User *u) override
	{
		OnUserLogin(u);
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool) override
	{
		for (const auto &[_, e] : items_by_priority)
		{
			NSMiscData *data = e->Get(na->nc);

			if (data != NULL)
				info[GetTitle(e)] = data->data;
		}
	}
};

MODULE_INIT(NSSetMisc)
