/* ChanServ core functions
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

static std::map<Anope::string, char> defaultFlags;

class FlagsChanAccess : public ChanAccess
{
 public:
	std::set<char> flags;

 	FlagsChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool Matches(User *u, NickCore *nc)
	{
		if (u && this->mask.find_first_of("!@?*") != Anope::string::npos && (Anope::Match(u->nick, this->mask) || Anope::Match(u->GetMask(), this->mask)))
			return true;
		else if (nc && Anope::Match(nc->display, this->mask))
			return true;
		return false;
	}

	bool HasPriv(const Anope::string &priv) const
	{
		std::map<Anope::string, char>::iterator it = defaultFlags.find(priv);
		if (it != defaultFlags.end() && this->flags.count(it->second) > 0)
			return true;
		return false;
	}

	Anope::string Serialize()
	{
		return Anope::string(this->flags.begin(), this->flags.end());
	}

	void Unserialize(const Anope::string &data)
	{
		for (unsigned i = data.length(); i > 0; --i)
			this->flags.insert(data[i - 1]);
	}

	static Anope::string DetermineFlags(ChanAccess *access)
	{
		if (access->provider->name == "access/flags")
			return access->Serialize();
		
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

	ChanAccess *Create()
	{
		return new FlagsChanAccess(this);
	}
};

class CommandCSFlags : public Command
{
	void DoModify(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string mask = params.size() > 2 ? params[2] : "";
		Anope::string flags = params.size() > 3 ? params[3] : "";

		if (flags.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		AccessGroup u_access = ci->AccessFor(u);

		if (mask.find_first_of("!*@") == Anope::string::npos && findnick(mask) == NULL)
			mask += "!*@*";

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
							if (u->HasPriv("chanserv/access/modify"))
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
							if (u->HasPriv("chanserv/access/modify"))
								override = true;
							else
							{
								source.Reply(_("You can not set the \002%c\002 flag."), f);
								continue;
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
				FOREACH_MOD(I_OnAccessDel, OnAccessDel(ci, u, current));
				ci->EraseAccess(current);
				source.Reply(_("\002%s\002 removed from the %s access list."), mask.c_str(), ci->name.c_str());
			}
			else
			{
				source.Reply(_("Insufficient flags given"));
			}
			return;
		}

		service_reference<AccessProvider> provider("access/flags");
		if (!provider)
			return;
		FlagsChanAccess *access = debug_cast<FlagsChanAccess *>(provider->Create());
		access->ci = ci;
		access->mask = mask;
		access->creator = u->nick;
		access->last_seen = current ? current->last_seen : 0;
		access->created = Anope::CurTime;
		access->flags = current_flags;

		if (current != NULL)
			ci->EraseAccess(current);

		ci->AddAccess(access);

		FOREACH_MOD(I_OnAccessAdd, OnAccessAdd(ci, u, access));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "MODIFY " << mask << " with flags " << access->Serialize();
		source.Reply(_("Access for \002%s\002 on %s set to +\002%s\002"), access->mask.c_str(), ci->name.c_str(), access->Serialize().c_str());

		return;
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &arg = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else
		{
			unsigned total = 0;

			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanAccess *access = ci->GetAccess(i);
				const Anope::string &flags = FlagsChanAccess::DetermineFlags(access);

				if (!arg.empty())
				{
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

				if (++total == 1)
				{
					source.Reply(_("Flags list for %s"), ci->name.c_str());
				}

				source.Reply(_("  %3d  %-10s +%-10s [last modified on %s by %s]"), i + 1, access->mask.c_str(), FlagsChanAccess::DetermineFlags(access).c_str(), do_strftime(access->created, source.u->Account(), true).c_str(), access->creator.c_str());
			}

			if (total == 0)
				source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
			else if (total == ci->GetAccessCount())
				source.Reply(_("End of access list."));
			else
				source.Reply(_("End of access list - %d/%d entries shown."), total, ci->GetAccessCount());
		}
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;

		if (!IsFounder(u, ci) && !u->HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(I_OnAccessClear, OnAccessClear(ci, u));

			source.Reply(_("Channel %s access list has been cleared."), ci->name.c_str());

			bool override = !IsFounder(u, ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";
		}

		return;
	}

 public:
	CommandCSFlags(Module *creator) : Command(creator, "chanserv/flags", 2, 4)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 MODIFY \037mask\037 \037changes\037"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | +\037flags\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		bool is_list = cmd.equals_ci("LIST");
		bool has_access = false;
		if (u->HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && ci->AccessFor(u).HasPriv("ACCESS_LIST"))
			has_access = true;
		else if (ci->AccessFor(u).HasPriv("ACCESS_CHANGE"))
			has_access = true;

		if (!has_access)
			source.Reply(ACCESS_DENIED);
		else if (readonly && !is_list)
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("%s is another way to modify the channel access list, similar to\n"
				"the XOP and ACCESS methods."), source.command.c_str());
		source.Reply(" ");
		source.Reply(_("The MODIFY command allows you to modify the access list. If mask is\n"
				"not already on the access list is it added, then the changes are applied.\n"
				"If the mask has no more flags, then the mask is removed from the access list.\n"
				"Additionally, you may use +* or -* to add or remove all flags, respectively. You are\n"
				"only able to modify the access list if you have the proper permission on the channel,\n"
				"and even then you can only give other people access to up what you already have."));
		source.Reply(" ");
		source.Reply(_("The LIST command allows you to list existing entries on the channel access list.\n"
				"If a mask is given, the mask is wildcard matched against all existing entries on the\n"
				"access list, and only those entries are returned. If a set of flags is given, only those\n"
				"on the access list with the specified flags are returned."));
		source.Reply(" ");
		source.Reply(_("The CLEAR command clears the channel access list, which requires channel founder."));
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
			source.Reply("  %c - %s", it->first, translate(source.u->Account(), p->desc.c_str()));
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


		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, 1);

		this->OnReload();
	}

	void OnReload()
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
