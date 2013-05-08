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

class CommandCSMode : public Command
{
	bool CanSet(CommandSource &source, ChannelInfo *ci, ChannelMode *cm, bool self)
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

		if ((subcommand.equals_ci("ADD") || subcommand.equals_ci("SET")) && !param.empty())
		{
			/* If setting, remove the existing locks */
			if (subcommand.equals_ci("SET"))
			{
				const ChannelInfo::ModeList &mlocks = ci->GetMLock();
				for (ChannelInfo::ModeList::const_iterator it = mlocks.begin(), it_next; it != mlocks.end(); it = it_next)
				{
					const ModeLock *ml = it->second;
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
					it_next = it;
					++it_next;
					if (cm && cm->CanSet(source.GetUser()))
						ci->RemoveMLock(cm, ml->set, ml->param);
				}
			}

			spacesepstream sep(param);
			Anope::string modes;

			sep.GetToken(modes);

			Anope::string pos = "+", neg = "-", pos_params, neg_params;
			
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
					default:
						if (adding == -1)
							break;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
						if (!cm)
						{
							source.Reply(_("Unknown mode character %c ignored."), modes[i]);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode %c."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (((cm->type == MODE_STATUS || cm->type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
							source.Reply(_("Missing parameter for mode %c."), cm->mchar);
						else if (cm->type == MODE_LIST && ci->c && IRCD->GetMaxListFor(ci->c) && ci->c->HasMode(cm->name) >= IRCD->GetMaxListFor(ci->c))
							source.Reply(_("List for mode %c is full."), cm->mchar);
						else
						{
							ci->SetMLock(cm, adding, mode_param, source.GetNick()); 

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

			source.Reply(_("%s locked on %s."), ci->GetMLockAsString(true).c_str(), ci->name.c_str());
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to lock " << ci->GetMLockAsString(true);

			if (ci->c)
				ci->c->CheckModes();
		}
		else if (subcommand.equals_ci("DEL") && !param.empty())
		{
			spacesepstream sep(param);
			Anope::string modes;

			sep.GetToken(modes);

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
					default:
						if (adding == -1)
							break;
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
						if (!cm)
						{
							source.Reply(_("Unknown mode character %c ignored."), modes[i]);
							break;
						}
						else if (u && !cm->CanSet(u))
						{
							source.Reply(_("You may not (un)lock mode %c."), modes[i]);
							break;
						}

						Anope::string mode_param;
						if (cm->type != MODE_REGULAR && !sep.GetToken(mode_param))
							source.Reply(_("Missing parameter for mode %c."), cm->mchar);
						else
						{
							if (ci->RemoveMLock(cm, adding, mode_param))
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
		}
		else if (subcommand.equals_ci("LIST"))
		{
			const ChannelInfo::ModeList &mlocks = ci->GetMLock();
			if (mlocks.empty())
			{
				source.Reply(_("Channel %s has no mode locks."), ci->name.c_str());
			}
			else
			{
				ListFormatter list;
				list.AddColumn("Mode").AddColumn("Param").AddColumn("Creator").AddColumn("Created");

				for (ChannelInfo::ModeList::const_iterator it = mlocks.begin(), it_end = mlocks.end(); it != it_end; ++it)
				{
					const ModeLock *ml = it->second;
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
					if (!cm)
						continue;

					ListFormatter::ListEntry entry;
					entry["Mode"] = Anope::printf("%c%c", ml->set ? '+' : '-', cm->mchar);
					entry["Param"] = ml->param;
					entry["Creator"] = ml->setter;
					entry["Created"] = Anope::strftime(ml->created, source.nc, false);
					list.AddEntry(entry);
				}

				source.Reply(_("Mode locks for %s:"), ci->name.c_str());

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);
			}
		}
		else
			this->OnSyntaxError(source, subcommand);
	}
	
	void DoSet(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.GetUser();

		bool has_access = source.AccessFor(ci).HasPriv("MODE") || source.HasPriv("chanserv/administration");

		spacesepstream sep(params.size() > 3 ? params[3] : "");
		Anope::string modes = params[2], param;

		bool override = !source.AccessFor(ci).HasPriv("MODE") && source.HasPriv("chanserv/administration");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to set " << params[2];

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
						if (!cm)
							continue;
						if (!u || cm->CanSet(u))
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
					if (!cm || (u && !cm->CanSet(u)))
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
									source.Reply(_("You do not have access to set mode %c."), cm->mchar);
									break;
								}

								for (Channel::ChanUserList::const_iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end;)
								{
									ChanUserContainer *uc = it->second;
									++it;

									AccessGroup targ_access = ci->AccessFor(uc->user);

									if (uc->user->IsProtected() || (ci->HasExt("PEACE") && targ_access >= u_access))
									{
										source.Reply(_("You do not have the access to change %s's modes."), uc->user->nick.c_str());
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
									source.Reply(NICK_X_NOT_IN_USE, param.c_str());
									break;
								}

								if (!this->CanSet(source, ci, cm, source.GetUser() == target))
								{
									source.Reply(_("You do not have access to set mode %c."), cm->mchar);
									break;
								}

								if (source.GetUser() != target)
								{
									AccessGroup targ_access = ci->AccessFor(target);
									if (ci->HasExt("PEACE") && targ_access >= u_access)
									{
										source.Reply(_("You do not have the access to change %s's modes."), target->nick.c_str());
										break;
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

	void DoClear(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			std::vector<Anope::string> new_params;
			new_params.push_back(params[0]);
			new_params.push_back("SET");
			new_params.push_back("-*");
			this->DoSet(source, ci, new_params);
		}
		else if (param.equals_ci("BANS") || param.equals_ci("EXEMPTS") || param.equals_ci("INVITEOVERRIDES") || param.equals_ci("VOICES") || param.equals_ci("HALFOPS") || param.equals_ci("OPS"))
		{
			const Anope::string &mname = param.upper().substr(0, param.length() - 1);
			ChannelMode *cm = ModeManager::FindChannelModeByName(mname);
			if (cm == NULL)
			{
				source.Reply(_("Your IRCD does not support %s."), mname.upper().c_str());
				return;
			}

			std::vector<Anope::string> new_params;
			new_params.push_back(params[0]);
			new_params.push_back("SET");
			new_params.push_back("-" + stringify(cm->mchar));
			new_params.push_back("*");
			this->DoSet(source, ci, new_params);
		}
		else
			this->SendSyntax(source);
	}

 public:
	CommandCSMode(Module *creator) : Command(creator, "chanserv/mode", 2, 4)
	{
		this->SetDesc(_("Control modes and mode locks on a channel"));
		this->SetSyntax(_("\037channel\037 LOCK {ADD|DEL|SET|LIST} [\037what\037]"));
		this->SetSyntax(_("\037channel\037 SET \037modes\037"));
		this->SetSyntax(_("\037channel\037 CLEAR [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &subcommand = params[1];

		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (!ci || !ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (subcommand.equals_ci("LOCK") && params.size() > 2)
		{
			if (!source.AccessFor(ci).HasPriv("MODE") && !source.HasPriv("chanserv/administration"))
				source.Reply(ACCESS_DENIED);
			else
				this->DoLock(source, ci, params);
		}
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Mainly controls mode locks and mode access (which is different from channel access)\n"
			"on a channel.\n"
			" \n"
			"The \002%s LOCK\002 command allows you to add, delete, and view mode locks on a channel.\n"
			"If a mode is locked on or off, services will not allow that mode to be changed. The \2SET\2\n"
			"command will clear all existing mode locks and set the new one given, while \2ADD\2 and \2DEL\2\n"
			"modify the existing mode lock.\n"
			"Example:\n"
			"     \002MODE #channel LOCK ADD +bmnt *!*@*aol*\002\n"
			" \n"
			"The \002%s SET\002 command allows you to set modes through services. Wildcards * and ? may\n"
			"be given as parameters for list and status modes.\n"
			"Example:\n"
			"     \002MODE #channel SET +v *\002\n"
			"       Sets voice status to all users in the channel.\n"
			" \n"
			"     \002MODE #channel SET -b ~c:*\n"
			"       Clears all extended bans that start with ~c:\n"
			" \n"
			"The \002%s CLEAR\002 command is an easy way to clear modes on a channel. \037what\037 may be\n"
			"one of bans, exempts, inviteoverrides, ops, halfops, or voices. If \037what\037 is not given then all\n"
			"basic modes are removed."),
			source.command.upper().c_str(), source.command.upper().c_str(), source.command.upper().c_str());
		return true;
	}
};

class CSMode : public Module
{
	CommandCSMode commandcsmode;

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsmode(this)
	{

	}
};

MODULE_INIT(CSMode)
