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
#include "modules/bs_badwords.h"
#include "modules/cs_akick.h"

class CommandCSClone : public Command
{
public:
	CommandCSClone(Module *creator) : Command(creator, "chanserv/clone", 2, 3)
	{
		this->SetDesc(_("Copy all settings from one channel to another"));
		this->SetSyntax(_("\037channel\037 \037target\037 [\037what\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &channel = params[0];
		const Anope::string &target = params[1];
		Anope::string what = params.size() > 2 ? params[2] : "";

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		User *u = source.GetUser();
		ChanServ::Channel *ci = ChanServ::Find(channel);
		bool override = false;

		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
			return;
		}

		ChanServ::Channel *target_ci = ChanServ::Find(target);
		if (!target_ci)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), target);
			return;
		}

		if (ci == target_ci)
		{
			source.Reply(_("Cannot clone channel \002{0}\002 to itself!"), ci->name);
			return;
		}

		if (!source.IsFounder(ci) || !source.IsFounder(target_ci))
		{
			if (!source.HasPriv("chanserv/administration"))
			{
				source.Reply(_("Access denied. You do not have the privilege \002{0}\002 on \002{1}\002 and \002{2}\002."), "FOUNDER", ci->name, target_ci->name);
				return;
			}
			else
				override = true;
		}

		if (what.equals_ci("ALL"))
			what.clear();

		if (what.empty())
		{
			delete target_ci;
			target_ci = ChanServ::service->Create(*ci);
			target_ci->name = target;
			ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
			map[target_ci->name] = target_ci;
			target_ci->c = Channel::Find(target_ci->name);

			target_ci->bi = NULL;
			if (ci->bi)
				ci->bi->Assign(u, target_ci);

			if (target_ci->c)
			{
				target_ci->c->ci = target_ci;

				target_ci->c->CheckModes();

				target_ci->c->SetCorrectModes(u, true);
			}

			if (target_ci->c && !target_ci->c->topic.empty())
			{
				target_ci->last_topic = target_ci->c->topic;
				target_ci->last_topic_setter = target_ci->c->topic_setter;
				target_ci->last_topic_time = target_ci->c->topic_time;
			}
			else
				target_ci->last_topic_setter = source.service->nick;

			Event::OnChanRegistered(&Event::ChanRegistered::OnChanRegistered, target_ci);

			source.Reply(_("All settings from \002{0}\002 have been cloned to \002{0}\002."), channel, target);
		}
		else if (what.equals_ci("ACCESS"))
		{
			std::set<Anope::string> masks;
			unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
			unsigned count = 0;

			for (unsigned i = 0; i < target_ci->GetAccessCount(); ++i)
				masks.insert(target_ci->GetAccess(i)->Mask());

			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				const ChanServ::ChanAccess *taccess = ci->GetAccess(i);
				ChanServ::AccessProvider *provider = taccess->provider;

				if (access_max && target_ci->GetDeepAccessCount() >= access_max)
					break;

				if (masks.count(taccess->Mask()))
					continue;
				masks.insert(taccess->Mask());

				ChanServ::ChanAccess *newaccess = provider->Create();
				newaccess->SetMask(taccess->Mask(), target_ci);
				newaccess->creator = taccess->creator;
				newaccess->last_seen = taccess->last_seen;
				newaccess->created = taccess->created;
				newaccess->AccessUnserialize(taccess->AccessSerialize());

				target_ci->AddAccess(newaccess);

				++count;
			}

			source.Reply(_("\002{0}\002 access entries from \002{1}\002 have been cloned to \002{2}\002."), count, channel, target);
		}
		else if (what.equals_ci("AKICK"))
		{
			target_ci->ClearAkick();
			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				const AutoKick *ak = ci->GetAkick(i);
				if (ak->nc)
					target_ci->AddAkick(ak->creator, ak->nc, ak->reason, ak->addtime, ak->last_used);
				else
					target_ci->AddAkick(ak->creator, ak->mask, ak->reason, ak->addtime, ak->last_used);
			}

			source.Reply(_("All auto kick entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target);
		}
		else if (what.equals_ci("BADWORDS"))
		{
			BadWords *target_badwords = target_ci->Require<BadWords>("badwords"),
				*badwords = ci->Require<BadWords>("badwords");

			if (!target_badwords || !badwords)
			{
				source.Reply(_("All badword entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target); // BotServ doesn't exist/badwords isn't loaded
				return;
			}

			target_badwords->ClearBadWords();

			for (unsigned i = 0; i < badwords->GetBadWordCount(); ++i)
			{
				const BadWord *bw = badwords->GetBadWord(i);
				target_badwords->AddBadWord(bw->word, bw->type);
			}

			badwords->Check();
			target_badwords->Check();

			source.Reply(_("All badword entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target);
		}
		else
		{
			this->OnSyntaxError(source, "");
			return;
		}

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clone " << (what.empty() ? "everything from it" : what) << " to " << target_ci->name;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Copies all settings, access, akicks, etc. from \037channel\037 to the \037target\037 channel."
		               " If \037what\037 is \002ACCESS\002, \002AKICK\002, or \002BADWORDS\002 then only the respective settings are cloned.\n"
		               "\n"
		               "You must be the founder of \037channel\037 and \037target\037."));
		return true;
	}
};

class CSClone : public Module
{
	CommandCSClone commandcsclone;

 public:
	CSClone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsclone(this)
	{

	}
};

MODULE_INIT(CSClone)
