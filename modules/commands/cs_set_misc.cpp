/*
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
	ChannelInfo *ci;
	Anope::string name;
	Anope::string data;

	MiscData(ChannelInfo *c, const Anope::string &n, const Anope::string &d) : ci(c), name(n), data(d)
	{
	}

	serialized_data serialize()
	{
		serialized_data sdata;

		sdata["ci"] << this->ci->name;
		sdata["name"] << this->name;
		sdata["data"] << this->data;

		return sdata;
	}

	static void unserialize(serialized_data &data)
	{
		ChannelInfo *ci = cs_findchan(data["ci"].astr());
		if (ci == NULL)
			return;

		ci->Extend(data["name"].astr(), new MiscData(ci, data["name"].astr(), data["data"].astr()));
	}
};

class CommandCSSetMisc : public Command
{
 public:
	CommandCSSetMisc(Module *creator, const Anope::string &cname = "chanserv/set/misc") : Command(creator, cname, 1, 1)
	{
		this->SetSyntax(_("\037channel\037 [\037parameters\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		else if (source.permission.empty() && !ci->AccessFor(source.u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		Anope::string key = "cs_set_misc:" + source.command.replace_all_cs(" ", "_");
		ci->Shrink(key);
		if (params.size() > 1)
		{
			ci->Extend(key, new MiscData(ci, key, params[1]));
			source.Reply(CHAN_SETTING_CHANGED, source.command.c_str(), ci->name.c_str(), params[1].c_str());
		}
		else
			source.Reply(CHAN_SETTING_UNSET, source.command.c_str(), ci->name.c_str());
	}
};

class CommandCSSASetMisc : public CommandCSSetMisc
{
 public:
	CommandCSSASetMisc(Module *creator) : CommandCSSetMisc(creator, "chanserv/saset/misc")
	{
	}
};

class CSSetMisc : public Module
{
	CommandCSSetMisc commandcssetmisc;
	CommandCSSASetMisc commandcssasetmisc;

 public:
	CSSetMisc(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetmisc(this), commandcssasetmisc(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnChanInfo };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		Serializable<MiscData>::Alloc.Register("CSMisc");
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, bool ShowHidden)
	{
		std::deque<Anope::string> list;
		ci->GetExtList(list);

		for (unsigned i = 0; i < list.size(); ++i)
		{
			if (list[i].find("cs_set_misc:") != 0)
				continue;
			
			MiscData *data = ci->GetExt<MiscData *>(list[i]);
			if (data != NULL)
				source.Reply("      %s: %s", list[i].substr(12).replace_all_cs("_", " ").c_str(), data->data.c_str());
		}
	}
};

MODULE_INIT(CSSetMisc)
