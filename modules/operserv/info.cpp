/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
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
#include "modules/nickserv/info.h"
#include "modules/chanserv/info.h"
#include "modules/operserv/info.h"

class OperInfoImpl : public OperInfo
{
	friend class OperInfoType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> info, creator;
	Serialize::Storage<time_t> created;

 public:
	using OperInfo::OperInfo;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetInfo() override;
	void SetInfo(const Anope::string &) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &) override;
};

class OperInfoType : public Serialize::Type<OperInfoImpl>
{
 public:
	Serialize::ObjectField<OperInfoImpl, NickServ::Account *> account;
	Serialize::ObjectField<OperInfoImpl, ChanServ::Channel *> channel;
	Serialize::Field<OperInfoImpl, Anope::string> info, creator;
	Serialize::Field<OperInfoImpl, time_t> created;

	OperInfoType(Module *c) : Serialize::Type<OperInfoImpl>(c)
		, account(this, "account", &OperInfoImpl::account, true)
		, channel(this, "channel", &OperInfoImpl::channel, true)
		, info(this, "info", &OperInfoImpl::info)
		, creator(this, "adder", &OperInfoImpl::creator)
		, created(this, "created", &OperInfoImpl::created)
	{
	}
};

NickServ::Account *OperInfoImpl::GetAccount()
{
	return Get(&OperInfoType::account);
}

void OperInfoImpl::SetAccount(NickServ::Account *acc)
{
	Set(&OperInfoType::account, acc);
}

ChanServ::Channel *OperInfoImpl::GetChannel()
{
	return Get(&OperInfoType::channel);
}

void OperInfoImpl::SetChannel(ChanServ::Channel *c)
{
	Set(&OperInfoType::channel, c);
}

Anope::string OperInfoImpl::GetInfo()
{
	return Get(&OperInfoType::info);
}

void OperInfoImpl::SetInfo(const Anope::string &i)
{
	Set(&OperInfoType::info, i);
}

Anope::string OperInfoImpl::GetCreator()
{
	return Get(&OperInfoType::creator);
}

void OperInfoImpl::SetCreator(const Anope::string &c)
{
	Set(&OperInfoType::creator, c);
}

time_t OperInfoImpl::GetCreated()
{
	return Get(&OperInfoType::created);
}

void OperInfoImpl::SetCreated(const time_t &t)
{
	Set(&OperInfoType::created, t);
}

class CommandOSInfo : public Command
{
 public:
	CommandOSInfo(Module *creator) : Command(creator, "operserv/info", 2, 3)
	{
		this->SetDesc(_("Associate oper info with a nick or channel"));
		this->SetSyntax(_("ADD \037target\037 \037info\037"));
		this->SetSyntax(_("DEL \037target\037 \037info\037"));
		this->SetSyntax(_("CLEAR \037target\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0], target = params[1], info = params.size() > 2 ? params[2] : "";

		NickServ::Account *account = nullptr;
		ChanServ::Channel *channel = nullptr;
		Serialize::Object *e;

		if (IRCD->IsChannelValid(target))
		{
			ChanServ::Channel *ci = ChanServ::Find(target);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), target);
				return;
			}

			channel = ci;
			e = ci;
		}
		else
		{
			NickServ::Nick *na = NickServ::FindNick(target);
			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), target);
				return;
			}

			account = na->GetAccount();
			e = account;
		}

		if (cmd.equals_ci("ADD"))
		{
			if (info.empty())
			{
				this->OnSyntaxError(source, cmd);
				return;
			}

			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>();
			if (oinfos.size() >= Config->GetModule(this->module)->Get<unsigned int>("max", "10"))
			{
				source.Reply(_("The oper info list for \002{0}\002 is full."), target);
				return;
			}

			for (OperInfo *o : oinfos)
				if (o->GetInfo().equals_ci(info))
				{
					source.Reply(_("The oper info already exists on \002{0}\002."), target);
					return;
				}

			OperInfo *o = Serialize::New<OperInfo *>();
			o->SetAccount(account);
			o->SetChannel(channel);
			o->SetInfo(info);
			o->SetCreator(source.GetNick());
			o->SetCreated(Anope::CurTime);

			source.Reply(_("Added info to \002{0}\002."), target);

			logger.Admin(source, _("{source} used {command} to add oper information to {0}: {1}"), target, info);

                	if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
		}
		else if (cmd.equals_ci("DEL"))
		{
			if (info.empty())
			{
				this->OnSyntaxError(source, cmd);
				return;
			}

			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>();
			if (oinfos.empty())
			{
				source.Reply(_("Oper info list for \002{0}\002 is empty."), target);
				return;
			}

			for (OperInfo *o : oinfos)
			{
				if (o->GetInfo().equals_ci(info))
				{
					o->Delete();

					source.Reply(_("Deleted info from \002{0}\002."), target);

					logger.Admin(source, _("{source} used {command} to remove oper information from {0}: {1}"), target, info);

	        	        	if (Anope::ReadOnly)
						source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
					return;
				}
			}

			source.Reply(_("No such info \002{0}\002 on \002{1}\002."), info, target);
		}
		else if (cmd.equals_ci("CLEAR"))
		{
			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>();

			if (oinfos.empty())
			{
				source.Reply(_("Oper info list for \002{0}\002 is empty."), target);
				return;
			}

			for (OperInfo *o : oinfos)
				o->Delete();

			source.Reply(_("Cleared info from \002{0}\002."), target);

			logger.Admin(source, _("{source} used {command} to clear oper information for {0}"), target);

                	if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
		}
		else
		{
			this->OnSyntaxError(source, cmd);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Add or delete oper information for a given account or channel. This information will show to opers in the respective info command for the account or channel."));
		return true;
	}
};

class OSInfo : public Module
	, public EventHook<Event::NickInfo>
	, public EventHook<Event::ChanInfo>
{
	CommandOSInfo commandosinfo;
	OperInfoType oinfotype;

	void OnInfo(CommandSource &source, Serialize::Object *e, InfoFormatter &info)
	{
		if (!source.IsOper())
			return;

		for (OperInfo *o : e->GetRefs<OperInfo *>())
			info[_("Oper Info")] = Anope::printf(_("(by %s on %s) %s"), o->GetCreator().c_str(), Anope::strftime(o->GetCreated(), source.GetAccount(), true).c_str(), o->GetInfo().c_str());
	}

 public:
	OSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::NickInfo>(this)
		, EventHook<Event::ChanInfo>(this)
		, commandosinfo(this)
		, oinfotype(this)
	{
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		OnInfo(source, na->GetAccount(), info);
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		OnInfo(source, ci, info);
	}
};

MODULE_INIT(OSInfo)
