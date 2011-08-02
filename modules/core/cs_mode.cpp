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

class CommandCSMode : public Command
{
	bool CanSet(User *u, ChannelInfo *ci, ChannelModeName mode)
	{
		switch (mode)
		{
			case CMODE_OWNER:
				return ci->HasPriv(u, CA_OWNER);
			case CMODE_PROTECT:
				return ci->HasPriv(u, CA_PROTECT);
			case CMODE_OP:	
				return ci->HasPriv(u, CA_OPDEOP);
			case CMODE_HALFOP:
				return ci->HasPriv(u, CA_HALFOP);
			case CMODE_VOICE:
				return ci->HasPriv(u, CA_VOICE);
			default:
				break;
		}

		return false;
	}

	void DoLock(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &subcommand = params[2];
		const Anope::string &param = params.size() > 3 ? params[3] : "";

		if (subcommand.equals_ci("ADD") && !param.empty())
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
						if (!cm || !cm->CanSet(u))
						{
							source.Reply(_("Unknown mode character %c ignored."), modes[i]);
							break;
						}
						Anope::string mode_param;
						if (((cm->Type == MODE_STATUS || cm->Type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->Type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
							source.Reply(_("Missing parameter for mode %c."), cm->ModeChar);
						else
						{
							ci->SetMLock(cm, adding, mode_param, u->nick); 
							if (!mode_param.empty())
								mode_param = " " + mode_param;
							source.Reply(_("%c%c%s locked on %s"), adding ? '+' : '-', cm->ModeChar, mode_param.c_str(), ci->name.c_str());
						}
				}
			}

			if (ci->c)
				check_modes(ci->c);
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
						if (!cm || !cm->CanSet(u))
						{
							source.Reply(_("Unknown mode character %c ignored."), modes[i]);
							break;
						}
						Anope::string mode_param;
						if (!cm->Type == MODE_REGULAR && !sep.GetToken(mode_param))
							source.Reply(_("Missing parameter for mode %c."), cm->ModeChar);
						else
						{
							if (ci->RemoveMLock(cm, mode_param))
							{
								if (!mode_param.empty())
									mode_param = " " + mode_param;
								source.Reply(_("%c%c%s has been unlocked from %s."), adding == 1 ? '+' : '-', cm->ModeChar, mode_param.c_str(), ci->name.c_str());
							}
							else
								source.Reply(_("%c is not locked on %s."), cm->ModeChar, ci->name.c_str());
						}
				}
			}
		}
		else if (subcommand.equals_ci("LIST"))
		{
			const std::multimap<ChannelModeName, ModeLock> &mlocks = ci->GetMLock();
			if (mlocks.empty())
			{
				source.Reply(_("Channel %s has no mode locks."), ci->name.c_str());
			}
			else
			{
				source.Reply(_("Mode locks for %s:"), ci->name.c_str());
				for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = mlocks.begin(), it_end = mlocks.end(); it != it_end; ++it)
				{
					const ModeLock &ml = it->second;
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
					if (!cm)
						continue;

					Anope::string modeparam = ml.param;
					if (!modeparam.empty())
						modeparam = " " + modeparam;
					source.Reply(_("%c%c%s, by %s on %s"), ml.set ? '+' : '-', cm->ModeChar, modeparam.c_str(), ml.setter.c_str(), do_strftime(ml.created).c_str());
				}
			}
		}
		else
			this->OnSyntaxError(source, subcommand);
	}
	
	void DoSet(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		spacesepstream sep(params.size() > 3 ? params[3] : "");
		Anope::string modes = params[2], param;

		Log(LOG_COMMAND, u, this, ci) << "to set " << params[2];

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
					if (adding == -1)
						break;
					for (unsigned j = 0; j < ModeManager::ChannelModes.size(); ++j)
					{
						ChannelMode *cm = ModeManager::ChannelModes[j];
						if (cm->CanSet(u))
						{
							if (cm->Type == MODE_REGULAR || (!adding && cm->Type == MODE_PARAM))
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
					if (!cm || !cm->CanSet(u))
						continue;
					switch (cm->Type)
					{
						case MODE_REGULAR:
							if (adding)
								ci->c->SetMode(NULL, cm);
							else
								ci->c->RemoveMode(NULL, cm);
							break;
						case MODE_PARAM:
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
								break;

							if (!this->CanSet(u, ci, cm->Name))
							{
								source.Reply(_("You do not have access to set mode %c."), cm->ModeChar);
								break;
							}

							AccessGroup u_access = ci->AccessFor(u);

							if (str_is_wildcard(param))
							{
								for (CUserList::const_iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
								{
									UserContainer *uc = *it;

									AccessGroup targ_access = ci->AccessFor(uc->user);

									if (targ_access > u_access)
									{
										source.Reply(_("You do not have the access to change %s's modes."), uc->user->nick.c_str());
										continue;
									}

									if (Anope::Match(uc->user->GetMask(), param))
									{
										if (adding)
											ci->c->SetMode(NULL, cm, uc->user->nick);
										else
											ci->c->RemoveMode(NULL, cm, uc->user->nick);
									}
								}
							}
							else
							{
								User *target = finduser(param);
								if (target != NULL)
								{
									AccessGroup targ_access = ci->AccessFor(target);
									if (targ_access > u_access)
									{
										source.Reply(_("You do not have the access to change %s's modes."), target->nick.c_str());
										break;
									}
								}
								if (adding)
									ci->c->SetMode(NULL, cm, param);
								else
									ci->c->RemoveMode(NULL, cm, param);
							}
							break;
						}
						case MODE_LIST:
							if (!sep.GetToken(param))
								break;
							if (adding)
								ci->c->SetMode(NULL, cm, param);
							else
							{
								std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = ci->c->GetModeList(cm->Name);
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

 public:
	CommandCSMode(Module *creator) : Command(creator, "chanserv/mode", 3, 4)
	{
		this->SetDesc(_("Control modes and mode locks on a channel"));
		this->SetSyntax(_("\037channel\037 LOCK {ADD|DEL|LIST} [\037what\037]"));
		this->SetSyntax(_("\037channel\037 SET \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &subcommand = params[1];

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);

		if (!ci || !ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!ci->HasPriv(u, CA_MODE) && !u->HasCommand("chanserv/chanserv/mode"))
			source.Reply(ACCESS_DENIED);
		else if (subcommand.equals_ci("LOCK"))
			this->DoLock(source, ci, params);
		else if (subcommand.equals_ci("SET"))
			this->DoSet(source, ci, params);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Mainly controls mode locks and mode access (which is different from channel access)\n"
			"on a channel.\n"
			" \n"
			"The \002MODE LOCK\002 command allows you to add, delete, and view mode locks on a channel.\n"
			"If a mode is locked on or off, services will not allow that mode to be changed.\n"
			"Example:\n"
			"     \002MODE #channel LOCK ADD +bmnt *!*@*aol*\002\n"
			" \n"
			"The \002MODE SET\002 command allows you to set modes through services. Wildcards * and ? may\n"
			"be given as parameters for list and status modes.\n"
			"Example:\n"
			"     \002MODE #channel SET +v *\002\n"
			"       Sets voice status to all users in the channel.\n"
			" \n"
			"     \002MODE #channel SET -b ~c:*\n"
			"       Clears all extended bans that start with ~c:"));
		return true;
	}
};

class CSMode : public Module
{
	CommandCSMode commandcsmode;

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsmode(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcsmode);
	}
};

MODULE_INIT(CSMode)
