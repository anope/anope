/* ChanServ core functions
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

static std::map<Anope::string, char> defaultFlags;

class FlagsChanAccess : public ChanAccess
{
 public:
	std::set<char> flags;

 	FlagsChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool HasPriv(const Anope::string &priv) const anope_override
	{
		std::map<Anope::string, char>::iterator it = defaultFlags.find(priv);
		if (it != defaultFlags.end() && this->flags.count(it->second) > 0)
			return true;
		return false;
	}

	Anope::string AccessSerialize() const
	{
		return Anope::string(this->flags.begin(), this->flags.end());
	}

	void AccessUnserialize(const Anope::string &data) anope_override
	{
		for (unsigned i = data.length(); i > 0; --i)
			this->flags.insert(data[i - 1]);
	}

	static Anope::string DetermineFlags(const ChanAccess *access)
	{
		if (access->provider->name == "access/flags")
			return access->AccessSerialize();
		
		std::set<char> buffer;

		for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
			if (access->HasPriv(it->first))
				buffer.insert(it->second);

		return Anope::string(buffer.begin(), buffer.end());
	}
};

class FlagsAccessProvider : public AccessProvider
{
 public:
	FlagsAccessProvider(Module *o) : AccessProvider(o, "access/flags")
	{
	}

	ChanAccess *Create() anope_override
	{
		return new FlagsChanAccess(this);
	}
};

class CommandCSFlags : public Command
{
	void DoModify(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";
		Anope::string flags = params.size() > 3 ? params[3] : "";

		if (flags.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		AccessGroup u_access = source.AccessFor(ci);

		if (mask.find_first_of("!*@") == Anope::string::npos && !NickAlias::Find(mask))
		{
			User *targ = User::Find(mask, true);
			if (targ != NULL)
				mask = "*!*@" + targ->GetDisplayedHost();
			else
			{
				source.Reply(NICK_X_NOT_REGISTERED, mask.c_str());
				return;
			}
		}

		ChanAccess *current = NULL;
		std::set<char> current_flags;
		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanAccess *access = ci->GetAccess(i - 1);
			if (mask.equals_ci(access->mask))
			{
				current = access;
				Anope::string cur_flags = FlagsChanAccess::DetermineFlags(access);
				for (unsigned j = cur_flags.length(); j > 0; --j)
					current_flags.insert(cur_flags[j - 1]);
				break;
			}
		}

		if (ci->GetAccessCount() >= Config->CSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel."), Config->CSAccessMax);
			return;
		}

		bool override = false;
		int add = -1;
		for (size_t i = 0; i < flags.length(); ++i)
		{
			char f = flags[i];
			switch (f)
			{
				case '+':
					add = 1;
					break;
				case '-':
					add = 0;
					break;
				case '*':
					if (add == -1)
						break;
					for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
					{
						if (!u_access.HasPriv(it->first))
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
								continue;
						}
						if (add == 1)
							current_flags.insert(it->second);
						else if (add == 0)
							current_flags.erase(it->second);
					}
					break;
				default:
					if (add == -1)
						break;
					for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
					{
						if (f != it->second)
							continue;
						else if (!u_access.HasPriv(it->first))
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
							{
								source.Reply(_("You can not set the \002%c\002 flag."), f);
								break;
							}
						}
						if (add == 1)
							current_flags.insert(f);
						else if (add == 0)
							current_flags.erase(f);
						break;
					}
			}
		}
		if (current_flags.empty())
		{
			if (current != NULL)
			{
				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, source, current));
				ci->EraseAccess(current);
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;
				source.Reply(_("\002%s\002 removed from the %s access list."), mask.c_str(), ci->name.c_str());
			}
			else
			{
				source.Reply(_("Insufficient flags given."));
			}
			return;
		}

		ServiceReference<AccessProvider> provider("AccessProvider", "access/flags");
		if (!provider)
			return;
		FlagsChanAccess *access = anope_dynamic_static_cast<FlagsChanAccess *>(provider->Create());
		access->ci = ci;
		access->mask = mask;
		access->creator = source.GetNick();
		access->last_seen = current ? current->last_seen : 0;
		access->created = Anope::CurTime;
		access->flags = current_flags;

		if (current != NULL)
			ci->EraseAccess(current);

		ci->AddAccess(access);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, source, access));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to modify " << mask << "'s flags to " << access->AccessSerialize();
		source.Reply(_("Access for \002%s\002 on %s set to +\002%s\002"), access->mask.c_str(), ci->name.c_str(), access->AccessSerialize().c_str());

		return;
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &arg = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s access list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list;

		list.AddColumn("Number").AddColumn("Mask").AddColumn("Flags").AddColumn("Creator").AddColumn("Created");

		unsigned count = 0;
		for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
		{
			const ChanAccess *access = ci->GetAccess(i);

			if (!arg.empty())
			{
				const Anope::string &flags = FlagsChanAccess::DetermineFlags(access);

				if (arg[0] == '+')
				{
					bool pass = true;
					for (size_t j = 1; j < arg.length(); ++j)
						if (flags.find(arg[j]) == Anope::string::npos)
							pass = false;
					if (pass == false)
						continue;
				}
				else if (!Anope::Match(access->mask, arg))
					continue;
			}

			ListFormatter::ListEntry entry;
			++count;
			entry["Number"] = stringify(i + 1);
			entry["Mask"] = access->mask;
			entry["Flags"] = FlagsChanAccess::DetermineFlags(access);
			entry["Creator"] = access->creator;
			entry["Created"] = Anope::strftime(access->created, source.nc, true);
			list.AddEntry(entry);
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Flags list for %s"), ci->name.c_str());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
			if (count == ci->GetAccessCount())
				source.Reply(_("End of access list."));
			else
				source.Reply(_("End of access list - %d/%d entries shown."), count, ci->GetAccessCount());
		}
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		if (!source.IsFounder(ci) && !source.HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, source));

			source.Reply(_("Channel %s access list has been cleared."), ci->name.c_str());

			bool override = !source.IsFounder(ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";
		}

		return;
	}

 public:
	CommandCSFlags(Module *creator) : Command(creator, "chanserv/flags", 2, 4)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 MODIFY \037mask\037 \037changes\037"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | +\037flags\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		bool is_list = cmd.equals_ci("LIST");
		bool has_access = false;
		if (source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && source.AccessFor(ci).HasPriv("ACCESS_LIST"))
			has_access = true;
		else if (source.AccessFor(ci).HasPriv("ACCESS_CHANGE"))
			has_access = true;

		if (!has_access)
			source.Reply(ACCESS_DENIED);
		else if (Anope::ReadOnly && !is_list)
			source.Reply(_("Sorry, channel access list modification is temporarily disabled."));
		else if (cmd.equals_ci("MODIFY"))
			this->DoModify(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, cmd);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("%s is another way to modify the channel access list, similar to\n"
				"the XOP and ACCESS methods."), source.command.c_str());
		source.Reply(" ");
		source.Reply(_("The \002MODIFY\002 command allows you to modify the access list. If mask is\n"
				"not already on the access list is it added, then the changes are applied.\n"
				"If the mask has no more flags, then the mask is removed from the access list.\n"
				"Additionally, you may use +* or -* to add or remove all flags, respectively. You are\n"
				"only able to modify the access list if you have the proper permission on the channel,\n"
				"and even then you can only give other people access to up what you already have."));
		source.Reply(" ");
		source.Reply(_("The \002LIST\002 command allows you to list existing entries on the channel access list.\n"
				"If a mask is given, the mask is wildcard matched against all existing entries on the\n"
				"access list, and only those entries are returned. If a set of flags is given, only those\n"
				"on the access list with the specified flags are returned."));
		source.Reply(" ");
		source.Reply(_("The \002CLEAR\002 command clears the channel access list, which requires channel founder."));
		source.Reply(" ");
		source.Reply(_("The available flags are:"));

		typedef std::multimap<char, Anope::string, ci::less> reverse_map;
		reverse_map reverse;
		for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
			reverse.insert(std::make_pair(it->second, it->first));

		for (reverse_map::iterator it = reverse.begin(), it_end = reverse.end(); it != it_end; ++it)
		{
			Privilege *p = PrivilegeManager::FindPrivilege(it->second);
			if (p == NULL)
				continue;
			source.Reply("  %c - %s", it->first, Language::Translate(source.nc, p->desc.c_str()));
		}

		return true;
	}
};

class CSFlags : public Module
{
	FlagsAccessProvider accessprovider;
	CommandCSFlags commandcsflags;

 public:
	CSFlags(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		accessprovider(this), commandcsflags(this)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, 1);

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		defaultFlags.clear();

		for (int i = 0; i < config.Enumerate("privilege"); ++i)
		{
			const Anope::string &pname = config.ReadValue("privilege", "name", "", i);

			Privilege *p = PrivilegeManager::FindPrivilege(pname);
			if (p == NULL)
				continue;

			const Anope::string &value = config.ReadValue("privilege", "flag", "", i);
			if (value.empty())
				continue;

			defaultFlags[p->name] = value[0];
		}
	}
};

MODULE_INIT(CSFlags)
