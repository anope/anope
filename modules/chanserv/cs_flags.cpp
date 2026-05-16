// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

#define FLAGS_MIGRATED_1        _("\002%s\002 has been migrated to the %s access system.")
#define FLAGS_MIGRATED_N       N_("\002%u\002 entry has been migrated to the %s access system.", "\002%u\002 entries have been migrated to the %s access system.")
#define FLAGS_NOT_MIGRATABLE_1  _("\002%s\002 can not be migrated to the %s access system because they have no migratable privileges.")
#define FLAGS_NOT_MIGRATABLE_N N_("\002%u\002 entry can not be migrated to the %s access system because they have no migratable privileges.", "\002%u\002 entries can not be migrated to the %s access system because they have no migratable privileges.")
#define FLAGS_NOT_MIGRATED_1    _("\002%s\002 can not be migrated to the %s access system because they have privileges that you do not.")
#define FLAGS_NOT_MIGRATED_N   N_("\002%u\002 entry can not be migrated to the %s access system because they have privileges that you do not.", "\002%u\002 entries can not be migrated to the %s access system because they have privileges that you do not.")

static std::map<Anope::string, char> defaultFlags;
static Anope::map<Anope::string> migrationRequires;

class FlagsChanAccess final
	: public ChanAccess
{
public:
	std::set<char> flags;

	FlagsChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool HasPriv(const Anope::string &priv) const override
	{
		auto it = defaultFlags.find(priv);
		return it != defaultFlags.end() && this->flags.count(it->second) > 0;
	}

	Anope::string AccessSerialize() const override
	{
		return Anope::string(this->flags.begin(), this->flags.end());
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		for (unsigned i = data.length(); i > 0; --i)
			this->flags.insert(data[i - 1]);
	}

	static Anope::string DetermineFlags(const ChanAccess *access)
	{
		if (access->provider->name == "access/flags")
			return access->AccessSerialize();

		std::set<char> buffer;

		for (auto &[priv, flag] : defaultFlags)
			if (access->HasPriv(priv))
				buffer.insert(flag);

		if (buffer.empty())
			return "(none)";
		else
			return Anope::string(buffer.begin(), buffer.end());
	}
};

class FlagsAccessProvider final
	: public AccessProvider
{
public:
	static FlagsAccessProvider *ap;

	FlagsAccessProvider(Module *o) : AccessProvider(o, "access/flags")
	{
		ap = this;
	}

	ChanAccess *Create() override
	{
		return new FlagsChanAccess(this);
	}
};
FlagsAccessProvider *FlagsAccessProvider::ap;

class CommandCSFlags final
	: public Command
{
	void DoModify(CommandSource &source, ChannelInfo *ci, Anope::string mask, const Anope::string &flags, Anope::string description)
	{
		if (flags.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		AccessGroup u_access = source.AccessFor(ci);
		const ChanAccess *highest = u_access.Highest();
		const NickAlias *na = NULL;

		const auto &csconf = Config->GetModule("chanserv");
		if (IRCD->IsChannelValid(mask))
		{
			if (csconf.Get<bool>("disallow_channel_access"))
			{
				source.Reply(_("Channels may not be on access lists."));
				return;
			}

			ChannelInfo *targ_ci = ChannelInfo::Find(mask);
			if (targ_ci == NULL)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, mask.c_str());
				return;
			}
			else if (ci == targ_ci)
			{
				source.Reply(_("You can't add a channel to its own access list."));
				return;
			}

			mask = targ_ci->name;
		}
		else
		{
			na = NickAlias::Find(mask);
			if (na)
			{
				if (na->nc->HasExt("NEVEROP"))
				{
					source.Reply(_("\002%s\002 does not wish to be added to channel access lists."),
						na->nc->display.c_str());
					return;
				}
				mask = na->nick;
			}
			else
			{
				if (csconf.Get<bool>("disallow_hostmask_access"))
				{
					source.Reply(_("Masks and unregistered users may not be on access lists."));
					return;
				}

				if (mask.find_first_of("!*@") == Anope::string::npos)
				{
					auto *targ = User::Find(mask, true);
					if (!targ)
					{
						source.Reply(NICK_X_NOT_IN_USE, mask.c_str());
						return;
					}

					auto *targnc = targ->Account();
					if (!targnc)
					{
						source.Reply(NICK_X_NOT_REGISTERED, targ->nick.c_str());
						return;
					}

					mask = targnc->display;
					if (description.empty())
						description = targ->nick;
				}
				else
				{
					// Normalize the entry mask.
					const auto cleanmask = Entry(mask).GetCleanMask();
					if (csconf.Get<bool>("disallow_malformed_hostmask") && cleanmask != mask)
					{
						source.Reply(CHAN_ACCESS_MALFORMED, cleanmask.c_str());
						return;
					}

					mask = cleanmask;
				}
			}
		}

		ChanAccess *current = NULL;
		unsigned current_idx;
		std::set<char> current_flags;
		bool override = false;
		for (current_idx = ci->GetAccessCount(); current_idx > 0; --current_idx)
		{
			ChanAccess *access = ci->GetAccess(current_idx - 1);
			if ((na && na->nc == access->GetAccount()) || mask.equals_ci(access->Mask()))
			{
				// Flags allows removing others that have the same access as you,
				// but no other access system does.
				if (highest && highest->provider != FlagsAccessProvider::ap && !u_access.founder)
					// operator<= on the non-me entry!
					if (*highest <= *access)
					{
						if (source.HasPriv("chanserv/access/modify"))
							override = true;
						else
						{
							source.Reply(ACCESS_DENIED);
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

		const auto access_count = ci->GetDeepAccessCount();
		const auto access_max = Config->GetModule("chanserv").Get<unsigned>("accessmax", "1000");
		if (access_max && access_count >= access_max)
		{
			if (access_count == ci->GetAccessCount())
				source.Reply(access_max, CHAN_ACCESS_LIMIT, access_max);
			else
				source.Reply(access_max, CHAN_ACCESS_LIMIT_DEEP, access_max);
			return;
		}

		Privilege *p = NULL;
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
					for (const auto &[priv, flag] : defaultFlags)
					{
						bool has = current_flags.count(flag);
						// If we are adding a flag they already have or removing one they don't have, don't bother
						if (add == has)
							continue;

						if (!u_access.HasPriv(priv) && !u_access.founder)
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
								continue;
						}

						if (add)
							current_flags.insert(flag);
						else
							current_flags.erase(flag);
					}
					break;
				default:
					p = PrivilegeManager::FindPrivilege(flags.substr(i));
					if (p != NULL && defaultFlags[p->name])
					{
						f = defaultFlags[p->name];
						i = flags.length();
					}

					for (const auto &[priv, flag] : defaultFlags)
					{
						if (f != flag)
							continue;
						else if (!u_access.HasPriv(priv) && !u_access.founder)
						{
							if (source.HasPriv("chanserv/access/modify"))
								override = true;
							else
							{
								source.Reply(_("You cannot set the \002%c\002 flag."), f);
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
				ci->EraseAccess(current_idx - 1);
				FOREACH_MOD(OnAccessDel, (ci, source, current, false));
				delete current;
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;
				source.Reply(_("\002%s\002 removed from the %s access list."), mask.c_str(), ci->name.c_str());
			}
			else
			{
				source.Reply(_("\002%s\002 not found on %s access list."), mask.c_str(), ci->name.c_str());
			}
			return;
		}

		ServiceReference<AccessProvider> provider("AccessProvider", "access/flags");
		if (!provider)
			return;
		auto *access = anope_dynamic_static_cast<FlagsChanAccess *>(provider->Create());
		access->SetMask(mask, ci);
		access->creator = source.GetNick();
		access->description = current && description.empty() ? current->description : description;
		access->last_seen = current ? current->last_seen : 0;
		access->created = Anope::CurTime;
		access->flags = current_flags;

		if (current != NULL)
			delete current;

		ci->AddAccess(access);

		FOREACH_MOD(OnAccessAdd, (ci, source, access, false));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to modify " << mask << "'s flags to " << access->AccessSerialize();
		if (p != NULL)
		{
			if (add)
				source.Reply(_("Privilege \002%s\002 added to \002%s\002 on \002%s\002, new flags are +\002%s\002"), p->name.c_str(), access->Mask().c_str(), ci->name.c_str(), access->AccessSerialize().c_str());
			else
				source.Reply(_("Privilege \002%s\002 removed from \002%s\002 on \002%s\002, new flags are +\002%s\002"), p->name.c_str(), access->Mask().c_str(), ci->name.c_str(), access->AccessSerialize().c_str());
		}
		else
			source.Reply(_("Flags for \002%s\002 on %s set to +\002%s\002"), access->Mask().c_str(), ci->name.c_str(), access->AccessSerialize().c_str());
	}

	static void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &arg = params.size() > 2 ? params[2] : "";
		const auto show_all = params.size() > 3 && params[3].equals_ci("ALL");

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s access list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Flags")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Description"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Description"].empty()
				? _("{number}: \002{mask}\002 = {flags} -- added by {creator} at {created}")
				: _("{number}: \002{mask}\002 = {flags} -- added by {creator} at {created} ({description})");
		});

		unsigned count = 0;
		unsigned foreign = 0;
		for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
		{
			const ChanAccess *access = ci->GetAccess(i);
			const Anope::string &flags = FlagsChanAccess::DetermineFlags(access);

			if (!arg.empty())
			{
				if (arg[0] == '+')
				{
					bool pass = true;
					for (size_t j = 1; j < arg.length(); ++j)
						if (flags.find(arg[j]) == Anope::string::npos)
							pass = false;
					if (!pass)
						continue;
				}
				else if (!Anope::Match(access->Mask(), arg))
					continue;
			}

			if (!show_all && access->provider->name != "access/flags")
			{
				foreign++;
				continue;
			}

			ListFormatter::ListEntry entry;
			++count;
			entry["Number"] = Anope::ToString(i + 1);
			entry["Mask"] = access->Mask();
			entry["Flags"] = flags;
			entry["Creator"] = access->creator;
			entry["Created"] = Anope::strftime(access->created, source.nc, true);
			entry["Description"] = access->description;
			list.AddEntry(entry);
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			source.Reply(_("Flags list for %s"), ci->name.c_str());
			list.SendTo(source);
			if (count == ci->GetAccessCount())
				source.Reply(_("End of access list."));
			else
				source.Reply(_("End of access list - %d/%d entries shown."), count, ci->GetAccessCount());
		}

		if (foreign)
		{
			const auto full_command = Anope::Format("%s %s LIST %s", source.command.c_str(),
				ci->name.c_str(), arg.empty() ? "*" : arg.c_str()).nobreak();

			source.Reply(foreign, CHAN_ACCESS_FOREIGN, foreign, full_command.c_str());
		}
	}

	void DoMigrate(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		auto override = false;

		unsigned migrated = 0, notmigratable = 0, notmigrated = 0;
		Anope::string migratedmask, notmigratablemask, notmigratedmask;
		const auto &entry = params.size() > 2 ? params[2] : "*";
		for (auto idx = ci->GetAccessCount(); idx > 0; --idx)
		{
			auto *access = ci->GetAccess(idx - 1);
			if (access->provider->name == "access/flags")
				continue; // Already using flags.

			if (!Anope::Match(access->Mask(), entry))
				continue; // Not this entry.

			std::set<char> newflags;
			for (auto &[priv, flag] : defaultFlags)
			{
				if (!access->HasPriv(priv))
					continue; // Source doesn't have this flag.

				// Check that the source has access to set this entry.
				const auto source_access = source.AccessFor(ci);
				if (!override && !source_access.HasPriv(priv) && !source_access.founder)
				{
					if (!source.HasPriv("chanserv/access/modify"))
					{
						notmigrated++;
						notmigratedmask = access->Mask();
						Log(LOG_DEBUG) << source.GetNick() << " does not have the access to migrate " << access->Mask() << " to flags";
						continue; // No privs
					}

					override = true;
				}

				auto req = migrationRequires.find(priv);
				if (req != migrationRequires.end() && !access->HasPriv(req->second))
				{
					Log(LOG_DEBUG) << access->Mask() << " has " << priv << " but not " << req->second << " so it will be lost on migration to flags";
					continue; // Required flag missing.
				}

				newflags.insert(flag);
			}

			if (newflags.empty())
			{
				notmigratable++;
				notmigratablemask = access->Mask();
				Log(LOG_DEBUG) << access->Mask() << " has " << access->AccessSerialize() << " that can not be migrated to flags";
				continue; // No privs that are migratable
			}

			migrated++;
			migratedmask = access->Mask();

			auto *newaccess = anope_dynamic_static_cast<FlagsChanAccess *>(FlagsAccessProvider::ap->Create());
			newaccess->SetMask(access->Mask(), ci);
			newaccess->created = access->created;
			newaccess->creator = access->creator;
			newaccess->description = access->description;
			newaccess->flags = newflags;
			newaccess->last_seen = access->last_seen;

			ci->EraseAccess(idx - 1);
			FOREACH_MOD(OnAccessDel, (ci, source, access, true));
			delete access;

			ci->AddAccess(newaccess);
			FOREACH_MOD(OnAccessAdd, (ci, source, newaccess, true));
		}

		if (migrated == 1)
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to migrate " << migratedmask;
			source.Reply(FLAGS_MIGRATED_1, migratedmask.c_str(), source.command.nobreak().c_str());
		}
		else if (migrated > 1)
		{
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to migrate " << migrated << " access entries";
			source.Reply(migrated, FLAGS_MIGRATED_N, migrated, source.command.nobreak().c_str());
		}

		if (notmigratable == 1)
			source.Reply(FLAGS_NOT_MIGRATABLE_1, notmigratablemask.c_str(), source.command.nobreak().c_str());
		else if (notmigratable > 1)
			source.Reply(notmigratable, FLAGS_NOT_MIGRATABLE_N, notmigratable, source.command.nobreak().c_str());

		if (notmigrated == 1)
			source.Reply(FLAGS_NOT_MIGRATED_1, notmigratedmask.c_str(), source.command.nobreak().c_str());
		else if (notmigrated > 1)
			source.Reply(notmigrated, FLAGS_NOT_MIGRATED_N, notmigrated, source.command.nobreak().c_str());
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		if (!source.IsFounder(ci) && !source.HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else
		{
			ci->ClearAccess();

			FOREACH_MOD(OnAccessClear, (ci, source));

			source.Reply(_("Channel %s access list has been cleared."), ci->name.c_str());

			bool override = !source.IsFounder(ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";
		}
	}

public:
	CommandCSFlags(Module *creator) : Command(creator, "chanserv/flags", 1, 5)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 [MODIFY] \037mask\037 \037changes\037 [\037description\037]"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | +\037flags\037] [ALL]"));
		this->SetSyntax(_("\037channel\037 MIGRATE [\037mask\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params.size() > 1 ? params[1] : "";

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
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
			source.Reply(ACCESS_DENIED);
		else if (Anope::ReadOnly && !is_list)
			source.Reply(READ_ONLY_MODE);
		else if (is_list)
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("MIGRATE"))
			this->DoMigrate(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
		{
			Anope::string mask, flags, description;
			if (cmd.equals_ci("MODIFY"))
			{
				mask = params.size() > 2 ? params[2] : "";
				flags = params.size() > 3 ? params[3] : "";
				description = params.size() > 4 ? params[4] : "";
			}
			else
			{
				mask = cmd;
				flags = params.size() > 2 ? params[2] : "";
				description = params.size() > 3 ? params[3] : "";
			}

			this->DoModify(source, ci, mask, flags, description);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"%s is another way to modify the channel access list, similar to "
				"the XOP and ACCESS methods."
				"\n\n"
				"The \002MODIFY\002 command allows you to modify the access list. If the mask is "
				"not already on the access list it is added, then the changes are applied. "
				"If the mask has no more flags, then the mask is removed from the access list. "
				"Additionally, you may use +* or -* to add or remove all flags, respectively. You are "
				"only able to modify the access list if you have the proper permission on the channel, "
				"and even then you can only give other people access to the equivalent of what your access is."
				"\n\n"
				"The \002LIST\002 command allows you to list existing entries on the channel access list. "
				"If a mask is given, the mask is wildcard matched against all existing entries on the "
				"access list, and only those entries are returned. If a set of flags is given, only those "
				"on the access list with the specified flags are returned. The \002ALL\002 option allows "
				"listing entries from other access systems as well as flags."
				"\n\n"
				"The \002CLEAR\002 command clears the channel access list. This requires channel founder access."
				"\n\n"
				"The available flags are:"
			),
			source.command.nobreak().c_str());

		typedef std::multimap<char, Anope::string, ci::less> reverse_map;
		reverse_map reverse;
		for (auto &[priv, flag] : defaultFlags)
			reverse.emplace(flag, priv);

		for (auto &[flag, priv] : reverse)
		{
			Privilege *p = PrivilegeManager::FindPrivilege(priv);
			if (p == NULL)
				continue;
			source.Reply("  %c - %s", flag, source.Translate(p->desc.c_str()));
		}

		return true;
	}
};

class CSFlags final
	: public Module
{
	FlagsAccessProvider accessprovider;
	CommandCSFlags commandcsflags;

public:
	CSFlags(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		accessprovider(this), commandcsflags(this)
	{
		this->SetPermanent(true);

	}

	void OnReload(Configuration::Conf &conf) override
	{
		defaultFlags.clear();

		for (int i = 0; i < conf.CountBlock("privilege"); ++i)
		{
			const auto &priv = conf.GetBlock("privilege", i);

			const Anope::string &pname = priv.Get<const Anope::string>("name");

			Privilege *p = PrivilegeManager::FindPrivilege(pname);
			if (p == NULL)
				continue;

			const Anope::string &value = priv.Get<const Anope::string>("flag");
			if (value.empty())
				continue;

			const auto &migration_requires = priv.Get<const Anope::string>("flag_migration_requires");
			if (!migration_requires.empty())
				migrationRequires[p->name] = migration_requires;

			defaultFlags[p->name] = value[0];
		}
	}
};

MODULE_INIT(CSFlags)
