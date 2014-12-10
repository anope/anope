/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/ns_info.h"
#include "modules/cs_info.h"
#include "modules/os_info.h"

class OperInfoImpl : public OperInfo
{
 public:
	OperInfoImpl(Serialize::TypeBase *type) : OperInfo(type) { }
	OperInfoImpl(Serialize::TypeBase *type, Serialize::ID id) : OperInfo(type, id) { }

	Serialize::Object *GetTarget();
	void SetTarget(Serialize::Object *);

	Anope::string GetInfo();
	void SetInfo(const Anope::string &);

	Anope::string GetCreator();
	void SetCreator(const Anope::string &);

	time_t GetCreated();
	void SetCreated(const time_t &);
};

class OperInfoType : public Serialize::Type<OperInfoImpl>
{
 public:
	Serialize::ObjectField<OperInfoImpl, Serialize::Object *> target;
	Serialize::Field<OperInfoImpl, Anope::string> info, creator;
	Serialize::Field<OperInfoImpl, time_t> created;

	OperInfoType(Module *c) : Serialize::Type<OperInfoImpl>(c, "OperInfo")
		, target(this, "target", true)
		, info(this, "info")
		, creator(this, "adder")
		, created(this, "created")
	{
	}
};

Serialize::Object *OperInfoImpl::GetTarget()
{
	return Get(&OperInfoType::target);
}

void OperInfoImpl::SetTarget(Serialize::Object *t)
{
	Set(&OperInfoType::target, t);
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

		Serialize::Object *e;
		if (IRCD->IsChannelValid(target))
		{
			ChanServ::Channel *ci = ChanServ::Find(target);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), target);
				return;
			}

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

			e = na->GetAccount();
		}

		if (cmd.equals_ci("ADD"))
		{
			if (info.empty())
			{
				this->OnSyntaxError(source, cmd);
				return;
			}

			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>(operinfo);
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

			OperInfo *o = operinfo.Create();
			o->SetTarget(e);
			o->SetInfo(info);
			o->SetCreator(source.GetNick());
			o->SetCreated(Anope::CurTime);

			source.Reply(_("Added info to \002{0}\002."), target);
			Log(LOG_ADMIN, source, this) << "to add information to " << target;

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

			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>(operinfo);
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
					Log(LOG_ADMIN, source, this) << "to remove information from " << target;

	        	        	if (Anope::ReadOnly)
						source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
					return;
				}
			}

			source.Reply(_("No such info \002{0}\002 on \002{1}\002."), info, target);
		}
		else if (cmd.equals_ci("CLEAR"))
		{
			std::vector<OperInfo *> oinfos = e->GetRefs<OperInfo *>(operinfo);

			if (oinfos.empty())
			{
				source.Reply(_("Oper info list for \002{0}\002 is empty."), target);
				return;
			}

			for (OperInfo *o : oinfos)
				o->Delete();

			source.Reply(_("Cleared info from \002{0}\002."), target);
			Log(LOG_ADMIN, source, this) << "to clear information for " << target;

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
		source.Reply(_("Add or delete oper information for a given accpint or channel. This informaition will show to opers in the respective info command for the account or channel."));
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

		for (OperInfo *o : e->GetRefs<OperInfo *>(operinfo))
			info[_("Oper Info")] = Anope::printf(_("(by %s on %s) %s"), o->GetCreator().c_str(), Anope::strftime(o->GetCreated(), source.GetAccount(), true).c_str(), o->GetInfo().c_str());
	}

 public:
	OSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::NickInfo>()
		, EventHook<Event::ChanInfo>()
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
