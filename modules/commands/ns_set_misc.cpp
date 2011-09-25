/*
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct MiscData : Anope::string, ExtensibleItem, Serializable<MiscData>
{
	NickCore *nc;
	Anope::string name;
	Anope::string data;

	MiscData(NickCore *ncore, const Anope::string &n, const Anope::string &d) : nc(ncore), name(n), data(d)
	{
	}

	serialized_data serialize()
	{
		serialized_data sdata;

		sdata["nc"] << this->nc->display;
		sdata["name"] << this->name;
		sdata["data"] << this->data;

		return sdata;
	}

	static void unserialize(serialized_data &data)
	{
		NickCore *nc = findcore(data["nc"].astr());
		if (nc == NULL)
			return;

		nc->Extend(data["name"].astr(), new MiscData(nc, data["name"].astr(), data["data"].astr()));
	}
};


class CommandNSSetMisc : public Command
{
 public:
	CommandNSSetMisc(Module *creator, const Anope::string &cname = "nickserv/set/misc", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetSyntax(_("[\037parameter\037]"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		Anope::string key = "ns_set_misc:" + source.command.replace_all_cs(" ", "_");
		nc->Shrink(key);
		if (!param.empty())
		{
			nc->Extend(key, new MiscData(nc, key, param));
			source.Reply(CHAN_SETTING_CHANGED, source.command.c_str(), nc->display.c_str(), param.c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, source.command.c_str(), nc->display.c_str());

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, source.u->Account()->display, !params.empty() ? params[0] : "");
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}
};

class NSSetMisc : public Module
{
	CommandNSSetMisc commandnssetmisc;
	CommandNSSASetMisc commandnssasetmisc;

 public:
	NSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetmisc(this), commandnssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickInfo };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		Serializable<MiscData>::Alloc.Register("NSMisc");
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, bool ShowHidden)
	{
		std::deque<Anope::string> list;
		na->nc->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("ns_set_misc:") != 0)
				continue;

			MiscData *data = na->nc->GetExt<MiscData *>(list[i]);
			if (data)
				source.Reply("      %s: %s", list[i].substr(12).replace_all_cs("_", " ").c_str(), data->data.c_str());
		}
	}
};

MODULE_INIT(NSSetMisc)
