/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_access.h"

static std::map<Anope::string, char> defaultFlags;

class FlagsChanAccess : public ChanServ::ChanAccess
{
 public:
	FlagsChanAccess(Serialize::TypeBase *type) : ChanServ::ChanAccess(type) { }
	FlagsChanAccess(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::ChanAccess(type, id) { }

	Anope::string GetFlags();
	void SetFlags(const Anope::string &);

	bool HasPriv(const Anope::string &priv) override
	{
		std::map<Anope::string, char>::iterator it = defaultFlags.find(priv);
		return it != defaultFlags.end() && GetFlags().find(it->second) != Anope::string::npos;
	}

	Anope::string AccessSerialize()
	{
		return GetFlags();
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		SetFlags(data);
	}

	static Anope::string DetermineFlags(ChanServ::ChanAccess *access)
	{
		if (access->GetSerializableType()->GetName() != "FlagsChanAccess")
			return access->AccessSerialize();

		std::set<char> buffer;

		for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
			if (access->HasPriv(it->first))
				buffer.insert(it->second);

		if (buffer.empty())
			return "(none)";
		else
			return Anope::string(buffer.begin(), buffer.end());
	}
};

class FlagsChanAccessType : public Serialize::Type<FlagsChanAccess, ChanServ::ChanAccessType>
{
 public:
	Serialize::Field<FlagsChanAccess, Anope::string> flags;
	//Serialize::Field<FlagsChanAccess, std::set<char>>> flags; XXX?

	FlagsChanAccessType(Module *me) : Serialize::Type<FlagsChanAccess, ChanAccessType>(me, "FlagsChanAccess")
		, flags(this, "flags")
	{
		SetParent(ChanServ::chanaccess);
	}
};

Anope::string FlagsChanAccess::GetFlags()
{
	return Get(&FlagsChanAccessType::flags);
}

void FlagsChanAccess::SetFlags(const Anope::string &i)
{
	Object::Set(&FlagsChanAccessType::flags, i);
}

class CommandCSFlags : public Command
{
	void DoModify(CommandSource &source, ChanServ::Channel *ci, Anope::string mask, const Anope::string &flags)
	{
		if (flags.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		ChanServ::AccessGroup u_access = source.AccessFor(ci);
		ChanServ::ChanAccess *highest = u_access.Highest();

		NickServ::Nick *na = nullptr;
		ChanServ::Channel *targ_ci = nullptr;

		if (IRCD->IsChannelValid(mask))
		{
			if (Config->GetModule("chanserv")->Get<bool>("disallow_channel_access"))
			{
				source.Reply(_("Channels may not be on access lists."));
				return;
			}

			targ_ci = ChanServ::Find(mask);
			if (targ_ci == NULL)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), mask);
				return;
			}
			else if (ci == targ_ci)
			{
				source.Reply(_("You can't add a channel to its own access list."));
				return;
			}

			mask = targ_ci->GetName();
		}
		else
		{
			na = NickServ::FindNick(mask);
			if (!na && Config->GetModule("chanserv")->Get<bool>("disallow_hostmask_access"))
			{
				source.Reply(_("Masks and unregistered users may not be on access lists."));
				return;
			}
			else if (mask.find_first_of("!*@") == Anope::string::npos && !na)
			{
				User *targ = User::Find(mask, true);
				if (targ != NULL)
					mask = "*!*@" + targ->GetDisplayedHost();
				else
				{
					source.Reply(_("\002{0}\002 isn't registered."), mask);
					return;
				}
			}

			if (na)
				mask = na->GetNick();
		}

		ChanServ::ChanAccess *current = NULL;
		unsigned current_idx;
		std::set<char> current_flags;
		bool override = false;
		for (current_idx = ci->GetAccessCount(); current_idx > 0; --current_idx)
		{
			ChanServ::ChanAccess *access = ci->GetAccess(current_idx - 1);
			if ((na && na->GetAccount() == access->GetAccount()) || mask.equals_ci(access->Mask()))
			{
				// Flags allows removing others that have the same access as you,
				// but no other access system does.
				if (highest && highest->GetSerializableType()->GetName() != "FlagsChanAccess" && !u_access.founder)
					// operator<= on the non-me entry!
					if (*highest <= *access)
					{
						if (source.HasPriv("chanserv/access/modify"))
							override = true;
						else
						{
							source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to modify the access of \002{1}\002."), ci->GetName(), access->Mask());
							return;
						}
					}

				current = access;
				Anope::string cur_flags = FlagsChanAccess::DetermineFlags(access);
				for (unsigned j = cur_flags.length(); j > 0; --j)
					current_flags.insert(cur_flags[j - 1]);
				break;
			}
		}

		unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
		if (access_max && ci->GetDeepAccessCount() >= access_max)
		{
			source.Reply(_("Sorry, you can only have \002{0}\002 access entries on a channel, including access entries from other channels."), access_max);
			return;
		}

		ChanServ::Privilege *p = NULL;
		bool add = true;
		for (size_t i = 0; i < flags.length(); ++i)
		{
			char f = flags[i];
			switch (f)
			{
				case '+':
					add = true;
					break;
				case '-':
					add = false;
					break;
				case '*':
					for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
					{
						bool has = current_flags.count(it->second);
						// If we are adding a flag they already have or removing one they don't have, don't bother
						if (add == has)
							continue;

						if (!u_access.HasPriv(it->first) && !u_access.founder)
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
								continue;
						}

						if (add)
							current_flags.insert(it->second);
						else
							current_flags.erase(it->second);
					}
					break;
				default:
					p = ChanServ::service ? ChanServ::service->FindPrivilege(flags.substr(i)) : nullptr;
					if (p != NULL && defaultFlags[p->name])
					{
						f = defaultFlags[p->name];
						i = flags.length();
					}

					for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
					{
						if (f != it->second)
							continue;
						else if (!u_access.HasPriv(it->first) && !u_access.founder)
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
							{
								source.Reply(_("You can not set the \002{0}\002 flag."), f);
								break;
							}
						}
						if (add)
							current_flags.insert(f);
						else
							current_flags.erase(f);
						break;
					}
			}
		}
		if (current_flags.empty())
		{
			if (current != NULL)
			{
				Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, current);
				delete current;
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;
				source.Reply(_("\002{0}\002 removed from the access list of \002{1}\002."), mask, ci->GetName());
			}
			else
			{
				source.Reply(_("\002{0}\002 not found on the access list of \002{1}\002."), mask, ci->GetName());
			}
			return;
		}

		FlagsChanAccess *access = anope_dynamic_static_cast<FlagsChanAccess *>(flagschanaccess.Create());
		if (na)
			access->SetObj(na->GetAccount());
		else if (targ_ci)
			access->SetObj(targ_ci);
		access->SetChannel(ci);
		access->SetMask(mask);
		access->SetCreator(source.GetNick());
		access->SetLastSeen(current ? current->GetLastSeen() : 0);
		access->SetCreated(Anope::CurTime);
		access->SetFlags(Anope::string(current_flags.begin(), current_flags.end()));

		if (current != NULL)
			delete current;

		Event::OnAccessAdd(&Event::AccessAdd::OnAccessAdd, ci, source, access);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to modify " << mask << "'s flags to " << access->AccessSerialize();
		if (p != NULL)
		{
			if (add)
				source.Reply(_("Privilege \002{0}\002 added to \002{1}\002 on \002{2}\002, new flags are +\002{3}\002"), p->name, access->Mask(), ci->GetName(), access->AccessSerialize());
			else
				source.Reply(_("Privilege \002{0}\002 removed from \002{1}\002 on \002{2}\002, new flags are +\002{3}\002"), p->name, access->Mask(), ci->GetName(), access->AccessSerialize());
		}
		else
			source.Reply(_("Flags for \002{0}\002 on \002{1}\002 set to +\002{2}\002"), access->Mask(), ci->GetName(), access->AccessSerialize());
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &arg = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list of \002{0}\002 is empty."), ci->GetName());
			return;
		}

		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Flags")).AddColumn(_("Creator")).AddColumn(_("Created"));

		unsigned count = 0;
		for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
		{
			ChanServ::ChanAccess *access = ci->GetAccess(i);
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
				else if (!Anope::Match(access->Mask(), arg))
					continue;
			}

			ListFormatter::ListEntry entry;
			++count;
			entry["Number"] = stringify(i + 1);
			entry["Mask"] = access->Mask();
			entry["Flags"] = flags;
			entry["Creator"] = access->GetCreator();
			entry["Created"] = Anope::strftime(access->GetCreated(), source.nc, true);
			list.AddEntry(entry);
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on the access list of \002{0}\002."), ci->GetName());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Flags list for \002{0}\002:"), ci->GetName());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
			if (count == ci->GetAccessCount())
				source.Reply(_("End of access list."));
			else
				source.Reply(_("End of access list - \002{0}/{1}\002 entries shown."), count, ci->GetAccessCount());
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		if (!source.IsFounder(ci) && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		ci->ClearAccess();

		Event::OnAccessClear(&Event::AccessClear::OnAccessClear, ci, source);

		source.Reply(_("The access list of \002{0}\002 has been cleared."), ci->GetName());

		bool override = !source.IsFounder(ci);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";
	}

 public:
	CommandCSFlags(Module *creator) : Command(creator, "chanserv/flags", 1, 4)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 [MODIFY] \037mask\037 \037changes\037"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | +\037flags\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params.size() > 1 ? params[1] : "";

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		bool is_list = cmd.empty() || cmd.equals_ci("LIST");
		bool has_access = false;
		if (source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && source.HasPriv("chanserv/access/list"))
			has_access = true;
		else if (is_list && source.AccessFor(ci).HasPriv("ACCESS_LIST"))
			has_access = true;
		else if (source.AccessFor(ci).HasPriv("ACCESS_CHANGE"))
			has_access = true;

		if (!has_access)
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), is_list ? "ACCESS_LIST" : "ACCESS_CHANGE", ci->GetName());
			return;
		}

		if (Anope::ReadOnly && !is_list)
		{
			source.Reply(_("Sorry, channel access list modification is temporarily disabled."));
			return;
		}

		if (is_list)
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
		{
			Anope::string mask, flags;
			if (cmd.equals_ci("MODIFY"))
			{
				mask = params.size() > 2 ? params[2] : "";
				flags = params.size() > 3 ? params[3] : "";
			}
			else
			{
				mask = cmd;
				flags = params.size() > 2 ? params[2] : "";
			}

			this->DoModify(source, ci, mask, flags);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("{0} allow granularly granting privileges on channels to users by assigning them flags, which map to one or more privileges.\n"
		               "\n"
		               "The \002MODIFY\002 command allows you to modify the access list."
		               " If \037mask\037 is not already on the access list is it added, then the \037changes\037 are applied."
		               " If the mask has no more flags, then the mask is removed from the access list."
		               " Additionally, you may use +* or -* to add or remove all flags, respectively."
		               " You are only able to modify the access list if you have the \002{1}\002 privilege on the channel, and can only give privileges to up what you have."),
		               source.command, "ACCESS_CHANGE");
		source.Reply(" ");
		source.Reply(_("The \002LIST\002 command allows you to list existing entries on the channel access list."
		               " If \037mask\037 is given, the \037mask\037 is wildcard matched against all existing entries on the access list, and only those entries are returned."
		               " If a set of flags is given, only those on the access list with the specified flags are returned."));
		source.Reply(" ");
		source.Reply(_("The \002CLEAR\002 command clears the channel access list, which requires being the founder of \037channel\037."));
		source.Reply(" ");
		source.Reply(_("\n"
		               "Examples:\n"
		               "         {command} #anope MODIFY Attila +fHhu-i\n"
		               "          Modifies the flags of \"Attila\" on the access list of \"#anope\" to \"+fHhu-i\".\n"
		               "\n"
		               "         {command} #anope MODIFY *!*@anope.org +*\n"
		               "         Modifies the flags of the host mask \"*!*@anope.org\" on the access list of \"#anope\" to have all flags."));
		source.Reply(" ");
		source.Reply(_("The available flags are:"));

		typedef std::multimap<char, Anope::string, ci::less> reverse_map;
		reverse_map reverse;
		for (std::map<Anope::string, char>::iterator it = defaultFlags.begin(), it_end = defaultFlags.end(); it != it_end; ++it)
			reverse.insert(std::make_pair(it->second, it->first));

		for (reverse_map::iterator it = reverse.begin(), it_end = reverse.end(); it != it_end; ++it)
		{
			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(it->second) : nullptr;
			if (p == NULL)
				continue;
			source.Reply("  %c - %s", it->first, Language::Translate(source.nc, p->desc.c_str()));
		}

		return true;
	}
};

class CSFlags : public Module
{
	CommandCSFlags commandcsflags;
	FlagsChanAccessType flagsaccesstype;

 public:
	CSFlags(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsflags(this)
		, flagsaccesstype(this)
	{
		this->SetPermanent(true);

	}

	void OnReload(Configuration::Conf *conf) override
	{
		defaultFlags.clear();

		for (int i = 0; i < conf->CountBlock("privilege"); ++i)
		{
			Configuration::Block *priv = conf->GetBlock("privilege", i);

			const Anope::string &pname = priv->Get<Anope::string>("name");

			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(pname) : nullptr;
			if (p == NULL)
				continue;

			const Anope::string &value = priv->Get<Anope::string>("flag");
			if (value.empty())
				continue;

			defaultFlags[p->name] = value[0];
		}
	}
};

MODULE_INIT(CSFlags)
