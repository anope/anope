/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct NSMiscData : ExtensibleItem, Serializable
{
	Serialize::Reference<NickCore> nc;
	Anope::string name;
	Anope::string data;

	NSMiscData(NickCore *ncore, const Anope::string &n, const Anope::string &d) : Serializable("NSMiscData"), nc(ncore), name(n), data(d)
	{
	}

	void Serialize(Serialize::Data &sdata) const anope_override
	{
		sdata["nc"] << this->nc->display;
		sdata["name"] << this->name;
		sdata["data"] << this->data;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string snc, sname, sdata;

		data["nc"] >> snc;
		data["name"] >> sname;
		data["data"] >> sdata;

		NickCore *nc = NickCore::Find(snc);
		if (nc == NULL)
			return NULL;

		NSMiscData *d;
		if (obj)
		{
			d = anope_dynamic_static_cast<NSMiscData *>(obj);
			d->nc = nc;
			data["name"] >> d->name;
			data["data"] >> d->data;
		}
		else
		{
			d = new NSMiscData(nc, sname, sdata);
			nc->Extend(sname, d);
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

class CommandNSSetMisc : public Command
{
 public:
	CommandNSSetMisc(Module *creator, const Anope::string &cname = "nickserv/set/misc", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetSyntax(_("[\037parameter\037]"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Anope::string scommand = GetAttribute(source.command);
		Anope::string key = "ns_set_misc:" + scommand;
		nc->Shrink(key);
		if (!param.empty())
		{
			nc->Extend(key, new NSMiscData(nc, key, param));
			source.Reply(CHAN_SETTING_CHANGED, scommand.c_str(), nc->display.c_str(), param.c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, scommand.c_str(), nc->display.c_str());

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, !params.empty() ? params[0] : "");
	}
};

class CommandNSSASetMisc : public CommandNSSetMisc
{
 public:
	CommandNSSASetMisc(Module *creator) : CommandNSSetMisc(creator, "nickserv/saset/misc", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 [\037parameter\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}
};

class NSSetMisc : public Module
{
	Serialize::Type nsmiscdata_type;
	CommandNSSetMisc commandnssetmisc;
	CommandNSSASetMisc commandnssasetmisc;

 public:
	NSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		nsmiscdata_type("NSMiscData", NSMiscData::Unserialize), commandnssetmisc(this), commandnssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickInfo };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool ShowHidden) anope_override
	{
		std::deque<Anope::string> list;
		na->nc->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("ns_set_misc:") != 0)
				continue;

			NSMiscData *data = na->nc->GetExt<NSMiscData *>(list[i]);
			if (data)
				info[list[i].substr(12).replace_all_cs("_", " ")] = data->data;
		}
	}
};

MODULE_INIT(NSSetMisc)
