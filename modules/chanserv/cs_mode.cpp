/* ChanServ core functions
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
#include "modules/chanserv/mode.h"

struct ModeLockImpl final
	: ModeLock
	, Serializable
{
	ModeLockImpl() : Serializable("ModeLock")
	{
	}

	~ModeLockImpl() override
	{
		ChannelInfo *chan = ChannelInfo::Find(ci);
		if (chan)
		{
			ModeLocks *ml = chan->GetExt<ModeLocks>("modelocks");
			if (ml)
				ml->RemoveMLock(this);
		}
	}
};

struct ModeLockTypeImpl final
	: Serialize::Type
{
	ModeLockTypeImpl()
		: Serialize::Type("ModeLock")
	{
	}
	void Serialize(Serializable *obj, Serialize::Data &data) const override;
	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
};

struct ModeLocksImpl final
	: ModeLocks
{
	Serialize::Reference<ChannelInfo> ci;
	Serialize::Checker<ModeList> mlocks;

	ModeLocksImpl(Extensible *obj) : ci(anope_dynamic_static_cast<ChannelInfo *>(obj)), mlocks("ModeLock")
	{
	}

	~ModeLocksImpl() override
	{
		ModeList modelist;
		mlocks->swap(modelist);
		for (auto *ml : modelist)
		{
			delete ml;
		}
	}

	bool HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const override
	{
		if (!mode)
			return false;

		for (auto *ml : *this->mlocks)
		{
			if (ml->name == mode->name && ml->set == status && ml->param == param)
				return true;
		}

		return false;
	}

	bool SetMLock(ChannelMode *mode, bool status, const Anope::string &param, Anope::string setter, time_t created = Anope::CurTime) override
	{
		if (!mode)
			return false;

		RemoveMLock(mode, status, param);

		if (setter.empty())
			setter = ci->GetFounder() ? ci->GetFounder()->display : "Unknown";

		auto *ml = new ModeLockImpl();
		ml->ci = ci->name;
		ml->set = status;
		ml->name = mode->name;
		ml->param = param;
		ml->setter = setter;
		ml->created = created;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnMLock, MOD_RESULT, (this->ci, ml));
		if (MOD_RESULT == EVENT_STOP)
		{
			delete ml;
			return false;
		}

		this->mlocks->push_back(ml);
		return true;
	}

	bool RemoveMLock(ChannelMode *mode, bool status, const Anope::string &param = "") override
	{
		if (!mode)
			return false;

		for (auto *m : *this->mlocks)
		{
			if (m->name == mode->name)
			{
				// For list or status modes, we must check the parameter
				if (mode->type == MODE_LIST || mode->type == MODE_STATUS)
					if (m->param != param)
						continue;

				EventReturn MOD_RESULT;
				FOREACH_RESULT(OnUnMLock, MOD_RESULT, (this->ci, m));
				if (MOD_RESULT == EVENT_STOP)
					break;

				delete m;
				return true;
			}
		}

		return false;
	}

	void RemoveMLock(ModeLock *mlock) override
	{
		ModeList::iterator it = std::find(this->mlocks->begin(), this->mlocks->end(), mlock);
		if (it != this->mlocks->end())
			this->mlocks->erase(it);
	}

	void ClearMLock() override
	{
		ModeList ml;
		this->mlocks->swap(ml);
		for (const auto *lock : ml)
			delete lock;
	}

	const ModeList &GetMLock() const override
	{
		return this->mlocks;
	}

	std::list<ModeLock *> GetModeLockList(const Anope::string &name) override
	{
		std::list<ModeLock *> mlist;
		for (auto *m : *this->mlocks)
		{
				if (m->name == name)
				mlist.push_back(m);
		}
		return mlist;
	}

	const ModeLock *GetMLock(const Anope::string &mname, const Anope::string &param = "") override
	{
		for (auto *m : *this->mlocks)
		{
			if (m->name == mname && m->param == param)
				return m;
		}

		return NULL;
	}

	Anope::string GetMLockAsString(bool complete) const override
	{
		Anope::string pos = "+", neg = "-", params;

		for (auto *ml : *this->mlocks)
		{
			ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);

			if (!cm || cm->type == MODE_LIST || cm->type == MODE_STATUS)
				continue;

			if (ml->set)
				pos += cm->mchar;
			else
				neg += cm->mchar;

			if (complete && ml->set && !ml->param.empty() && cm->type == MODE_PARAM)
				params += " " + ml->param;
		}

		if (pos.length() == 1)
			pos.clear();
		if (neg.length() == 1)
			neg.clear();

		return pos + neg + params;
	}

	void Check() override
	{
		if (this->mlocks->empty())
			ci->Shrink<ModeLocks>("modelocks");
	}
};

void ModeLockTypeImpl::Serialize(Serializable *obj, Serialize::Data &data) const
{
	const auto *ml = static_cast<const ModeLockImpl *>(obj);
	data.Store("ci", ml->ci);
	data.Store("set", ml->set);
	data.Store("name", ml->name);
	data.Store("param", ml->param);
	data.Store("setter", ml->setter);
	data.Store("created", ml->created);
}

Serializable *ModeLockTypeImpl::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	Anope::string sci;

	data["ci"] >> sci;

	ChannelInfo *ci = ChannelInfo::Find(sci);
	if (!ci)
		return NULL;

	ModeLockImpl *ml;
	if (obj)
		ml = anope_dynamic_static_cast<ModeLockImpl *>(obj);
	else
	{
		ml = new ModeLockImpl();
		ml->ci = ci->name;
	}

	data["set"] >> ml->set;
	data["created"] >> ml->created;
	data["setter"] >> ml->setter;
	data["name"] >> ml->name;
	data["param"] >> ml->param;

	if (!obj)
		ci->Require<ModeLocksImpl>("modelocks")->mlocks->push_back(ml);

	return ml;
}

class CommandCSMode final
	: public Command
{
	static bool CanSet(CommandSource &source, ChannelInfo *ci, ChannelMode *cm, bool self)
	{
		if (!ci || !cm || cm->type != MODE_STATUS)
			return false;

		return source.AccessFor(ci).HasPriv(cm->name + (self ? "ME" : ""));
	}

	void DoLock(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();
		const Anope::string &subcommand = params[2];
		const Anope::string &param = params.size() > 3 ? params[3] : "";

		bool override = !source.AccessFor(ci).HasPriv("MODE");
		ModeLocks *modelocks = ci->Require<ModeLocks>("modelocks");

		if (Anope::ReadOnly && !subcommand.equals_ci("LIST"))
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if ((subcommand.equals_ci("ADD") || subcommand.equals_ci("SET")) && !param.empty())
		{
			/* If setting, remove the existing locks */
			if (subcommand.equals_ci("SET"))
			{
				const ModeLocks::ModeList mlocks = modelocks->GetMLock();
				for (auto *ml : mlocks)
				{
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
					if (cm && cm->CanSet(source.GetUser()))
						modelocks->RemoveMLock(cm, ml->set, ml->param);
				}
			}

			spacesepstream sep(param);
			Anope::string modes;

			sep.GetToken(modes);

			Anope::string pos = "+", neg = "-", pos_params, neg_params;

			int adding = 1;
			bool needreply = true;
			for (auto mode : modes)
			{
				switch (mode)
				{
					case '+':
						adding = 1;
						break;
					case '-':
						adding = 0;
						break;
					default:
						needreply = false;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(mode);
						if (!cm)
						{
							source.Reply(_("Unknown mode character %c ignored."), mode);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode %c."), mode);
							break;
						}

						Anope::string mode_param;
						if (((cm->type == MODE_STATUS || cm->type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
						{
							source.Reply(_("Missing parameter for mode %c."), cm->mchar);
							continue;
						}

						if (cm->type == MODE_STATUS && !CanSet(source, ci, cm, false))
						{
							source.Reply(ACCESS_DENIED);
							continue;
						}

						if (cm->type == MODE_LIST && ci->c && IRCD->GetMaxListFor(ci->c, cm) && ci->c->HasMode(cm->name) >= IRCD->GetMaxListFor(ci->c, cm))
						{
							source.Reply(_("List for mode %c is full."), cm->mchar);
							continue;
						}

						if (modelocks->GetMLock().size() >= Config->GetModule(this->owner).Get<unsigned>("max", "50"))
						{
							source.Reply(_("The mode lock list of \002%s\002 is full."), ci->name.c_str());
							continue;
						}

						modelocks->SetMLock(cm, adding, mode_param, source.GetNick());

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
				source.Reply(_("%s locked on %s."), reply.c_str(), ci->name.c_str());
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
			for (auto mode : modes)
			{
				switch (mode)
				{
					case '+':
						adding = 1;
						break;
					case '-':
						adding = 0;
						break;
					default:
						needreply = false;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(mode);
						if (!cm)
						{
							source.Reply(_("Unknown mode character %c ignored."), mode);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode %c."), mode);
							break;
						}

						Anope::string mode_param;
						if (cm->type != MODE_REGULAR && !sep.GetToken(mode_param))
							source.Reply(_("Missing parameter for mode %c."), cm->mchar);
						else
						{
							if (modelocks->RemoveMLock(cm, adding, mode_param))
							{
								if (!mode_param.empty())
									mode_param = " " + mode_param;
								source.Reply(_("%c%c%s has been unlocked from %s."), adding == 1 ? '+' : '-', cm->mchar, mode_param.c_str(), ci->name.c_str());
								Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to unlock " << (adding ? '+' : '-') << cm->mchar << mode_param;
							}
							else
								source.Reply(_("%c%c is not locked on %s."), adding == 1 ? '+' : '-', cm->mchar, ci->name.c_str());
						}
				}
			}

			if (needreply)
				source.Reply(_("Nothing to do."));
		}
		else if (subcommand.equals_ci("LIST"))
		{
			const ModeLocks::ModeList mlocks = modelocks->GetMLock();
			if (mlocks.empty())
			{
				source.Reply(_("Channel %s has no mode locks."), ci->name.c_str());
			}
			else
			{
				ListFormatter list(source.GetAccount());
				list.AddColumn(_("Mode")).AddColumn(_("Param")).AddColumn(_("Creator")).AddColumn(_("Created"));

				for (auto *ml : mlocks)
				{
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
					if (!cm)
						continue;

					ListFormatter::ListEntry entry;
					entry["Mode"] = Anope::printf("%c%c", ml->set ? '+' : '-', cm->mchar);
					entry["Param"] = ml->param;
					entry["Creator"] = ml->setter;
					entry["Created"] = Anope::strftime(ml->created, NULL, true);
					list.AddEntry(entry);
				}

				source.Reply(_("Mode locks for %s:"), ci->name.c_str());

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (const auto &reply : replies)
					source.Reply(reply);
			}
		}
		else
			this->OnSyntaxError(source, subcommand);
	}

	void DoSet(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();

		bool has_access = source.AccessFor(ci).HasPriv("MODE") || source.HasPriv("chanserv/administration");
		bool can_override = source.HasPriv("chanserv/administration");

		spacesepstream sep(params.size() > 3 ? params[3] : "");
		Anope::string modes = params[2], param;

		bool override = !source.AccessFor(ci).HasPriv("MODE") && source.HasPriv("chanserv/administration");

		int adding = -1;
		for (auto mode : modes)
		{
			switch (mode)
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
					ChannelMode *cm = ModeManager::FindChannelModeByChar(mode);
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

							AccessGroup u_access = source.AccessFor(ci);

							if (param.find_first_of("*?") != Anope::string::npos)
							{
								if (!this->CanSet(source, ci, cm, false))
								{
									if (can_override)
									{
										override = true;
									}
									else
									{
										source.Reply(_("You do not have access to set mode %c."), cm->mchar);
										break;
									}
								}

								for (Channel::ChanUserList::const_iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end;)
								{
									ChanUserContainer *uc = it->second;
									++it;

									AccessGroup targ_access = ci->AccessFor(uc->user);

									if (uc->user->IsProtected())
									{
										source.Reply(_("You do not have the access to change %s's modes."), uc->user->nick.c_str());
										continue;
									}

									if (ci->HasExt("PEACE") && targ_access >= u_access)
									{
										if (can_override)
										{
											override = true;
										}
										else
										{
											source.Reply(_("You do not have the access to change %s's modes."), uc->user->nick.c_str());
											continue;
										}
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
									source.Reply(NICK_X_NOT_IN_USE, param.c_str());
									break;
								}

								if (!this->CanSet(source, ci, cm, source.GetUser() == target))
								{
									if (can_override)
									{
										override = true;
									}
									else
									{
										source.Reply(_("You do not have access to set mode %c."), cm->mchar);
										break;
									}
								}

								if (source.GetUser() != target)
								{
									AccessGroup targ_access = ci->AccessFor(target);
									if (ci->HasExt("PEACE") && targ_access >= u_access)
									{
										source.Reply(_("You do not have the access to change %s's modes."), target->nick.c_str());
										break;
									}
									else if (can_override)
									{
										override = true;
									}
									else if (target->IsProtected())
									{
										source.Reply(ACCESS_DENIED);
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

							// Change to internal name, eg giving -b ~q:*
							cm = cm->Unwrap(param);

							if (adding)
							{
								if (IRCD->GetMaxListFor(ci->c, cm) && ci->c->HasMode(cm->name) < IRCD->GetMaxListFor(ci->c, cm))
									ci->c->SetMode(NULL, cm, param);
							}
							else
							{
								std::vector<Anope::string> v = ci->c->GetModeList(cm->name);
								for (const auto &mode :  v)
								{
									if (Anope::Match(mode, param))
										ci->c->RemoveMode(NULL, cm, mode);
								}
							}
					}
			} // switch
		}

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to set " << modes << (params.size() > 3 ? " " + params[3] : "");
	}

	void DoClear(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			std::vector<Anope::string> new_params;
			new_params.push_back(params[0]);
			new_params.emplace_back("SET");
			new_params.emplace_back("-*");
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
			source.Reply(_("There is no such mode %s."), param.c_str());
			return;
		}

		if (cm->type != MODE_STATUS && cm->type != MODE_LIST)
		{
			source.Reply(_("Mode %s is not a status or list mode."), param.c_str());
			return;
		}

		if (!cm->mchar)
		{
			source.Reply(_("Mode %s is a virtual mode and can't be cleared."), cm->name.c_str());
			return;
		}

		std::vector<Anope::string> new_params;
		new_params.push_back(params[0]);
		new_params.emplace_back("SET");
		new_params.push_back("-" + Anope::ToString(cm->mchar));
		new_params.emplace_back("*");
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
		const Anope::string &subcommand = params[1];

		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (!ci)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (subcommand.equals_ci("LOCK") && params.size() > 2)
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(ACCESS_DENIED);
			else
				this->DoLock(source, ci, params);
		}
		else if (!ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (subcommand.equals_ci("SET") && params.size() > 2)
			this->DoSet(source, ci, params);
		else if (subcommand.equals_ci("CLEAR"))
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(ACCESS_DENIED);
			else
				this->DoClear(source, ci, params);
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Mainly controls mode locks and mode access (which is different from channel access) "
				"on a channel."
				"\n\n"
				"The \002%s\032LOCK\002 command allows you to add, delete, and view mode locks on a channel. "
				"If a mode is locked on or off, services will not allow that mode to be changed. The \002SET\002 "
				"command will clear all existing mode locks and set the new one given, while \002ADD\002 and \002DEL\002 "
				"modify the existing mode lock."
				"\n\n"
				"Example:\n"
				"     \002%s\032#channel\032%s\032ADD\032+bmnt\032*!*@*aol*\002\n"
				"\n\n"
				"The \002%s\032SET\002 command allows you to set modes through services. Wildcards * and ? may "
				"be given as parameters for list and status modes."
				"\n\n"
				"Example:\n"
				"     \002%s\032#channel\032SET\032+v\032*\002\n"
				"       Sets voice status to all users in the channel."
				"\n\n"
				"     \002%s\032#channel\032SET\032-b\032~c:*\n"
				"       Clears all extended bans that start with ~c:"
				"\n\n"
				"The \002%s\032CLEAR\002 command is an easy way to clear modes on a channel. \037what\037 may be "
				"any mode name. Examples include bans, excepts, inviteoverrides, ops, halfops, and voices. If \037what\037 "
				"is not given then all basic modes are removed."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());
		return true;
	}
};

static Anope::map<std::pair<bool, Anope::string> > modes;

class CommandCSModes final
	: public Command
{
private:
	void DoMode(CommandSource &source, ChannelInfo *ci, User *targ)
	{
		auto *u = source.GetUser();

		AccessGroup u_access = source.AccessFor(ci), targ_access = ci->AccessFor(targ);
		const std::pair<bool, Anope::string> &m = modes[source.command];

		bool can_override = source.HasPriv("chanserv/administration");
		bool override = false;

		if (m.second.empty())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (u == targ ? !u_access.HasPriv(m.second + "ME") : !u_access.HasPriv(m.second))
		{
			if (!can_override)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			else
				override = true;
		}

		if (!override && !m.first && u != targ && (targ->IsProtected() || (ci->HasExt("PEACE") && targ_access >= u_access)))
		{
			if (!can_override)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			else
				override = true;
		}

		if (!ci->c->FindUser(targ))
		{
			source.Reply(NICK_X_NOT_ON_CHAN, targ->nick.c_str(), ci->name.c_str());
			return;
		}

		if (m.first)
			ci->c->SetMode(NULL, m.second, targ->GetUID());
		else
			ci->c->RemoveMode(NULL, m.second, targ->GetUID());

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "on " << targ->nick;
	}

public:
	CommandCSModes(Module *creator) : Command(creator, "chanserv/modes", 1)
	{
		this->SetSyntax(_("\037channel\037 [\037user\037]+"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		auto *ci = ChannelInfo::Find(params[0]);
		if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		if (!ci->c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
			return;
		}

		if (params.size() == 1)
		{
			// The source is executing the command on themself.
			if (source.GetUser())
				DoMode(source, ci, source.GetUser());
			return;
		}

		// The source has provided list of nicks.
		for (size_t i = 1; i < params.size(); ++i)
		{
			auto &nick = params[i];
			auto *targ = User::Find(nick, true);
			if (!targ)
			{
				source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
				continue;
			}
			DoMode(source, ci, targ);
		}
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

		this->SendSyntax(source);
		source.Reply(" ");
		if (m.first)
		{
			source.Reply(_("Gives %s status to the selected nicks on a channel. If \037nick\037 is not given, it will %s you."),
				m.second.upper().c_str(), m.second.lower().c_str());
		}
		else
		{
			source.Reply(_("Removes %s status from the selected nicks on a channel. If \037nick\037 is not given, it will de%s you."),
				m.second.upper().c_str(), m.second.lower().c_str());
		}
		source.Reply(" ");
		source.Reply(_("You must have the %s(ME) privilege on the channel to use this command."), m.second.upper().c_str());

		return true;
	}
};

class CSMode final
	: public Module
{
	CommandCSMode commandcsmode;
	CommandCSModes commandcsmodes;
	ExtensibleItem<ModeLocksImpl> modelocks;
	ModeLockTypeImpl modelocks_type;

public:
	CSMode(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandcsmode(this)
		, commandcsmodes(this)
		, modelocks(this, "modelocks")
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		modes.clear();

		for (int i = 0; i < conf.CountBlock("command"); ++i)
		{
			const auto &block = conf.GetBlock("command", i);

			const Anope::string &cname = block.Get<const Anope::string>("name"),
					&cmd = block.Get<const Anope::string>("command");

			if (cname.empty() || cmd != "chanserv/modes")
				continue;

			const Anope::string &set = block.Get<const Anope::string>("set"),
					&unset = block.Get<const Anope::string>("unset");

			if (set.empty() && unset.empty())
				continue;

			modes[cname] = std::make_pair(!set.empty(), !set.empty() ? set : unset);
		}
	}

	void OnCheckModes(Reference<Channel> &c) override
	{
		if (!c || !c->ci)
			return;

		ModeLocks *locks = modelocks.Get(c->ci);
		if (locks)
		{
			for (auto *ml : locks->GetMLock())
			{
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
				if (!cm)
					continue;

				if (cm->type == MODE_REGULAR)
				{
					if (!c->HasMode(cm->name) && ml->set)
						c->SetMode(NULL, cm, {}, false);
					else if (c->HasMode(cm->name) && !ml->set)
						c->RemoveMode(NULL, cm, "", false);
				}
				else if (cm->type == MODE_PARAM)
				{
					/* If the channel doesn't have the mode, or it does and it isn't set correctly */
					if (ml->set)
					{
						Anope::string param;
						c->GetParam(cm->name, param);

						if (!c->HasMode(cm->name) || (!param.empty() && !ml->param.empty() && !param.equals_cs(ml->param)))
							c->SetMode(NULL, cm, ml->param, false);
					}
					else
					{
						if (c->HasMode(cm->name))
							c->RemoveMode(NULL, cm, "", false);
					}

				}
				else if (cm->type == MODE_LIST || cm->type == MODE_STATUS)
				{
					if (ml->set)
						c->SetMode(NULL, cm, ml->param, false);
					else
						c->RemoveMode(NULL, cm, ml->param, false);
				}
			}
		}
	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		ModeLocks *ml = modelocks.Require(ci);
		Anope::string mlock;
		spacesepstream sep(Config->GetModule(this).Get<const Anope::string>("mlock", "+nt"));
		if (sep.GetToken(mlock))
		{
			bool add = true;
			for (auto mode : mlock)
			{
				if (mode == '+')
				{
					add = true;
					continue;
				}

				if (mode == '-')
				{
					add = false;
					continue;
				}

				ChannelMode *cm = ModeManager::FindChannelModeByChar(mode);
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

				ml->SetMLock(cm, add, param);
			}
		}
		ml->Check();
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		ModeLocks *ml = modelocks.Get(ci);
		if (ml)
			info[_("Mode lock")] = ml->GetMLockAsString(true);
	}
};

MODULE_INIT(CSMode)
