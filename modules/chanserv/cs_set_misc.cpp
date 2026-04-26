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

#define MISC_PREFIX "cs_set_misc:"

struct CommandData final
{
	Anope::string description;
	Anope::string pattern;
	Anope::string syntax;
	Anope::string title;
	unsigned numeric = 0;
};

static Anope::map<CommandData> command_data;

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
		auto *ci = ChannelInfo::Find(data.Load("ci"));
		if (ci == NULL)
			return NULL;

		const auto sname = data.Load("name");
		const auto sdata = data.Load("data");

		CSMiscData *d = NULL;
		if (obj)
		{
			d = anope_dynamic_static_cast<CSMiscData *>(obj);
			d->object = ci->name;
			d->name = sname;
			d->data = sdata;
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
		return MISC_PREFIX + command.substr(sp + 1);
	return MISC_PREFIX + command;
}

static const char* GetTitle(ExtensibleItem<CSMiscData> *ext)
{
	auto it = command_data.find(ext->name);
	if (it == command_data.end() || it->second.title.empty())
		return ext->name.c_str() + 12;
	return it->second.title.c_str();
}

class CommandCSSetMisc final
	: public Command
{
private:
	bool CheckSyntax(CommandSource &source, ExtensibleItem<CSMiscData> *ext, const Anope::string &value) const
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
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc") : Command(creator, cname, 1, 2)
	{
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

		const auto key = GetAttribute(source.command);
		ExtensibleItem<CSMiscData> *item = GetItem(key);
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

			item->Set(ci, CSMiscData(ci, key, param));
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change it to " << param;
			source.Reply(CHAN_SETTING_CHANGED, GetTitle(item), ci->name.c_str(), params[1].c_str());
		}
		else
		{
			item->Unset(ci);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset it";
			source.Reply(CHAN_SETTING_UNSET, GetTitle(item), ci->name.c_str());
		}
	}

	void OnServHelp(CommandSource &source, HelpWrapper &help) override
	{
		auto it = command_data.find(GetAttribute(source.command));
		if (it != command_data.end() && !it->second.description.empty())
		{
			this->SetDesc(it->second.description);
			Command::OnServHelp(source, help);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		auto it = command_data.find(GetAttribute(source.command));
		if (it != command_data.end() && !it->second.description.empty())
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply("%s", source.Translate(it->second.description.c_str()));
			return true;
		}
		return false;
	}

	void SendSyntax(CommandSource &source) override
	{
		auto *value = _("value");
		auto it = command_data.find(GetAttribute(source.command));
		if (it != command_data.end() && !it->second.syntax.empty())
			value = it->second.syntax.c_str();

		this->ClearSyntax();
		this->SetSyntax(Anope::Format(
			source.Translate(_("\037channel\037 [\037%s\037]")),
			source.Translate(value)
		));

		Command::SendSyntax(source);
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
		command_data.clear();
		for (int i = 0; i < conf.CountBlock("command"); ++i)
		{
			const auto &block = conf.GetBlock("command", i);
			if (block.Get<const Anope::string>("command") != "chanserv/set/misc")
				continue;

			Anope::string cname = block.Get<const Anope::string>("name");
			Anope::string desc = block.Get<const Anope::string>("misc_description");
			if (cname.empty() || desc.empty())
				continue;

			// Force creation of the extension item.
			const auto extname = GetAttribute(cname);
			GetItem(extname);

			auto &data = command_data[extname];
			data.description = desc;
			data.title = block.Get<const Anope::string>("misc_title");
			data.pattern = block.Get<const Anope::string>("misc_pattern");
			data.syntax = block.Get<const Anope::string>("misc_syntax");

			const auto numeric = block.Get<unsigned>("misc_numeric");
			if (numeric >= 1 && numeric <= 999)
				data.numeric =  numeric;
		}
	}

	void OnJoinChannel(User *user, Channel *c) override
	{
		if (!c->ci || !user->server->IsSynced() || user->server == Me || command_data.empty())
			return;

		for (const auto &[name, ext] : items)
		{
			auto *data = ext->Get(c->ci);
			if (!data)
				continue;

			auto command = command_data.find(name);
			if (command != command_data.end() && command->second.numeric)
				IRCD->SendNumeric(command->second.numeric, user->GetUID(), c->ci->name, data->data);
		}
	}


	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool) override
	{
		for (const auto &[_, e] : items)
		{
			MiscData *data = e->Get(ci);

			if (data != NULL)
				info[GetTitle(e)] = data->data;
		}
	}
};

MODULE_INIT(CSSetMisc)
