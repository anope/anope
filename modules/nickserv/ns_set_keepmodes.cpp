/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSSetKeepModes
	: public Command
{
public:
	CommandNSSetKeepModes(Module *creator, const Anope::string &sname = "nickserv/set/keepmodes", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Enable or disable keep modes"));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable keepmodes for " << nc->display;
			nc->Extend<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002on\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable keepmodes for " << nc->display;
			nc->Shrink<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002off\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables keepmodes for your nick. If keep "
			"modes is enabled, services will remember your usermodes "
			"and attempt to re-set them the next time you authenticate."
		));
		return true;
	}
};

class CommandNSSASetKeepModes final
	: public CommandNSSetKeepModes
{
public:
	CommandNSSASetKeepModes(Module *creator)
		: CommandNSSetKeepModes(creator, "nickserv/saset/keepmodes", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables keepmodes for the given nick. If keep "
			"modes is enabled, services will remember users' usermodes "
			"and attempt to re-set them the next time they authenticate."
		));
		return true;
	}
};


class NSSetKeepModes final
	: public Module
{
private:
	CommandNSSetKeepModes commandnssetkeepmodes;
	CommandNSSASetKeepModes commandnssasetkeepmodes;

	struct KeepModes final
		: SerializableExtensibleItem<bool>
	{
		KeepModes(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			SerializableExtensibleItem<bool>::ExtensibleSerialize(e, s, data);

			if (s->GetSerializableType()->GetName() != NICKCORE_TYPE)
				return;

			const NickCore *nc = anope_dynamic_static_cast<const NickCore *>(s);
			Anope::string modes;
			for (const auto &[last_mode, last_data] : nc->last_modes)
			{
				if (!modes.empty())
					modes += " ";

				modes += '+';
				modes += last_mode;
				if (!last_data.value.empty())
				{
					modes += "," + Anope::ToString(last_data.set_at);
					modes += "," + last_data.set_by;
					modes += "," + last_data.value;
				}
			}
			data.Store("last_modes", modes);
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != NICKCORE_TYPE)
				return;

			NickCore *nc = anope_dynamic_static_cast<NickCore *>(s);
			Anope::string modes;
			data["last_modes"] >> modes;
			nc->last_modes.clear();
			for (spacesepstream sep(modes); sep.GetToken(modes);)
			{
				if (modes[0] == '+')
				{
					commasepstream mode(modes, true);
					mode.GetToken(modes);
					modes.erase(0, 1);

					ModeData info;
					Anope::string set_at;
					mode.GetToken(set_at);
					info.set_at = Anope::Convert(set_at, 0);
					mode.GetToken(info.set_by);
					info.value = mode.GetRemaining();

					nc->last_modes.emplace(modes, info);
					continue;
				}
				else
				{
					// Begin 2.0 compatibility.
					size_t c = modes.find(',');
					if (c == Anope::string::npos)
						nc->last_modes.emplace(modes, ModeData());
					else
						nc->last_modes.emplace(modes.substr(0, c), ModeData(modes.substr(c + 1)));
					// End 2.0 compatibility.
				}
			}
		}
	} keep_modes;

public:
	NSSetKeepModes(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetkeepmodes(this)
		, commandnssasetkeepmodes(this)
		, keep_modes(this, "NS_KEEP_MODES")
	{
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (keep_modes.HasExt(na->nc))
			info.AddOption(_("Keep modes"));
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->IsIdentified() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->IsIdentified() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}

	void OnUserLogin(User *u) override
	{
		if (keep_modes.HasExt(u->Account()))
		{
			const auto norestore = Config->GetModule(this).Get<const Anope::string>("norestore");
			User::ModeList modes = u->Account()->last_modes;
			for (const auto &[last_mode, last_data] : modes)
			{
				auto *um = ModeManager::FindUserModeByName(last_mode);
				if (um && um->CanSet(nullptr) && norestore.find(um->mchar) == Anope::string::npos)
					u->SetMode(nullptr, last_mode, last_data);
			}
		}
	}
};

MODULE_INIT(NSSetKeepModes)
