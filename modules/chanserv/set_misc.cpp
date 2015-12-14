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
#include "modules/cs_set_misc.h"
#include "modules/cs_info.h"
#include "modules/cs_set.h"

static Anope::map<Anope::string> descriptions;

class CSMiscDataImpl : public CSMiscData
{
 public:
	CSMiscDataImpl(Serialize::TypeBase *type) : CSMiscData(type) { }
	CSMiscDataImpl(Serialize::TypeBase *type, Serialize::ID id) : CSMiscData(type, id) { }

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

	CSMiscDataType(Module *me) : Serialize::Type<CSMiscDataImpl>(me, "CSMiscData")
		, owner(this, "owner", true)
		, name(this, "name")
		, data(this, "data")
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
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		Anope::string scommand = GetAttribute(source.command);

		/* remove existing */
		for (CSMiscData *data : ci->GetRefs<CSMiscData *>(csmiscdata))
			if (data->GetName() == scommand)
			{
				data->Delete();
				break;
			}

		if (!param.empty())
		{
			CSMiscData *data = csmiscdata.Create();
			data->SetChannel(ci);
			data->SetName(scommand);
			data->SetData(param);

			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change it to " << param;
			source.Reply(_("\002{0}\002 for \002{1}\002 set to \002{2}\002."), scommand, ci->GetName(), param);
		}
		else
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset it";
			source.Reply(_("\002{0}\002 for \002{1}\002 unset."), scommand, ci->GetName());
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
	CSMiscDataType type;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
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
		for (CSMiscData *data : ci->GetRefs<CSMiscData *>(csmiscdata))
			info[data->GetName()] = data->GetData();
	}
};

MODULE_INIT(CSSetMisc)
