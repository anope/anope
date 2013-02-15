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

class CommandCSClone : public Command
{
public:
	CommandCSClone(Module *creator) : Command(creator, "chanserv/clone", 2, 3)
	{
		this->SetDesc(_("Copy all settings from one channel to another"));
		this->SetSyntax(_("\037channel\037 \037target\037 [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &channel = params[0];
		const Anope::string &target = params[1];
		Anope::string what = params.size() > 2 ? params[2] : "";

		User *u = source.GetUser();
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		ChannelInfo *target_ci = ChannelInfo::Find(target);
		if (!target_ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, target.c_str());
			return;
		}
		if (!source.IsFounder(ci) || !source.IsFounder(target_ci))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (what.equals_ci("ALL"))
			what.clear();

		if (what.empty())
		{
			delete target_ci;
			target_ci = new ChannelInfo(*ci);
			target_ci->name = target;
			(*RegisteredChannelList)[target_ci->name] = target_ci;
			target_ci->c = Channel::Find(target_ci->name);
			if (target_ci->c)
			{
				target_ci->c->ci = target_ci;

				target_ci->c->CheckModes();

				ChannelMode *cm;
				if (u && u->FindChannel(target_ci->c) != NULL)
				{
					/* On most ircds you do not receive the admin/owner mode till its registered */
					if ((cm = ModeManager::FindChannelModeByName("OWNER")))
						target_ci->c->SetMode(NULL, cm, u->GetUID());
					else if ((cm = ModeManager::FindChannelModeByName("PROTECT")))
						target_ci->c->RemoveMode(NULL, cm, u->GetUID());
				}

				/* Mark the channel as persistent */
				if (target_ci->c->HasMode("PERM"))
					target_ci->ExtendMetadata("PERSIST");
				/* Persist may be in def cflags, set it here */
				else if (target_ci->HasExt("PERSIST") && (cm = ModeManager::FindChannelModeByName("PERM")))
					target_ci->c->SetMode(NULL, cm);
	
				if (target_ci->bi && target_ci->c->FindUser(target_ci->bi) == NULL)
					target_ci->bi->Join(target_ci->c, &ModeManager::DefaultBotModes);
			}

			if (target_ci->c && !target_ci->c->topic.empty())
			{
				target_ci->last_topic = target_ci->c->topic;
				target_ci->last_topic_setter = target_ci->c->topic_setter;
				target_ci->last_topic_time = target_ci->c->topic_time;
			}
			else
				target_ci->last_topic_setter = source.service->nick;

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(target_ci));

			source.Reply(_("All settings from \002%s\002 have been cloned to \002%s\002."), channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("ACCESS"))
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				const ChanAccess *taccess = ci->GetAccess(i);
				AccessProvider *provider = taccess->provider;

				ChanAccess *newaccess = provider->Create();
				newaccess->ci = target_ci;
				newaccess->mask = taccess->mask;
				newaccess->creator = taccess->creator;
				newaccess->last_seen = taccess->last_seen;
				newaccess->created = taccess->created;
				newaccess->AccessUnserialize(taccess->AccessSerialize());

				target_ci->AddAccess(newaccess);
			}

			source.Reply(_("All access entries from \002%s\002 have been cloned to \002%s\002."), channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("AKICK"))
		{
			target_ci->ClearAkick();
			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				const AutoKick *akick = ci->GetAkick(i);
				if (akick->nc)
					target_ci->AddAkick(akick->creator, akick->nc, akick->reason, akick->addtime, akick->last_used);
				else
					target_ci->AddAkick(akick->creator, akick->mask, akick->reason, akick->addtime, akick->last_used);
			}

			source.Reply(_("All akick entries from \002%s\002 have been cloned to \002%s\002."), channel.c_str(), target.c_str());
		}
		else if (what.equals_ci("BADWORDS"))
		{
			target_ci->ClearBadWords();
			for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
			{
				const BadWord *bw = ci->GetBadWord(i);
				target_ci->AddBadWord(bw->word, bw->type);
			}

			source.Reply(_("All badword entries from \002%s\002 have been cloned to \002%s\002."), channel.c_str(), target.c_str());
		}
		else
		{
			this->OnSyntaxError(source, "");
			return;
		}

		Log(LOG_COMMAND, source, this, ci) << "to clone " << (what.empty() ? "everything from it" : what) << " to " << target_ci->name;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Copies all settings, access, akicks, etc from \002channel\002 to the\n"
				"\002target\002 channel. If \037what\037 is \002ACCESS\002, \002AKICK\002, or \002BADWORDS\002\n"
				"then only the respective settings are cloned.\n"
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

	}
};

MODULE_INIT(CSClone)
