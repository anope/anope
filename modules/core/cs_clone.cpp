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

class CommandCSClone : public Command
{
public:
	CommandCSClone() : Command("CLONE", 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		const Anope::string &channel = params[0];
		const Anope::string &target = params[1];
		Anope::string what = params.size() > 2 ? params[2] : "";

		ChannelInfo *ci = cs_findchan(channel);
		if (!check_access(u, ci, CA_SET))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}
		ChannelInfo *target_ci = cs_findchan(target);
		if (!target_ci)
		{
			u->SendMessage(ChanServ, CHAN_X_NOT_REGISTERED, target.c_str());
			return MOD_CONT;
		}
		if (!IsFounder(u, ci) || !IsFounder(u, target_ci))
		{
			u->SendMessage(ChanServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (Config->CSMaxReg && u->Account()->channelcount >= Config->CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
		{
			u->SendMessage(ChanServ, u->Account()->channelcount > Config->CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, Config->CSMaxReg);
			return MOD_CONT;
		}

		if (what.equals_ci("ALL"))
			what.clear();

		if (what.empty())
		{
			delete target_ci;
			target_ci = new ChannelInfo(ci);
			target_ci->name = target;
			RegisteredChannelList[target_ci->name] = target_ci;
			target_ci->c = findchan(target_ci->name);
			if (target_ci->c)
			{
				target_ci->c->ci = target_ci;

				check_modes(target_ci->c);

				ChannelMode *cm;
				if (u->FindChannel(target_ci->c) != NULL)
				{
					/* On most ircds you do not receive the admin/owner mode till its registered */
					if ((cm = ModeManager::FindChannelModeByName(CMODE_OWNER)))
						target_ci->c->SetMode(NULL, cm, u->nick);
					else if ((cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
						target_ci->c->RemoveMode(NULL, cm, u->nick);
				}

				/* Mark the channel as persistant */
				if (target_ci->c->HasMode(CMODE_PERM))
					target_ci->SetFlag(CI_PERSIST);
				/* Persist may be in def cflags, set it here */
				else if (target_ci->HasFlag(CI_PERSIST) && (cm = ModeManager::FindChannelModeByName(CMODE_PERM)))
					target_ci->c->SetMode(NULL, CMODE_PERM);
	
				if (target_ci->bi && target_ci->c->FindUser(target_ci->bi) == NULL)
					target_ci->bi->Join(target_ci->c);
			}

			if (target_ci->c && !target_ci->c->topic.empty())
			{
				target_ci->last_topic = target_ci->c->topic;
				target_ci->last_topic_setter = target_ci->c->topic_setter;
				target_ci->last_topic_time = target_ci->c->topic_time;
			}
			else
				target_ci->last_topic_setter = Config->s_ChanServ;

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(target_ci));

			u->SendMessage(ChanServ, CHAN_CLONED, channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("ACCESS"))
		{
			target_ci->ClearAccess();
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanAccess *access = ci->GetAccess(i);
				target_ci->AddAccess(access->nc, access->level, access->creator, access->last_seen);
			}

			u->SendMessage(ChanServ, CHAN_CLONED_ACCESS, channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("AKICK"))
		{
			target_ci->ClearAkick();
			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				AutoKick *akick = ci->GetAkick(i);
				if (akick->HasFlag(AK_ISNICK))
					target_ci->AddAkick(akick->creator, akick->nc, akick->reason, akick->addtime, akick->last_used);
				else
					target_ci->AddAkick(akick->creator, akick->mask, akick->reason, akick->addtime, akick->last_used);
			}

			u->SendMessage(ChanServ, CHAN_CLONED_AKICK, channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("BADWORDS"))
		{
			target_ci->ClearBadWords();
			for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
			{
				BadWord *bw = ci->GetBadWord(i);
				target_ci->AddBadWord(bw->word, bw->type);
			}

			u->SendMessage(ChanServ, CHAN_CLONED_BADWORDS, channel.c_str(), target.c_str());
		}
		else
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		Log(LOG_COMMAND, u, this, ci) << "to clone it to " << target_ci->name;

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CLONE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "CLONE", CHAN_CLONE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_CLONE);
	}
};

class CSClone : public Module
{
	CommandCSClone commandcsclone;

 public:
	CSClone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsclone);
	}
};

MODULE_INIT(CSClone)
