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

class CommandCSClone : public Command
{
public:
	CommandCSClone(Module *creator) : Command(creator, "chanserv/clone", 2, 3)
	{
		this->SetDesc(_("Copy all settings from one channel to another"));
		this->SetSyntax(_("\037channel\037 \037target\037 [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &channel = params[0];
		const Anope::string &target = params[1];
		Anope::string what = params.size() > 2 ? params[2] : "";

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!ci->AccessFor(u).HasPriv(CA_SET))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}
		ChannelInfo *target_ci = cs_findchan(target);
		if (!target_ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, target.c_str());
			return;
		}
		if (!IsFounder(u, ci) || !IsFounder(u, target_ci))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Config->CSMaxReg && u->Account()->channelcount >= Config->CSMaxReg && !u->HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(u->Account()->channelcount > Config->CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : _(CHAN_REACHED_CHANNEL_LIMIT), Config->CSMaxReg);
			return;
		}

		if (what.equals_ci("ALL"))
			what.clear();

		if (what.empty())
		{
			delete target_ci;
			target_ci = new ChannelInfo(*ci);
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

				/* Mark the channel as persistent */
				if (target_ci->c->HasMode(CMODE_PERM))
					target_ci->SetFlag(CI_PERSIST);
				/* Persist may be in def cflags, set it here */
				else if (target_ci->HasFlag(CI_PERSIST) && (cm = ModeManager::FindChannelModeByName(CMODE_PERM)))
					target_ci->c->SetMode(NULL, CMODE_PERM);
	
				if (target_ci->bi && target_ci->c->FindUser(target_ci->bi) == NULL)
					target_ci->bi->Join(target_ci->c, &Config->BotModeList);
			}

			if (target_ci->c && !target_ci->c->topic.empty())
			{
				target_ci->last_topic = target_ci->c->topic;
				target_ci->last_topic_setter = target_ci->c->topic_setter;
				target_ci->last_topic_time = target_ci->c->topic_time;
			}
			else
				target_ci->last_topic_setter = source.owner->nick;

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(target_ci));

			source.Reply(_("All settings from \002%s\002 have been transferred to \002%s\002"), channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("ACCESS"))
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanAccess *taccess = ci->GetAccess(i);
				AccessProvider *provider = taccess->provider;

				ChanAccess *newaccess = provider->Create();
				newaccess->ci = target_ci;
				newaccess->mask = taccess->mask;
				newaccess->creator = taccess->creator;
				newaccess->last_seen = taccess->last_seen;
				newaccess->created = taccess->created;
				newaccess->Unserialize(taccess->Serialize());

				target_ci->AddAccess(newaccess);
			}

			source.Reply(_("All access entries from \002%s\002 have been transferred to \002%s\002"), channel.c_str(), target.c_str());
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

			source.Reply(_("All akick entries from \002%s\002 have been transferred to \002%s\002"), channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("BADWORDS"))
		{
			target_ci->ClearBadWords();
			for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
			{
				BadWord *bw = ci->GetBadWord(i);
				target_ci->AddBadWord(bw->word, bw->type);
			}

			source.Reply(_("All badword entries from \002%s\002 have been transferred to \002%s\002"), channel.c_str(), target.c_str());
		}
		else
		{
			this->OnSyntaxError(source, "");
			return;
		}

		Log(LOG_COMMAND, u, this, ci) << "to clone it to " << target_ci->name;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Copies all settings, access, akicks, etc from channel to the\n"
				"target channel. If \037what\037 is access, akick, or badwords\n"
				"then only the respective settings are transferred.\n"
				"You must be the founder of \037channel\037 and \037target\037."));
		return true;
	}
};

class CSClone : public Module
{
	CommandCSClone commandcsclone;

 public:
	CSClone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcsclone(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcsclone);
	}
};

MODULE_INIT(CSClone)
