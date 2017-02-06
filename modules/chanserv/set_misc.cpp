/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */


#include "module.h"
#include "modules/chanserv/set_misc.h"
#include "modules/chanserv/info.h"
#include "modules/chanserv/set.h"

static Anope::map<Anope::string> descriptions;

class CSMiscDataImpl : public CSMiscData
{
	friend class CSMiscDataType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> name, data;

 public:
	using CSMiscData::CSMiscData;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *s) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &n) override;

	Anope::string GetData() override;
	void SetData(const Anope::string &d) override;
};

class CSMiscDataType : public Serialize::Type<CSMiscDataImpl>
{
 public:
	Serialize::ObjectField<CSMiscDataImpl, ChanServ::Channel *> owner;
	Serialize::Field<CSMiscDataImpl, Anope::string> name, data;

	CSMiscDataType(Module *me) : Serialize::Type<CSMiscDataImpl>(me)
		, owner(this, "channel", &CSMiscDataImpl::channel, true)
		, name(this, "name", &CSMiscDataImpl::name)
		, data(this, "data", &CSMiscDataImpl::data)
	{
	}
};

ChanServ::Channel *CSMiscDataImpl::GetChannel()
{
	return Get(&CSMiscDataType::owner);
}

void CSMiscDataImpl::SetChannel(ChanServ::Channel *s)
{
	Set(&CSMiscDataType::owner, s);
}

Anope::string CSMiscDataImpl::GetName()
{
	return Get(&CSMiscDataType::name);
}

void CSMiscDataImpl::SetName(const Anope::string &n)
{
	Set(&CSMiscDataType::name, n);
}

Anope::string CSMiscDataImpl::GetData()
{
	return Get(&CSMiscDataType::data);
}

void CSMiscDataImpl::SetData(const Anope::string &d)
{
	Set(&CSMiscDataType::data, d);
}

class CommandCSSetMisc : public Command
{
	Anope::string GetAttribute(const Anope::string &command)
	{
		size_t sp = command.rfind(' ');
		if (sp != Anope::string::npos)
			return command.substr(sp + 1);
		return command;
	}

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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.GetPermission().empty() && !source.HasOverridePriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		Anope::string scommand = GetAttribute(source.GetCommand());

		/* remove existing */
		for (CSMiscData *data : ci->GetRefs<CSMiscData *>())
			if (data->GetName() == scommand)
			{
				data->Delete();
				break;
			}

		if (!param.empty())
		{
			CSMiscData *data = Serialize::New<CSMiscData *>();
			data->SetChannel(ci);
			data->SetName(scommand);
			data->SetData(param);

			logger.Command(source, ci,
					_("{source} used {command} on {channel} to change it to {0}"),
					param);

			source.Reply(_("\002{0}\002 for \002{1}\002 set to \002{2}\002."), scommand, ci->GetName(), param);
		}
		else
		{
			logger.Command(source, ci, _("{source} used {command} on {channel} to unset it"));

			source.Reply(_("\002{0}\002 for \002{1}\002 unset."), scommand, ci->GetName());
		}
	}

	void OnServHelp(CommandSource &source) override
	{
		if (descriptions.count(source.GetCommand()))
		{
			this->SetDesc(descriptions[source.GetCommand()]);
			Command::OnServHelp(source);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (descriptions.count(source.GetCommand()))
		{
			source.Reply(Language::Translate(source.nc, descriptions[source.GetCommand()]));
			return true;
		}
		return false;
	}
};

class CSSetMisc : public Module
	, public EventHook<Event::ChanInfo>
{
	CommandCSSetMisc commandcssetmisc;
	CSMiscDataType type;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanInfo>(this)
		, commandcssetmisc(this)
		, type(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		descriptions.clear();

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);

			if (block->Get<Anope::string>("command") != "chanserv/set/misc")
				continue;

			Anope::string cname = block->Get<Anope::string>("name");
			Anope::string desc = block->Get<Anope::string>("misc_description");

			if (cname.empty() || desc.empty())
				continue;

			descriptions[cname] = desc;
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool) override
	{
		for (CSMiscData *data : ci->GetRefs<CSMiscData *>())
			info[data->GetName()] = data->GetData();
	}
};

MODULE_INIT(CSSetMisc)
