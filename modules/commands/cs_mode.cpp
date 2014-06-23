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

struct ModeLockImpl : ModeLock, Serializable
{
	ModeLockImpl() : Serializable("ModeLock")
	{
	}

	~ModeLockImpl()
	{
		ChanServ::Channel *chan = ChanServ::Find(ci);
		if (chan)
		{
			ModeLocks *ml = chan->GetExt<ModeLocks>("modelocks");
			if (ml)
				ml->RemoveMLock(this);
		}
	}

	void Serialize(Serialize::Data &data) const override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data);
};

struct ModeLocksImpl : ModeLocks
{
	Serialize::Reference<ChanServ::Channel> ci;
	Serialize::Checker<ModeList> mlocks;

	ModeLocksImpl(Extensible *obj) : ci(anope_dynamic_static_cast<ChanServ::Channel *>(obj)), mlocks("ModeLock")
	{
	}

	~ModeLocksImpl()
	{
		for (ModeList::iterator it = this->mlocks->begin(); it != this->mlocks->end();)
		{
			ModeLock *ml = *it;
			++it;
			delete ml;
		}
	}

	bool HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const override
	{
		if (!mode)
			return false;

		for (ModeList::const_iterator it = this->mlocks->begin(); it != this->mlocks->end(); ++it)
		{
			const ModeLock *ml = *it;

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

		ModeLock *ml = new ModeLockImpl();
		ml->ci = ci->name;
		ml->set = status;
		ml->name = mode->name;
		ml->param = param;
		ml->setter = setter;
		ml->created = created;

		EventReturn MOD_RESULT;
		MOD_RESULT = (*events)(&Event::MLockEvents::OnMLock, this->ci, ml);
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

		for (ModeList::iterator it = this->mlocks->begin(); it != this->mlocks->end(); ++it)
		{
			ModeLock *m = *it;

			if (m->name == mode->name)
			{
				// For list or status modes, we must check the parameter
				if (mode->type == MODE_LIST || mode->type == MODE_STATUS)
					if (m->param != param)
						continue;

				EventReturn MOD_RESULT;
				MOD_RESULT = (*events)(&Event::MLockEvents::OnUnMLock, this->ci, m);
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
		for (unsigned i = 0; i < ml.size(); ++i)
			delete ml[i];
	}

	const ModeList &GetMLock() const override
	{
		return this->mlocks;
	}

	std::list<ModeLock *> GetModeLockList(const Anope::string &name) override
	{
		std::list<ModeLock *> mlist;
		for (ModeList::const_iterator it = this->mlocks->begin(); it != this->mlocks->end(); ++it)
		{
			ModeLock *m = *it;
			if (m->name == name)
				mlist.push_back(m);
		}
		return mlist;
	}

	const ModeLock *GetMLock(const Anope::string &mname, const Anope::string &param = "") override
	{
		for (ModeList::const_iterator it = this->mlocks->begin(); it != this->mlocks->end(); ++it)
		{
			ModeLock *m = *it;

			if (m->name == mname && m->param == param)
				return m;
		}

		return NULL;
	}

	Anope::string GetMLockAsString(bool complete) const override
	{
		Anope::string pos = "+", neg = "-", params;

		for (ModeList::const_iterator it = this->mlocks->begin(); it != this->mlocks->end(); ++it)
		{
			const ModeLock *ml = *it;
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

void ModeLockImpl::Serialize(Serialize::Data &data) const
{
	data["ci"] << this->ci;
	data["set"] << this->set;
	data["name"] << this->name;
	data["param"] << this->param;
	data["setter"] << this->setter;
	data.SetType("created", Serialize::Data::DT_INT); data["created"] << this->created;
}

Serializable* ModeLockImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci;

	data["ci"] >> sci;

	ChanServ::Channel *ci = ChanServ::Find(sci);
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
		ModeLocks *modelocks = ci->Require<ModeLocks>("modelocks");

		if (Anope::ReadOnly && !subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if ((subcommand.equals_ci("ADD") || subcommand.equals_ci("SET")) && !param.empty())
		{
			/* If setting, remove the existing locks */
			if (subcommand.equals_ci("SET"))
			{
				const ModeLocks::ModeList mlocks = modelocks->GetMLock();
				for (ModeLocks::ModeList::const_iterator it = mlocks.begin(); it != mlocks.end(); ++it)
				{
					const ModeLock *ml = *it;
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
						else
						{
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
			}

			if (pos == "+")
				pos.clear();
			if (neg == "-")
				neg.clear();
			Anope::string reply = pos + neg + pos_params + neg_params;

			if (!reply.empty())
			{
				source.Reply(_("\002{0}\002 locked on \002{1}\002."), reply.c_str(), ci->name.c_str());
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
							if (modelocks->RemoveMLock(cm, adding, mode_param))
							{
								if (!mode_param.empty())
									mode_param = " " + mode_param;
								source.Reply(_("\002{0}{1}{2}\002 has been unlocked from \002{3}\002."), adding == 1 ? '+' : '-', cm->mchar, mode_param, ci->name);
								Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to unlock " << (adding ? '+' : '-') << cm->mchar << mode_param;
							}
							else
								source.Reply(_("\002{0}{1}\002 is not locked on \002{2}\002."), adding == 1 ? '+' : '-', cm->mchar, ci->name);
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
				source.Reply(_("Channel \002{0}\002 has no mode locks."), ci->name);
			}
			else
			{
				ListFormatter list(source.GetAccount());
				list.AddColumn(_("Mode")).AddColumn(_("Param")).AddColumn(_("Creator")).AddColumn(_("Created"));

				for (ModeLocks::ModeList::const_iterator it = mlocks.begin(), it_end = mlocks.end(); it != it_end; ++it)
				{
					const ModeLock *ml = *it;
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

				source.Reply(_("Mode locks for \002{0}\002:"), ci->name);

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);
			}
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
					for (unsigned j = 0; j < ModeManager::GetChannelModes().size(); ++j)
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

									if (uc->user->IsProtected() || (ci->HasExt("PEACE") && targ_access >= u_access && !can_override))
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
									if (ci->HasExt("PEACE") && targ_access >= u_access && !can_override)
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
								std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = ci->c->GetModeList(cm->name);
								for (; its.first != its.second;)
								{
									const Anope::string &mask = its.first->second;
									++its.first;

									if (Anope::Match(mask, param))
										ci->c->RemoveMode(NULL, cm, mask);
								}
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
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->name);
			else
				this->DoLock(source, ci, params);
		}
		else if (!ci->c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->name);
		else if (subcommand.equals_ci("SET") && params.size() > 2)
			this->DoSet(source, ci, params);
		else if (subcommand.equals_ci("CLEAR"))
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MODE", ci->name);
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
			source.Reply(_("Channel \002%s\002 doesn't exist."), ci->name);
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
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), want, ci->name);
				return;
			}
			else
				override = true;
		}

		if (!override && !m.first && u != targ && (targ->IsProtected() || (ci->HasExt("PEACE") && targ_access >= u_access)))
		{
			if (!can_override)
			{
				source.Reply(_("Access denied. \002{0}\002 has the same or more privileges than you on \002{1}\002."), targ->nick, ci->name);
				return;
			}
			else
				override = true;
		}

		if (!ci->c->FindUser(targ))
		{
			source.Reply(_("User \002{0}\002 is not on channel \002{1}\002."), targ->nick, ci->name);
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
	ExtensibleItem<ModeLocksImpl> modelocks;
	Serialize::Type modelocks_type;

	EventHandlers<Event::MLockEvents> modelockevents;

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CheckModes>("OnCheckModes")
		, EventHook<Event::ChanRegistered>("OnChanRegistered")
		, EventHook<Event::ChanInfo>("OnChanInfo")
		, commandcsmode(this)
		, commandcsmodes(this)
		, modelocks(this, "modelocks")
		, modelocks_type("ModeLock", ModeLockImpl::Unserialize)
		, modelockevents(this, "MLock")
	{
		events = &modelockevents;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		modes.clear();

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);

			const Anope::string &cname = block->Get<const Anope::string>("name"),
					&cmd = block->Get<const Anope::string>("command");

			if (cname.empty() || cmd != "chanserv/modes")
				continue;

			const Anope::string &set = block->Get<const Anope::string>("set"),
					&unset = block->Get<const Anope::string>("unset");

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
			for (ModeLocks::ModeList::const_iterator it = locks->GetMLock().begin(), it_end = locks->GetMLock().end(); it != it_end; ++it)
			{
				const ModeLock *ml = *it;
				ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
				if (!cm)
					continue;

				if (cm->type == MODE_REGULAR)
				{
					if (!c->HasMode(cm->name) && ml->set)
						c->SetMode(NULL, cm, "", false);
					else if (c->HasMode(cm->name) && !ml->set)
						c->RemoveMode(NULL, cm, "", false);
				}
				else if (cm->type == MODE_PARAM)
				{
					/* If the channel doesnt have the mode, or it does and it isn't set correctly */
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

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		ModeLocks *ml = modelocks.Require(ci);
		Anope::string mlock;
		spacesepstream sep(Config->GetModule(this)->Get<const Anope::string>("mlock", "+nt"));
		if (sep.GetToken(mlock))
		{
			bool add = true;
			for (unsigned i = 0; i < mlock.length(); ++i)
			{
				if (mlock[i] == '+')
					add = true;
				else if (mlock[i] == '-')
					add = false;
				else
				{
					ChannelMode *cm = ModeManager::FindChannelModeByChar(mlock[i]);
					Anope::string param;
					if (cm && (cm->type == MODE_REGULAR || sep.GetToken(param)))
						ml->SetMLock(cm, add, param);
				}
			}
		}
		ml->Check();
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		ModeLocks *ml = modelocks.Get(ci);
		if (ml)
			info[_("Mode lock")] = ml->GetMLockAsString(true);
	}
};

MODULE_INIT(CSMode)
