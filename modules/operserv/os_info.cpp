/* OperServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/info.h"

struct OperInfoImpl final
	: OperInfo
	, Serializable
{
	OperInfoImpl()
		: Serializable("OperInfo")
	{
	}

	OperInfoImpl(const Anope::string &t, const Anope::string &i, const Anope::string &a, time_t c)
		: OperInfo(t, i, a, c)
		, Serializable("OperInfo")
	{
	}

	~OperInfoImpl() override;

	void Serialize(Serialize::Data &data) const override
	{
		data.Store("target", target);
		data.Store("info", info);
		data.Store("adder", adder);
		data.Store("created", created);
	}

	static Serializable *Unserialize(Serializable *obj, Serialize::Data &data);
};

struct OperInfos final
	: OperInfoList
{
	OperInfos(Extensible *) { }

	OperInfo *Create() override
	{
		return new OperInfoImpl();
	}

	static Extensible *Find(const Anope::string &target)
	{
		NickAlias *na = NickAlias::Find(target);
		if (na)
			return na->nc;
		return ChannelInfo::Find(target);
	}
};

OperInfoImpl::~OperInfoImpl()
{
	Extensible *e = OperInfos::Find(target);
	if (e)
	{
		OperInfos *op  = e->GetExt<OperInfos>("operinfo");
		if (op)
		{
			std::vector<OperInfo *>::iterator it = std::find((*op)->begin(), (*op)->end(), this);
			if (it != (*op)->end())
				(*op)->erase(it);
		}
	}
}

Serializable *OperInfoImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string starget;
	data["target"] >> starget;

	Extensible *e = OperInfos::Find(starget);
	if (!e)
		return NULL;

	OperInfos *oi = e->Require<OperInfos>("operinfo");
	OperInfoImpl *o;
	if (obj)
		o = anope_dynamic_static_cast<OperInfoImpl *>(obj);
	else
	{
		o = new OperInfoImpl();
		o->target = starget;
	}
	data["info"] >> o->info;
	data["adder"] >> o->adder;
	data["created"] >> o->created;

	if (!obj)
		(*oi)->push_back(o);
	return o;
}

class CommandOSInfo final
	: public Command
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

		Extensible *e;
		if (IRCD->IsChannelValid(target))
		{
			ChannelInfo *ci = ChannelInfo::Find(target);
			if (!ci)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, target.c_str());
				return;
			}

			e = ci;
		}
		else
		{
			NickAlias *na = NickAlias::Find(target);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, target.c_str());
				return;
			}

			e = na->nc;
		}

		if (cmd.equals_ci("ADD"))
		{
			if (info.empty())
			{
				this->OnSyntaxError(source, cmd);
				return;
			}

			OperInfos *oi = e->Require<OperInfos>("operinfo");

			if ((*oi)->size() >= Config->GetModule(this->module).Get<unsigned>("max", "10"))
			{
				source.Reply(_("The oper info list for \002%s\002 is full."), target.c_str());
				return;
			}

			for (auto *o : *(*oi))
			{
				if (o->info.equals_ci(info))
				{
					source.Reply(_("The oper info already exists on \002%s\002."), target.c_str());
					return;
				}
			}

			(*oi)->push_back(new OperInfoImpl(target, info, source.GetNick(), Anope::CurTime));

			source.Reply(_("Added info to \002%s\002."), target.c_str());
			Log(LOG_ADMIN, source, this) << "to add information to " << target;

			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);
		}
		else if (cmd.equals_ci("DEL"))
		{
			if (info.empty())
			{
				this->OnSyntaxError(source, cmd);
				return;
			}

			OperInfos *oi = e->GetExt<OperInfos>("operinfo");

			if (!oi)
			{
				source.Reply(_("Oper info list for \002%s\002 is empty."), target.c_str());
				return;
			}

			bool found = false;
			for (unsigned i = (*oi)->size(); i > 0; --i)
			{
				OperInfo *o = (*oi)->at(i - 1);

				if (o->info.equals_ci(info))
				{
					delete o;
					found = true;
					break;
				}
			}

			if (!found)
			{
				source.Reply(_("No such info \"%s\" on \002%s\002."), info.c_str(), target.c_str());
			}
			else
			{
				if ((*oi)->empty())
					e->Shrink<OperInfos>("operinfo");

				source.Reply(_("Deleted info from \002%s\002."), target.c_str());
				Log(LOG_ADMIN, source, this) << "to remove information from " << target;

				if (Anope::ReadOnly)
					source.Reply(READ_ONLY_MODE);
			}
		}
		else if (cmd.equals_ci("CLEAR"))
		{
			OperInfos *oi = e->GetExt<OperInfos>("operinfo");

			if (!oi)
			{
				source.Reply(_("Oper info list for \002%s\002 is empty."), target.c_str());
				return;
			}

			e->Shrink<OperInfos>("operinfo");

			source.Reply(_("Cleared info from \002%s\002."), target.c_str());
			Log(LOG_ADMIN, source, this) << "to clear information for " << target;

			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);
		}
		else
		{
			this->OnSyntaxError(source, cmd);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Add or delete oper information for a given nick or channel.\n"
				"This will show to opers in the respective info command for\n"
				"the nick or channel."));
		return true;
	}
};

class OSInfo final
	: public Module
{
	CommandOSInfo commandosinfo;
	ExtensibleItem<OperInfos> oinfo;
	Serialize::Type oinfo_type;

	void OnInfo(CommandSource &source, Extensible *e, InfoFormatter &info)
	{
		if (!source.IsOper())
			return;

		OperInfos *oi = oinfo.Get(e);
		if (!oi)
			return;

		for (auto *o : *(*oi))
		{
			info[_("Oper Info")] = Anope::printf(_("(by %s on %s) %s"), o->adder.c_str(), Anope::strftime(o->created, source.GetAccount(), true).c_str(), o->info.c_str());
		}
	}

public:
	OSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosinfo(this), oinfo(this, "operinfo"), oinfo_type("OperInfo", OperInfoImpl::Unserialize)
	{

	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		OnInfo(source, na->nc, info);
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) override
	{
		OnInfo(source, ci, info);
	}
};

MODULE_INIT(OSInfo)
