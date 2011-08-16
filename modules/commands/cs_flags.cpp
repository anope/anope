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

static struct FlagLevels
{
	ChannelAccess priv;
	char default_char;
	Anope::string config_name;
	Anope::string desc;
} flagLevels[] = {
	{ CA_ACCESS_CHANGE, 'f', "flag_change", _("Allowed to modify the access list") },
	{ CA_ACCESS_LIST, 'l', "flag_list", _("Allowed to view the access list") },
	{ CA_AKICK, 'K', "flag_akick", _("Allowed to use AKICK command") },
	{ CA_ASSIGN, 's', "flag_assign", _("Allowed to assign/unassign a bot") },
	{ CA_AUTOHALFOP, 'H', "flag_autohalfop", _("Automatic mode +h") },
	{ CA_AUTOOP, 'O', "flag_autoop", _("Automatic channel operator status") },
	{ CA_AUTOOWNER,	'Q', "flag_autoowner", _("Automatic mode +q") },
	{ CA_AUTOPROTECT, 'A', "flag_autoprotect", _("Automatic mode +a") },
	{ CA_AUTOVOICE, 'V', "flag_autovoice", _("Automatic mode +v") },
	{ CA_BADWORDS, 'k', "flag_badwords", _("Allowed to modify channel badwords list") },
	{ CA_BAN, 'b', "flag_ban", _("Allowed to use ban users") },
	{ CA_FANTASIA, 'c', "flag_fantasia", _("Allowed to use fantasy commands") },
	{ CA_FOUNDER, 'F', "flag_founder", _("Allowed to issue commands restricted to channel founders") },
	{ CA_GETKEY, 'G', "flag_getkey", _("Allowed to use GETKEY command") },
	{ CA_GREET, 'g', "flag_greet", _("Greet message displayed") },
	{ CA_HALFOP, 'h', "flag_halfop", _("Allowed to (de)halfop users") },
	{ CA_HALFOPME, 'h', "flag_halfopme", _("Allowed to (de)halfop him/herself") },
	{ CA_INFO, 'I', "flag_info", _("Allowed to use INFO command with ALL option") },
	{ CA_INVITE, 'i', "flag_invite", _("Allowed to use the INVITE command") },
	{ CA_KICK, 'k', "flag_kick", _("Allowed to use the KICK command") },
	{ CA_MEMO, 'm', "flag_memo", _("Allowed to read channel memos") },
	{ CA_MODE, 's', "flag_mode", _("Allowed to change channel modes") },
	{ CA_NOKICK, 'N', "flag_nokick", _("Prevents users being kicked by Services") },
	{ CA_OPDEOP, 'o', "flag_opdeop", _("Allowed to (de)op users") },
	{ CA_OPDEOPME, 'o', "flag_opdeopme", _("Allowed to (de)op him/herself") },
	{ CA_OWNER, 'q', "flag_owner", _("Allowed to use (de)owner users") },
	{ CA_OWNERME, 'q', "flag_ownerme", _("Allowed to (de)owner him/herself") },
	{ CA_PROTECT, 'a', "flag_protect", _("Allowed to (de)protect users") },
	{ CA_PROTECTME, 'a', "flag_protectme", _("Allowed to (de)protect him/herself"), },
	{ CA_SAY, 'B', "flag_say", _("Allowed to use SAY and ACT commands") },
	{ CA_SET, 's', "flag_set", _("Allowed to set channel settings") },
	{ CA_SIGNKICK, 'K', "flag_signkick", _("Prevents kicks from being signed") },
	{ CA_TOPIC, 't', "flag_topic", _("Allowed to change channel topics") },
	{ CA_UNBAN, 'u', "flag_unban", _("Allowed to unban users") },
	{ CA_VOICE, 'v', "flag_voice", _("Allowed to (de)voice users") },
	{ CA_VOICEME, 'v', "flag_voiceme", _("Allowed to (de)voice him/herself") },
	{ CA_SIZE, -1, "", "" }
};

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

	bool HasPriv(ChannelAccess priv)
	{
		for (int i = 0; flagLevels[i].priv != CA_SIZE; ++i)
			if (flagLevels[i].priv == priv)
				return this->flags.count(flagLevels[i].default_char);

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

		for (int i = 0; flagLevels[i].priv != CA_SIZE; ++i)
		{
			FlagLevels &l = flagLevels[i];

			if (access->HasPriv(l.priv))
				buffer.insert(l.default_char);
		}

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
					for (int j = 0; flagLevels[j].priv != CA_SIZE; ++j)
					{
						FlagLevels &l = flagLevels[j];
						if (!u_access.HasPriv(l.priv))
						{
							if (u->HasPriv("chanserv/access/modify"))
								override = true;
							else
								continue;
						}
						if (add == 1)
							current_flags.insert(l.default_char);
						else if (add == 0)
							current_flags.erase(l.default_char);
					}
					break;
				default:
					if (add == -1)
						break;
					for (int j = 0; flagLevels[j].priv != CA_SIZE; ++j)
					{
						FlagLevels &l = flagLevels[j];
						if (f != l.default_char)
							continue;
						else if (!u_access.HasPriv(l.priv))
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
		else if (is_list && ci->AccessFor(u).HasPriv(CA_ACCESS_LIST))
			has_access = true;
		else if (ci->AccessFor(u).HasPriv(CA_ACCESS_CHANGE))
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

		std::multimap<char, FlagLevels *, std::less<ci::string> > levels;
		for (int i = 0; flagLevels[i].priv != CA_SIZE; ++i)
			levels.insert(std::make_pair(flagLevels[i].default_char, &flagLevels[i]));
		for (std::multimap<char, FlagLevels *, std::less<ci::string> >::iterator it = levels.begin(), it_end = levels.end(); it != it_end; ++it)
		{
			FlagLevels *l = it->second;
			source.Reply("  %c - %s", l->default_char, translate(source.u->Account(), l->desc.c_str()));
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

		ModuleManager::RegisterService(&accessprovider);
		ModuleManager::RegisterService(&commandcsflags);

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, 1);

		this->OnReload();
	}

	void OnReload()
	{
		ConfigReader config;

		for (int i = 0; flagLevels[i].priv != CA_SIZE; ++i)
		{
			FlagLevels &l = flagLevels[i];

			const Anope::string &value = config.ReadValue("chanserv", l.config_name, "", 0);
			if (value.empty())
				continue;
			l.default_char = value[0];
		}
	}
};

MODULE_INIT(CSFlags)
