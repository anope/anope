/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/chanserv/mode.h"
#include "modules/chanserv/info.h"

class ModeLockImpl : public ModeLock
{
	friend class ModeLockType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<bool> set;
	Serialize::Storage<Anope::string> name, param, setter;
	Serialize::Storage<time_t> created;

 public:
	using ModeLock::ModeLock;

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
	Serialize::ObjectField<ModeLockImpl, ChanServ::Channel *> channel;
	Serialize::Field<ModeLockImpl, bool> set;
	Serialize::Field<ModeLockImpl, Anope::string> name, param, setter;
	Serialize::Field<ModeLockImpl, time_t> created;

	ModeLockType(Module *me) : Serialize::Type<ModeLockImpl>(me)
		, channel(this, "channel", &ModeLockImpl::channel, true)
		, set(this, "set", &ModeLockImpl::set)
		, name(this, "name", &ModeLockImpl::name)
		, param(this, "param", &ModeLockImpl::param)
		, setter(this, "setter", &ModeLockImpl::setter)
		, created(this, "created", &ModeLockImpl::created)
	{
	}
};

ChanServ::Channel *ModeLockImpl::GetChannel()
{
	return Get(&ModeLockType::channel);
}

void ModeLockImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&ModeLockType::channel, ci);
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

void ModeLockImpl::SetName(const Anope::string &n)
{
	Set(&ModeLockType::name, n);
}

Anope::string ModeLockImpl::GetParam()
{
	return Get(&ModeLockType::param);
}

void ModeLockImpl::SetParam(const Anope::string &p)
{
	Set(&ModeLockType::param, p);
}

Anope::string ModeLockImpl::GetSetter()
{
	return Get(&ModeLockType::setter);
}

void ModeLockImpl::SetSetter(const Anope::string &s)
{
	Set(&ModeLockType::setter, s);
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

		for (ModeLock *ml : ci->GetRefs<ModeLock *>())
			if (ml->GetName() == mode->name && ml->GetSet() == status && ml->GetParam() == param)
				return true;

		return false;
	}

	bool SetMLock(ChanServ::Channel *ci, ChannelMode *mode, bool status, const Anope::string &param = "", Anope::string setter = "", time_t created = Anope::CurTime) override
	{
		if (!mode)
			return false;

		RemoveMLock(ci, mode, status, param);

		if (setter.empty())
			setter = ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "Unknown";

		ModeLock *ml = Serialize::New<ModeLock *>();
		ml->SetChannel(ci);
		ml->SetSet(status);
		ml->SetName(mode->name);
		ml->SetParam(param);
		ml->SetSetter(setter);
		ml->SetCreated(created);

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::MLockEvents::OnMLock, ci, ml);
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

		for (ModeLock *m : ci->GetRefs<ModeLockImpl *>())
			if (m->GetName() == mode->name)
			{
				// For list or status modes, we must check the parameter
				if (mode->type == MODE_LIST || mode->type == MODE_STATUS)
					if (m->GetParam() != param)
						continue;

				EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::MLockEvents::OnUnMLock, ci, m);
				if (MOD_RESULT == EVENT_STOP)
					break;

				m->Delete();
				return true;
			}

		return false;
	}

	void ClearMLock(ChanServ::Channel *ci) override
	{
		for (ModeLock *m : ci->GetRefs<ModeLock *>())
			delete m;
	}

	ModeList GetMLock(ChanServ::Channel *ci) const override
	{
		return ci->GetRefs<ModeLock *>();
	}

	std::list<ModeLock *> GetModeLockList(ChanServ::Channel *ci, const Anope::string &name) override
	{
		std::list<ModeLock *> mlist;
		for (ModeLock *m : ci->GetRefs<ModeLock *>())
			if (m->GetName() == name)
				mlist.push_back(m);
		return mlist;
	}

	ModeLock *GetMLock(ChanServ::Channel *ci, const Anope::string &mname, const Anope::string &param = "") override
	{
		for (ModeLock *m : ci->GetRefs<ModeLock *>())
			if (m->GetName() == mname && m->GetParam() == param)
				return m;

		return NULL;
	}

	Anope::string GetMLockAsString(ChanServ::Channel *ci, bool complete) const override
	{
		Anope::string pos = "+", neg = "-", params;

		for (ModeLock *ml : ci->GetRefs<ModeLock *>())
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
	ServiceReference<ModeLocks> mlocks;
	
	bool CanSet(CommandSource &source, ChanServ::Channel *ci, ChannelMode *cm, bool self)
	{
		if (!ci || !cm || cm->type != MODE_STATUS)
			return false;

		return source.AccessFor(ci).HasPriv(cm->name + (self ? "ME" : ""));
	}

	void DoLock(CommandSource &source, ChanServ::Channel *ci, Channel *c, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();
		const Anope::string &subcommand = params[2];
		const Anope::string &param = params.size() > 3 ? params[3] : "";

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

						if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode \002{0}\002."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (((cm->type == MODE_STATUS || cm->type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
						{
							source.Reply(_("Missing parameter for mode \002{0}\002."), cm->mchar);
							break;
						}

						if (cm->type == MODE_STATUS && !CanSet(source, ci, cm, false))
						{
							source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), cm->name, ci->GetName());
							break;
						}

						if (cm->type == MODE_LIST && c && IRCD->GetMaxListFor(c) && c->HasMode(cm->name) >= IRCD->GetMaxListFor(c))
						{
							source.Reply(_("List for mode \002{0}\002 is full."), cm->mchar);
							break;
						}

						if (ci->GetRefs<ModeLock *>().size() >= Config->GetModule(this->GetOwner())->Get<unsigned>("max", "32"))
						{
							source.Reply(_("The mode lock list of \002{0}\002 is full."), ci->GetName());
							break;
						}

						if (!mlocks->SetMLock(ci, cm, adding, mode_param, source.GetNick()))
							continue;

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

			if (pos == "+")
				pos.clear();
			if (neg == "-")
				neg.clear();
			Anope::string reply = pos + neg + pos_params + neg_params;

			if (!reply.empty())
			{
				source.Reply(_("\002{0}\002 locked on \002{1}\002."), reply, ci->GetName());

				logger.Command(source, ci, _("{source} used {command} on {channel} to lock {0}"), reply);
			}
			else if (needreply)
			{
				source.Reply(_("Nothing to do."));
			}

			if (c)
				c->CheckModes();
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

						if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode \002{0}\002."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (cm->type != MODE_REGULAR && !sep.GetToken(mode_param))
						{
							source.Reply(_("Missing parameter for mode \002{0}\002."), cm->mchar);
							break;
						}

						if (mlocks->RemoveMLock(ci, cm, adding, mode_param))
						{
							if (!mode_param.empty())
								mode_param = " " + mode_param;
							source.Reply(_("\002{0}{1}{2}\002 has been unlocked from \002{3}\002."), adding == 1 ? '+' : '-', cm->mchar, mode_param, ci->GetName());

							logger.Command(source, ci, _("{source} used {command} on {channel} to unlock {0}"),
									(adding ? '+' : '-') + cm->mchar + mode_param);
						}
						else
						{
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
		{
			this->OnSyntaxError(source);
		}
	}

	void DoSet(CommandSource &source, ChanServ::Channel *ci, Channel *c, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();

		bool has_access = source.AccessFor(ci).HasPriv("MODE") || source.HasOverridePriv("chanserv/administration");

		spacesepstream sep(params.size() > 3 ? params[3] : "");
		Anope::string modes = params[2], param;

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
					for (unsigned j = 0; j < ModeManager::GetChannelModes().size() && c; ++j)
					{
						ChannelMode *cm = ModeManager::GetChannelModes()[j];

						if (!u || cm->CanSet(u) || source.IsOverride())
						{
							if (cm->type == MODE_REGULAR || (!adding && cm->type == MODE_PARAM))
							{
								if (adding)
									c->SetMode(NULL, cm);
								else
									c->RemoveMode(NULL, cm);
							}
						}
					}
					break;
				default:
					if (adding == -1)
						break;
					ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
					if (!cm || (u && !cm->CanSet(u) && !source.IsOverride()))
						continue;
					switch (cm->type)
					{
						case MODE_REGULAR:
							if (!has_access)
								break;
							if (adding)
								c->SetMode(NULL, cm);
							else
								c->RemoveMode(NULL, cm);
							break;
						case MODE_PARAM:
							if (!has_access)
								break;
							if (adding && !sep.GetToken(param))
								break;
							if (adding)
								c->SetMode(NULL, cm, param);
							else
								c->RemoveMode(NULL, cm);
							break;
						case MODE_STATUS:
						{
							if (!sep.GetToken(param))
								param = source.GetNick();

							ChanServ::AccessGroup u_access = source.AccessFor(ci);

							if (param.find_first_of("*?") != Anope::string::npos)
							{
								if (!this->CanSet(source, ci, cm, false) && !source.HasOverridePriv("chanserv/administration"))
								{
									source.Reply(_("You do not have access to set mode \002{0}\002."), cm->mchar);
									break;
								}

								for (Channel::ChanUserList::const_iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
								{
									ChanUserContainer *uc = it->second;

									ChanServ::AccessGroup targ_access = ci->AccessFor(uc->user);

									if (uc->user->IsProtected() || (ci->IsPeace() && targ_access >= u_access && !source.HasOverridePriv("chanserv/administration")))
									{
										source.Reply(_("You do not have the access to change the modes of \002{0}\002."), uc->user->nick.c_str());
										continue;
									}

									if (Anope::Match(uc->user->GetMask(), param))
									{
										if (adding)
											c->SetMode(NULL, cm, uc->user->GetUID());
										else
											c->RemoveMode(NULL, cm, uc->user->GetUID());
									}
								}
							}
							else
							{
								User *target = User::Find(param, true);
								if (target == nullptr)
								{
									source.Reply(_("User \002{0}\002 isn't currently online."), param);
									break;
								}

								if (!this->CanSet(source, ci, cm, source.GetUser() == target) && !source.HasOverridePriv("chanserv/administration"))
								{
									source.Reply(_("You do not have access to set mode \002{0}\002."), cm->mchar);
									break;
								}

								if (source.GetUser() != target)
								{
									ChanServ::AccessGroup targ_access = ci->AccessFor(target);
									if (ci->IsPeace() && targ_access >= u_access && !source.HasOverridePriv("chanserv/administration"))
									{
										source.Reply(_("You do not have the access to change the modes of \002{0}\002"), target->nick);
										break;
									}

									if (target->IsProtected())
									{
										source.Reply(_("Access denied. \002{0}\002 is protected and can not have their modes changed."), target->nick);
										break;
									}
								}

								if (adding)
									c->SetMode(NULL, cm, target->GetUID());
								else
									c->RemoveMode(NULL, cm, target->GetUID());
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
								if (IRCD->GetMaxListFor(c) && c->HasMode(cm->name) < IRCD->GetMaxListFor(c))
									c->SetMode(NULL, cm, param);
							}
							else
							{
								std::vector<Anope::string> v = c->GetModeList(cm->name);
								for (unsigned j = 0; j < v.size(); ++j)
									if (Anope::Match(v[j], param))
										c->RemoveMode(NULL, cm, v[j]);
							}
					}
			}
		}

		logger.Command(source, ci, _("{source} used {command} on {channel} to set {0}"),
				params[2] + (params.size() > 3 ? " " + params[3] : ""));

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
			this->DoSet(source, ci, ci->GetChannel(), new_params);
			return;
		}

		ChannelMode *cm;
		if (param.length() == 1)
		{
			cm = ModeManager::FindChannelModeByChar(param[0]);
		}
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
		this->DoSet(source, ci, ci->GetChannel(), new_params);
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

		Channel *c = ci->GetChannel();

		if (subcommand.equals_ci("LOCK") && params.size() > 2)
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasOverridePriv("chanserv/administration"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->GetName());
				return;
			}

			this->DoLock(source, ci, c, params);
			return;
		}

		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
			return;
		}

		if (subcommand.equals_ci("SET") && params.size() > 2)
		{
			this->DoSet(source, ci, c, params);
		}
		else if (subcommand.equals_ci("CLEAR"))
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->GetName());
			else
				this->DoClear(source, ci, params);
		}
		else
		{
			this->OnSyntaxError(source);
		}
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
		               source.GetCommand().upper());
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

		Channel *c = ci->GetChannel();
		if (!c)
		{
			source.Reply(_("Channel \002%s\002 doesn't exist."), ci->GetName());
			return;
		}

		ChanServ::AccessGroup u_access = source.AccessFor(ci), targ_access = ci->AccessFor(targ);
		const std::pair<bool, Anope::string> &m = modes[source.GetCommand()];

		if (m.second.empty())
			return; // Configuration issue

		const Anope::string &want = u == targ ? m.second + "ME" : m.second;
		if (!u_access.HasPriv(want))
		{
			if (!source.HasOverridePriv("chanserv/administration"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), want, ci->GetName());
				return;
			}
		}

		if (!m.first && u != targ && (targ->IsProtected() || (ci->IsPeace() && targ_access >= u_access)))
		{
			if (!source.HasOverridePriv("chanserv/administration"))
			{
				source.Reply(_("Access denied. \002{0}\002 has the same or more privileges than you on \002{1}\002."), targ->nick, ci->GetName());
				return;
			}
		}

		if (!c->FindUser(targ))
		{
			source.Reply(_("User \002{0}\002 is not on channel \002{1}\002."), targ->nick, ci->GetName());
			return;
		}

		if (m.first)
			c->SetMode(NULL, m.second, targ->GetUID());
		else
			c->RemoveMode(NULL, m.second, targ->GetUID());

		logger.Command(source, ci, _("{source} used {command} on {channel} on {3}"), targ->nick);
	}

 	const Anope::string GetDesc(CommandSource &source) const override
	{
		const std::pair<bool, Anope::string> &m = modes[source.GetCommand()];
		if (!m.second.empty())
		{
			if (m.first)
				return Anope::printf(Language::Translate(source.GetAccount(), _("Gives you or the specified nick %s status on a channel")), m.second.c_str());
			else
				return Anope::printf(Language::Translate(source.GetAccount(), _("Removes %s status from you or the specified nick on a channel")), m.second.c_str());
		}
		else
		{
			return "";
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		const std::pair<bool, Anope::string> &m = modes[source.GetCommand()];
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

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CheckModes>(this)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::ChanInfo>(this)
		, commandcsmode(this)
		, commandcsmodes(this)
		, modelock(this)
		, modelock_type(this)
	{
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
		if (!c || !c->GetChannel())
			return;

		ModeLocks::ModeList locks = modelock.GetMLock(c->GetChannel());
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

				modelock.SetMLock(ci, cm, add, param);
			}
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		const Anope::string &m = modelock.GetMLockAsString(ci, true);
		if (!m.empty())
			info[_("Mode lock")] = m;
	}
};

MODULE_INIT(CSMode)
