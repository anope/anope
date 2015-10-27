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
#include "modules/cs_mode.h"
#include "modules/cs_info.h"

static EventHandlers<Event::MLockEvents> *events;

class ModeLockImpl : public ModeLock
{
 public:
	ModeLockImpl(Serialize::TypeBase *type) : ModeLock(type) { }
	ModeLockImpl(Serialize::TypeBase *type, Serialize::ID id) : ModeLock(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *ci) override;

	bool GetSet() override;
	void SetSet(const bool &) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &name) override;

	Anope::string GetParam() override;
	void SetParam(const Anope::string &p) override;

	Anope::string GetSetter() override;
	void SetSetter(const Anope::string &s) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &) override;
};

class ModeLockType : public Serialize::Type<ModeLockImpl>
{
 public:
	Serialize::ObjectField<ModeLockImpl, ChanServ::Channel *> ci;
	Serialize::Field<ModeLockImpl, bool> set;
	Serialize::Field<ModeLockImpl, Anope::string> name, param, setter;
	Serialize::Field<ModeLockImpl, time_t> created;

	ModeLockType(Module *me) : Serialize::Type<ModeLockImpl>(me, "ModeLock")
		, ci(this, "ci", true)
		, set(this, "set")
		, name(this, "name")
		, param(this, "param")
		, setter(this, "setter")
		, created(this, "created")
	{
	}
};

ChanServ::Channel *ModeLockImpl::GetChannel()
{
	return Get(&ModeLockType::ci);
}

void ModeLockImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&ModeLockType::ci, ci);
}

bool ModeLockImpl::GetSet()
{
	return Get(&ModeLockType::set);
}

void ModeLockImpl::SetSet(const bool &b)
{
	Set(&ModeLockType::set, b);
}

Anope::string ModeLockImpl::GetName()
{
	return Get(&ModeLockType::name);
}

void ModeLockImpl::SetName(const Anope::string &name)
{
	Set(&ModeLockType::name, name);
}

Anope::string ModeLockImpl::GetParam()
{
	return Get(&ModeLockType::param);
}

void ModeLockImpl::SetParam(const Anope::string &p)
{
	Set(&ModeLockType::name, p);
}

Anope::string ModeLockImpl::GetSetter()
{
	return Get(&ModeLockType::setter);
}

void ModeLockImpl::SetSetter(const Anope::string &s)
{
	Set(&ModeLockType::name, s);
}

time_t ModeLockImpl::GetCreated()
{
	return Get(&ModeLockType::created);
}

void ModeLockImpl::SetCreated(const time_t &c)
{
	Set(&ModeLockType::created, c);
}

class ModeLocksImpl : public ModeLocks
{
 public:
	using ModeLocks::ModeLocks;

	bool HasMLock(ChanServ::Channel *ci, ChannelMode *mode, const Anope::string &param, bool status) const override
	{
		if (!mode)
			return false;

		for (ModeLock *ml : ci->GetRefs<ModeLock *>(modelock))
			if (ml->GetName() == mode->name && ml->GetSet() == status && ml->GetParam() == param)
				return true;

		return false;
	}

	bool SetMLock(ChanServ::Channel *ci, ChannelMode *mode, bool status, const Anope::string &param, Anope::string setter, time_t created = Anope::CurTime) override
	{
		if (!mode)
			return false;

		RemoveMLock(ci, mode, status, param);

		if (setter.empty())
			setter = ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "Unknown";

		ModeLock *ml = modelock.Create();
		ml->SetChannel(ci);
		ml->SetSet(status);
		ml->SetName(name);
		ml->SetParam(param);
		ml->SetSetter(setter);
		ml->SetCreated(created);

		EventReturn MOD_RESULT;
		MOD_RESULT = (*events)(&Event::MLockEvents::OnMLock, ci, ml);
		if (MOD_RESULT == EVENT_STOP)
		{
			delete ml;
			return false;
		}

		return true;
	}

	bool RemoveMLock(ChanServ::Channel *ci, ChannelMode *mode, bool status, const Anope::string &param = "") override
	{
		if (!mode)
			return false;

		for (ModeLock *m : ci->GetRefs<ModeLockImpl *>(modelock))
			if (m->GetName() == mode->name)
			{
				// For list or status modes, we must check the parameter
				if (mode->type == MODE_LIST || mode->type == MODE_STATUS)
					if (m->GetParam() != param)
						continue;

				EventReturn MOD_RESULT;
				MOD_RESULT = (*events)(&Event::MLockEvents::OnUnMLock, ci, m);
				if (MOD_RESULT == EVENT_STOP)
					break;

				delete m;
				return true;
			}

		return false;
	}

	void ClearMLock(ChanServ::Channel *ci) override
	{
		for (ModeLock *m : ci->GetRefs<ModeLock *>(modelock))
			delete m;
	}

	ModeList GetMLock(ChanServ::Channel *ci) const override
	{
		return ci->GetRefs<ModeLock *>(modelock);
	}

	std::list<ModeLock *> GetModeLockList(ChanServ::Channel *ci, const Anope::string &name) override
	{
		std::list<ModeLock *> mlist;
		for (ModeLock *m : ci->GetRefs<ModeLock *>(modelock))
			if (m->GetName() == name)
				mlist.push_back(m);
		return mlist;
	}

	ModeLock *GetMLock(ChanServ::Channel *ci, const Anope::string &mname, const Anope::string &param = "") override
	{
		for (ModeLock *m : ci->GetRefs<ModeLock *>(modelock))
			if (m->GetName() == mname && m->GetParam() == param)
				return m;

		return NULL;
	}

	Anope::string GetMLockAsString(ChanServ::Channel *ci, bool complete) const override
	{
		Anope::string pos = "+", neg = "-", params;

		for (ModeLock *ml : ci->GetRefs<ModeLock *>(modelock))
		{
			ChannelMode *cm = ModeManager::FindChannelModeByName(ml->GetName());

			if (!cm || cm->type == MODE_LIST || cm->type == MODE_STATUS)
				continue;

			if (ml->GetSet())
				pos += cm->mchar;
			else
				neg += cm->mchar;

			if (complete && ml->GetSet() && !ml->GetParam().empty() && cm->type == MODE_PARAM)
				params += " " + ml->GetParam();
		}

		if (pos.length() == 1)
			pos.clear();
		if (neg.length() == 1)
			neg.clear();

		return pos + neg + params;
	}
};

class CommandCSMode : public Command
{
	bool CanSet(CommandSource &source, ChanServ::Channel *ci, ChannelMode *cm, bool self)
	{
		if (!ci || !cm || cm->type != MODE_STATUS)
			return false;

		return source.AccessFor(ci).HasPriv(cm->name + (self ? "ME" : ""));
	}

	void DoLock(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();
		const Anope::string &subcommand = params[2];
		const Anope::string &param = params.size() > 3 ? params[3] : "";

		bool override = !source.AccessFor(ci).HasPriv("MODE");

		if (Anope::ReadOnly && !subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if ((subcommand.equals_ci("ADD") || subcommand.equals_ci("SET")) && !param.empty())
		{
			/* If setting, remove the existing locks */
			if (subcommand.equals_ci("SET"))
				for (ModeLock *ml : mlocks->GetMLock(ci))
				{
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->GetName());
					if (cm && cm->CanSet(source.GetUser()))
						mlocks->RemoveMLock(ci, cm, ml->GetSet(), ml->GetParam());
				}

			spacesepstream sep(param);
			Anope::string modes;

			sep.GetToken(modes);

			Anope::string pos = "+", neg = "-", pos_params, neg_params;

			int adding = 1;
			bool needreply = true;
			for (size_t i = 0; i < modes.length(); ++i)
			{
				switch (modes[i])
				{
					case '+':
						adding = 1;
						break;
					case '-':
						adding = 0;
						break;
					default:
						needreply = false;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
						if (!cm)
						{
							source.Reply(_("Unknown mode character \002{0}\002 ignored."), modes[i]);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode \002{0}\002."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (((cm->type == MODE_STATUS || cm->type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
							source.Reply(_("Missing parameter for mode \002{0}\002."), cm->mchar);
						else if (cm->type == MODE_LIST && ci->c && IRCD->GetMaxListFor(ci->c) && ci->c->HasMode(cm->name) >= IRCD->GetMaxListFor(ci->c))
							source.Reply(_("List for mode \002{0}\002 is full."), cm->mchar);
						else if (ci->GetRefs<ModeLock *>(modelock).size() >= Config->GetModule(this->owner)->Get<unsigned>("max", "32"))
							source.Reply(_("The mode lock list of \002{0}\002 is full."), ci->GetName());
						else
						{
							mlocks->SetMLock(ci, cm, adding, mode_param, source.GetNick());

							if (adding)
							{
								pos += cm->mchar;
								if (!mode_param.empty())
									pos_params += " " + mode_param;
							}
							else
							{
								neg += cm->mchar;
								if (!mode_param.empty())
									neg_params += " " + mode_param;
							}
						}
				}
			}

			if (pos == "+")
				pos.clear();
			if (neg == "-")
				neg.clear();
			Anope::string reply = pos + neg + pos_params + neg_params;

			if (!reply.empty())
			{
				source.Reply(_("\002{0}\002 locked on \002{1}\002."), reply, ci->GetName());
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to lock " << reply;
			}
			else if (needreply)
				source.Reply(_("Nothing to do."));

			if (ci->c)
				ci->c->CheckModes();
		}
		else if (subcommand.equals_ci("DEL") && !param.empty())
		{
			spacesepstream sep(param);
			Anope::string modes;

			sep.GetToken(modes);

			int adding = 1;
			bool needreply = true;
			for (size_t i = 0; i < modes.length(); ++i)
			{
				switch (modes[i])
				{
					case '+':
						adding = 1;
						break;
					case '-':
						adding = 0;
						break;
					default:
						needreply = false;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
						if (!cm)
						{
							source.Reply(_("Unknown mode character \002{0}\002 ignored."), modes[i]);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode \002{0}\002."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (cm->type != MODE_REGULAR && !sep.GetToken(mode_param))
							source.Reply(_("Missing parameter for mode \002{0}\002."), cm->mchar);
						else
						{
							if (mlocks->RemoveMLock(ci, cm, adding, mode_param))
							{
								if (!mode_param.empty())
									mode_param = " " + mode_param;
								source.Reply(_("\002{0}{1}{2}\002 has been unlocked from \002{3}\002."), adding == 1 ? '+' : '-', cm->mchar, mode_param, ci->GetName());
								Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to unlock " << (adding ? '+' : '-') << cm->mchar << mode_param;
							}
							else
								source.Reply(_("\002{0}{1}\002 is not locked on \002{2}\002."), adding == 1 ? '+' : '-', cm->mchar, ci->GetName());
						}
				}
			}

			if (needreply)
				source.Reply(_("Nothing to do."));
		}
		else if (subcommand.equals_ci("LIST"))
		{
			ModeLocks::ModeList locks = mlocks->GetMLock(ci);
			if (locks.empty())
			{
				source.Reply(_("Channel \002{0}\002 has no mode locks."), ci->GetName());
				return;
			}

			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Mode")).AddColumn(_("Param")).AddColumn(_("Creator")).AddColumn(_("Created"));

			for (ModeLock *ml : locks)
			{
				ChannelMode *cm = ModeManager::FindChannelModeByName(ml->GetName());
				if (!cm)
					continue;

				ListFormatter::ListEntry entry;
				entry["Mode"] = Anope::printf("%c%c", ml->GetSet() ? '+' : '-', cm->mchar);
				entry["Param"] = ml->GetParam();
				entry["Creator"] = ml->GetSetter();
				entry["Created"] = Anope::strftime(ml->GetCreated(), NULL, true);
				list.AddEntry(entry);
			}

			source.Reply(_("Mode locks for \002{0}\002:"), ci->GetName());

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else
			this->OnSyntaxError(source, subcommand);
	}

	void DoSet(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();

		bool has_access = source.AccessFor(ci).HasPriv("MODE") || source.HasPriv("chanserv/administration");
		bool can_override = source.HasPriv("chanserv/administration");

		spacesepstream sep(params.size() > 3 ? params[3] : "");
		Anope::string modes = params[2], param;

		bool override = !source.AccessFor(ci).HasPriv("MODE") && source.HasPriv("chanserv/administration");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to set " << params[2] << (params.size() > 3 ? " " + params[3] : "");

		int adding = -1;
		for (size_t i = 0; i < modes.length(); ++i)
		{
			switch (modes[i])
			{
				case '+':
					adding = 1;
					break;
				case '-':
					adding = 0;
					break;
				case '*':
					if (adding == -1 || !has_access)
						break;
					for (unsigned j = 0; j < ModeManager::GetChannelModes().size() && ci->c; ++j)
					{
						ChannelMode *cm = ModeManager::GetChannelModes()[j];

						if (!u || cm->CanSet(u) || can_override)
						{
							if (cm->type == MODE_REGULAR || (!adding && cm->type == MODE_PARAM))
							{
								if (adding)
									ci->c->SetMode(NULL, cm);
								else
									ci->c->RemoveMode(NULL, cm);
							}
						}
					}
					break;
				default:
					if (adding == -1)
						break;
					ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
					if (!cm || (u && !cm->CanSet(u) && !can_override))
						continue;
					switch (cm->type)
					{
						case MODE_REGULAR:
							if (!has_access)
								break;
							if (adding)
								ci->c->SetMode(NULL, cm);
							else
								ci->c->RemoveMode(NULL, cm);
							break;
						case MODE_PARAM:
							if (!has_access)
								break;
							if (adding && !sep.GetToken(param))
								break;
							if (adding)
								ci->c->SetMode(NULL, cm, param);
							else
								ci->c->RemoveMode(NULL, cm);
							break;
						case MODE_STATUS:
						{
							if (!sep.GetToken(param))
								param = source.GetNick();

							ChanServ::AccessGroup u_access = source.AccessFor(ci);

							if (param.find_first_of("*?") != Anope::string::npos)
							{
								if (!this->CanSet(source, ci, cm, false) && !can_override)
								{
									source.Reply(_("You do not have access to set mode \002{0}\002."), cm->mchar);
									break;
								}

								for (Channel::ChanUserList::const_iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end;)
								{
									ChanUserContainer *uc = it->second;
									++it;

									ChanServ::AccessGroup targ_access = ci->AccessFor(uc->user);

									if (uc->user->IsProtected() || (ci->HasFieldS("PEACE") && targ_access >= u_access && !can_override))
									{
										source.Reply(_("You do not have the access to change the modes of \002{0}\002."), uc->user->nick.c_str());
										continue;
									}

									if (Anope::Match(uc->user->GetMask(), param))
									{
										if (adding)
											ci->c->SetMode(NULL, cm, uc->user->GetUID());
										else
											ci->c->RemoveMode(NULL, cm, uc->user->GetUID());
									}
								}
							}
							else
							{
								User *target = User::Find(param, true);
								if (target == NULL)
								{
									source.Reply(_("User \002{0}\002 isn't currently online."), param);
									break;
								}

								if (!this->CanSet(source, ci, cm, source.GetUser() == target) && !can_override)
								{
									source.Reply(_("You do not have access to set mode \002{0}\002."), cm->mchar);
									break;
								}

								if (source.GetUser() != target)
								{
									ChanServ::AccessGroup targ_access = ci->AccessFor(target);
									if (ci->HasFieldS("PEACE") && targ_access >= u_access && !can_override)
									{
										source.Reply(_("You do not have the access to change the modes of \002{0}\002"), target->nick);
										break;
									}
									else if (target->IsProtected())
									{
										source.Reply(_("Access denied. \002{0}\002 is protected and can not have their modes changed."), target->nick);
										break;
									}
								}

								if (adding)
									ci->c->SetMode(NULL, cm, target->GetUID());
								else
									ci->c->RemoveMode(NULL, cm, target->GetUID());
							}
							break;
						}
						case MODE_LIST:
							if (!has_access)
								break;
							if (!sep.GetToken(param))
								break;
							if (adding)
							{
								if (IRCD->GetMaxListFor(ci->c) && ci->c->HasMode(cm->name) < IRCD->GetMaxListFor(ci->c))
									ci->c->SetMode(NULL, cm, param);
							}
							else
							{
								std::vector<Anope::string> v = ci->c->GetModeList(cm->name);
								for (unsigned j = 0; j < v.size(); ++j)
									if (Anope::Match(v[j], param))
										ci->c->RemoveMode(NULL, cm, v[j]);
							}
					}
			}
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			std::vector<Anope::string> new_params;
			new_params.push_back(params[0]);
			new_params.push_back("SET");
			new_params.push_back("-*");
			this->DoSet(source, ci, new_params);
			return;
		}

		ChannelMode *cm;
		if (param.length() == 1)
			cm = ModeManager::FindChannelModeByChar(param[0]);
		else
		{
			cm = ModeManager::FindChannelModeByName(param.upper());
			if (!cm)
				cm = ModeManager::FindChannelModeByName(param.substr(0, param.length() - 1).upper());
		}

		if (!cm)
		{
			source.Reply(_("There is no such mode \002{0}\002."), param);
			return;
		}

		if (cm->type != MODE_STATUS && cm->type != MODE_LIST)
		{
			source.Reply(_("Mode \002{0}\002 is not a status or list mode."), param);
			return;
		}

		std::vector<Anope::string> new_params;
		new_params.push_back(params[0]);
		new_params.push_back("SET");
		new_params.push_back("-" + stringify(cm->mchar));
		new_params.push_back("*");
		this->DoSet(source, ci, new_params);
	}

 public:
	CommandCSMode(Module *creator) : Command(creator, "chanserv/mode", 2, 4)
	{
		this->SetDesc(_("Control modes and mode locks on a channel"));
		this->SetSyntax(_("\037channel\037 LOCK {ADD|DEL|SET|LIST} [\037what\037]"));
		this->SetSyntax(_("\037channel\037 SET \037modes\037"));
		this->SetSyntax(_("\037channel\037 CLEAR [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &subcommand = params[1];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (!ci)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (subcommand.equals_ci("LOCK") && params.size() > 2)
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->GetName());
			else
				this->DoLock(source, ci, params);
		}
		else if (!ci->c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
		else if (subcommand.equals_ci("SET") && params.size() > 2)
			this->DoSet(source, ci, params);
		else if (subcommand.equals_ci("CLEAR"))
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->GetName());
			else
				this->DoClear(source, ci, params);
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Controls mode locks and allows changing modes on a channel."
		               "\n"
		               "The \002{0} LOCK\002 command allows you to add, delete, and view mode locks on a channel."
		               " If a mode is locked on or off, services will not allow that mode to be changed."
		               " The \002SET\002 command will clear all existing mode locks and set the new one given, while \002ADD\002 and \002DEL\002 modify the existing mode lock.\n"
		               "\n"
		               "Example:\n"
		               "         {0} #channel LOCK ADD +bmnt *!*@*aol*\n"
		               "\n"
		               "The \002{0} SET\002 command allows you to set modes through services. Wildcards * and ? may be given as parameters for list and status modes.\n"
		               "Example:\n"
		               "         {0} #channel SET +v *\n"
			       "         Sets voice status to all users in the channel.\n"
		               "\n"
		               "         {0} #channel SET -b ~c:*\n"
		               "         Clears all extended bans that start with ~c:\n"
		               "\n"
		               "The \002{0} CLEAR\002 command is an easy way to clear modes on a channel. \037what\037 may be any mode name. Examples include bans, excepts, inviteoverrides, ops, halfops, and voices."
		               " If \037what\037 is not given then all basic modes are removed."),
		               source.command.upper());
		return true;
	}
};

static Anope::map<std::pair<bool, Anope::string> > modes;

class CommandCSModes : public Command
{
 public:
	CommandCSModes(Module *creator) : Command(creator, "chanserv/modes", 1, 2)
	{
		this->SetSyntax(_("\037channel\037 [\037user\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		User *u = source.GetUser(),
			*targ = params.size() > 1 ? User::Find(params[1], true) : u;

		if (!targ)
		{
			source.Reply(_("User \002{0}\002 isn't currently online."), params.size() > 1 ? params[1] : source.GetNick());
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (!ci)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!ci->c)
		{
			source.Reply(_("Channel \002%s\002 doesn't exist."), ci->GetName());
			return;
		}

		ChanServ::AccessGroup u_access = source.AccessFor(ci), targ_access = ci->AccessFor(targ);
		const std::pair<bool, Anope::string> &m = modes[source.command];

		bool can_override = source.HasPriv("chanserv/administration");
		bool override = false;

		if (m.second.empty())
			return; // Configuration issue

		const Anope::string &want = u == targ ? m.second + "ME" : m.second;
		if (!u_access.HasPriv(want))
		{
			if (!can_override)
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), want, ci->GetName());
				return;
			}
			else
				override = true;
		}

		if (!override && !m.first && u != targ && (targ->IsProtected() || (ci->HasFieldS("PEACE") && targ_access >= u_access)))
		{
			if (!can_override)
			{
				source.Reply(_("Access denied. \002{0}\002 has the same or more privileges than you on \002{1}\002."), targ->nick, ci->GetName());
				return;
			}
			else
				override = true;
		}

		if (!ci->c->FindUser(targ))
		{
			source.Reply(_("User \002{0}\002 is not on channel \002{1}\002."), targ->nick, ci->GetName());
			return;
		}

		if (m.first)
			ci->c->SetMode(NULL, m.second, targ->GetUID());
		else
			ci->c->RemoveMode(NULL, m.second, targ->GetUID());

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "on " << targ->nick;
	}

 	const Anope::string GetDesc(CommandSource &source) const override
	{
		const std::pair<bool, Anope::string> &m = modes[source.command];
		if (!m.second.empty())
		{
			if (m.first)
				return Anope::printf(Language::Translate(source.GetAccount(), _("Gives you or the specified nick %s status on a channel")), m.second.c_str());
			else
				return Anope::printf(Language::Translate(source.GetAccount(), _("Removes %s status from you or the specified nick on a channel")), m.second.c_str());
		}
		else
			return "";
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		const std::pair<bool, Anope::string> &m = modes[source.command];
		if (m.second.empty())
			return false;

		if (m.first)
			source.Reply(_("Gives {0} status to the selected \037user\037 on \037channel\037. If \037user\037 is not given, it will give the status to you."),
					m.second.upper());
		else
			source.Reply(_("Removes {0} status from the selected \037user\037 on \037channel\037. If \037user\037 is not given, it will remove the status from you."),
					 m.second.upper());
		source.Reply(" ");
		source.Reply(_("Use of this command requires the \002{1}(ME)\002 privilege on \037channel\037."), m.second.upper());

		return true;
	}
};

class CSMode : public Module
	, public EventHook<Event::CheckModes>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::ChanInfo>
{
	CommandCSMode commandcsmode;
	CommandCSModes commandcsmodes;
	ModeLocksImpl modelock;
	ModeLockType modelock_type;

	EventHandlers<Event::MLockEvents> modelockevents;

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsmode(this)
		, commandcsmodes(this)
		, modelock(this)
		, modelock_type(this)
		, modelockevents(this)
	{
		events = &modelockevents;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		modes.clear();

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);

			const Anope::string &cname = block->Get<Anope::string>("name"),
					&cmd = block->Get<Anope::string>("command");

			if (cname.empty() || cmd != "chanserv/modes")
				continue;

			const Anope::string &set = block->Get<Anope::string>("set"),
					&unset = block->Get<Anope::string>("unset");

			if (set.empty() && unset.empty())
				continue;

			modes[cname] = std::make_pair(!set.empty(), !set.empty() ? set : unset);
		}
	}

	void OnCheckModes(Reference<Channel> &c) override
	{
		if (!c || !c->ci)
			return;

		ModeLocks::ModeList locks = mlocks->GetMLock(c->ci);
		for (ModeLock *ml : locks)
		{
			ChannelMode *cm = ModeManager::FindChannelModeByName(ml->GetName());
			if (!cm)
				continue;

			if (cm->type == MODE_REGULAR)
			{
				if (!c->HasMode(cm->name) && ml->GetSet())
					c->SetMode(NULL, cm, "", false);
				else if (c->HasMode(cm->name) && !ml->GetSet())
					c->RemoveMode(NULL, cm, "", false);
			}
			else if (cm->type == MODE_PARAM)
			{
				/* If the channel doesn't have the mode, or it does and it isn't set correctly */
				if (ml->GetSet())
				{
					Anope::string param;
					c->GetParam(cm->name, param);

					if (!c->HasMode(cm->name) || (!param.empty() && !ml->GetParam().empty() && !param.equals_cs(ml->GetParam())))
						c->SetMode(NULL, cm, ml->GetParam(), false);
				}
				else
				{
					if (c->HasMode(cm->name))
						c->RemoveMode(NULL, cm, "", false);
				}
			}
			else if (cm->type == MODE_LIST || cm->type == MODE_STATUS)
			{
				if (ml->GetSet())
					c->SetMode(NULL, cm, ml->GetParam(), false);
				else
					c->RemoveMode(NULL, cm, ml->GetParam(), false);
			}
		}
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		Anope::string mlock;
		spacesepstream sep(Config->GetModule(this)->Get<Anope::string>("mlock", "+nt"));
		if (sep.GetToken(mlock))
		{
			bool add = true;
			for (unsigned i = 0; i < mlock.length(); ++i)
			{
				if (mlock[i] == '+')
				{
					add = true;
					continue;
				}

				if (mlock[i] == '-')
				{
					add = false;
					continue;
				}

				ChannelMode *cm = ModeManager::FindChannelModeByChar(mlock[i]);
				if (!cm)
					continue;

				Anope::string param;
				if (cm->type == MODE_PARAM)
				{
					ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
					if (add || !cmp->minus_no_arg)
					{
						sep.GetToken(param);
						if (param.empty() || !cmp->IsValid(param))
							continue;
					}
				}
				else if (cm->type != MODE_REGULAR)
				{
					sep.GetToken(param);
					if (param.empty())
						continue;
				}

				mlocks->SetMLock(ci, cm, add, param);
			}
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		const Anope::string &m = mlocks->GetMLockAsString(ci, true);
		if (!m.empty())
			info[_("Mode lock")] = m;
	}
};

MODULE_INIT(CSMode)
