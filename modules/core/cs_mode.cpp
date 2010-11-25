/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	void DoLock(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
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
							source.Reply(CHAN_MODE_LOCK_UNKNOWN, modes[i]);
							break;
						}
						Anope::string mode_param;
						if (((cm->Type == MODE_STATUS || cm->Type == MODE_LIST) && !sep.GetToken(mode_param)) || (cm->Type == MODE_PARAM && adding && !sep.GetToken(mode_param)))
							source.Reply(CHAN_MODE_LOCK_MISSING_PARAM, cm->ModeChar);
						else
						{
							ci->SetMLock(cm, adding, mode_param, u->nick); 
							if (!mode_param.empty())
								mode_param = " " + mode_param;
							source.Reply(CHAN_MODE_LOCKED, adding ? '+' : '-', cm->ModeChar, mode_param.c_str(), ci->name.c_str());
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
							source.Reply(CHAN_MODE_LOCK_UNKNOWN, modes[i]);
							break;
						}
						Anope::string mode_param;
						if (!cm->Type == MODE_REGULAR && !sep.GetToken(mode_param))
							source.Reply(CHAN_MODE_LOCK_MISSING_PARAM, cm->ModeChar);
						else
						{
							if (ci->RemoveMLock(cm, mode_param))
							{
								if (!mode_param.empty())
									mode_param = " " + mode_param;
								source.Reply(CHAN_MODE_UNLOCKED, adding == 1 ? '+' : '-', cm->ModeChar, mode_param.c_str(), ci->name.c_str());
							}
							else
								source.Reply(CHAN_MODE_NOT_LOCKED, cm->ModeChar, ci->name.c_str());
						}
				}
			}
		}
		else if (subcommand.equals_ci("LIST"))
		{
			const std::multimap<ChannelModeName, ModeLock> &mlocks = ci->GetMLock();
			if (mlocks.empty())
			{
				source.Reply(CHAN_MODE_LOCK_NONE, ci->name.c_str());
			}
			else
			{
				source.Reply(CHAN_MODE_LOCK_HEADER, ci->name.c_str());
				for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = mlocks.begin(), it_end = mlocks.end(); it != it_end; ++it)
				{
					const ModeLock &ml = it->second;
					ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
					if (!cm)
						continue;

					Anope::string modeparam = ml.param;
					if (!modeparam.empty())
						modeparam = " " + modeparam;
					Anope::string setter = ml.setter;
					if (setter.empty())
						setter = ci->founder ? ci->founder->display : "Unknown";
					source.Reply(CHAN_MODE_LIST_FMT, ml.set ? '+' : '-', cm->ModeChar, modeparam.c_str(), setter.c_str(), do_strftime(ml.created).c_str());
				}
			}
		}
		else
			this->OnSyntaxError(u, subcommand);
	}
	
	void DoSet(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

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
					for (std::map<Anope::string, Mode *>::const_iterator it = ModeManager::Modes.begin(), it_end = ModeManager::Modes.end(); it != it_end; ++it)
					{
						Mode *m = it->second;
						if (m->Class == MC_CHANNEL)
						{
							ChannelMode *cm = debug_cast<ChannelMode *>(m);
							if (cm->Type == MODE_REGULAR || (!adding && cm->Type == MODE_PARAM))
							{
								if (!cm->CanSet(u))
									continue;
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
							if (!sep.GetToken(param))
								break;
							if (str_is_wildcard(param))
							{
								for (CUserList::const_iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
								{
									UserContainer *uc = *it;

									if (Anope::Match(u->GetMask(), param))
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
								if (adding)
									ci->c->SetMode(NULL, cm, param);
								else
									ci->c->RemoveMode(NULL, cm, param);
							}
							break;
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
	CommandCSMode() : Command("MODE", 3, 4)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &subcommand = params[1];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (!ci || !ci->c)
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!check_access(u, ci, CA_MODE) && !u->Account()->HasCommand("chanserv/mode"))
			source.Reply(ACCESS_DENIED);
		else if (subcommand.equals_ci("LOCK"))
			this->DoLock(source, params);
		else if (subcommand.equals_ci("SET"))
			this->DoSet(source, params);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_MODE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "MODE", CHAN_MODE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_MODE);
	}
};

class CSMode : public Module
{
	CommandCSMode commandcsmode;

 public:
	CSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsmode);
	}
};

MODULE_INIT(CSMode)
