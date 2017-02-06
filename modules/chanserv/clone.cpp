/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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
#include "modules/botserv/badwords.h"
#include "modules/chanserv/akick.h"

class CommandCSClone : public Command
{
	ServiceReference<BadWords> badwords;

#warning "levels hasnt been merged"
#if 0
	void CopyLevels(CommandSource &source, ChannelInfo *ci, ChannelInfo *target_ci)
	{
		const Anope::map<int16_t> &cilevels = ci->GetLevelEntries();

		for (Anope::map<int16_t>::const_iterator it = cilevels.begin(); it != cilevels.end(); ++it)
		{
			target_ci->SetLevel(it->first, it->second);
		}

		source.Reply(_("All level entries from \002%s\002 have been cloned into \002%s\002."), ci->name.c_str(), target_ci->name.c_str());
	}
#endif

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
			source.Reply(_("Cannot clone channel \002{0}\002 to itself!"), ci->GetName());
			return;
		}

		if (!source.IsFounder(ci) || !source.IsFounder(target_ci))
		{
			if (!source.HasOverridePriv("chanserv/administration"))
			{
				source.Reply(_("Access denied. You do not have the privilege \002{0}\002 on \002{1}\002 and \002{2}\002."), "FOUNDER", ci->GetName(), target_ci->GetName());
				return;
			}
		}

		if (what.equals_ci("ALL"))
			what.clear();

		if (what.empty())
		{
			target_ci->Delete();
			target_ci = Serialize::New<ChanServ::Channel *>();
			target_ci->SetName(target);
			target_ci->SetTimeRegistered(Anope::CurTime);
			ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
			map[target_ci->GetName()] = target_ci;
			target_ci->c = Channel::Find(target_ci->GetName());

			if (ci->GetBot())
				ci->GetBot()->Assign(u, target_ci);
			else
				target_ci->SetBot(nullptr);

			if (target_ci->c)
			{
				target_ci->c->ci = target_ci;

				target_ci->c->CheckModes();

				target_ci->c->SetCorrectModes(u, true);
			}

			if (target_ci->c && !target_ci->c->topic.empty())
			{
				target_ci->SetLastTopic(target_ci->GetLastTopic());
				target_ci->SetLastTopicSetter(target_ci->c->topic_setter);
				target_ci->SetLastTopicTime(target_ci->c->topic_time);
			}
			else
			{
				target_ci->SetLastTopicSetter(source.service->nick);
			}

			EventManager::Get()->Dispatch(&Event::ChanRegistered::OnChanRegistered, target_ci);

			source.Reply(_("All settings from \002{0}\002 have been cloned to \002{0}\002."), channel, target);
		}
		else if (what.equals_ci("ACCESS"))
		{
			std::set<Anope::string> masks;
			unsigned access_max = Config->GetModule("chanserv/main")->Get<unsigned>("accessmax", "1024");
			unsigned count = 0;

			for (unsigned i = 0; i < target_ci->GetAccessCount(); ++i)
				masks.insert(target_ci->GetAccess(i)->Mask());

			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanServ::ChanAccess *taccess = ci->GetAccess(i);

				if (access_max && target_ci->GetAccessCount() >= access_max)
					break;

				if (masks.count(taccess->Mask()))
					continue;
				masks.insert(taccess->Mask());

				ChanServ::ChanAccess *newaccess = anope_dynamic_static_cast<ChanServ::ChanAccess *>(taccess->GetSerializableType()->Create());
				newaccess->SetChannel(taccess->GetChannel());
				newaccess->SetMask(taccess->GetMask());
				newaccess->SetMask(taccess->GetMask());
				newaccess->SetCreator(taccess->GetCreator());
				newaccess->SetLastSeen(taccess->GetLastSeen());
				newaccess->SetCreated(taccess->GetCreated());
				newaccess->AccessUnserialize(taccess->AccessSerialize());

				++count;
			}

			source.Reply(_("\002{0}\002 access entries from \002{1}\002 have been cloned to \002{2}\002."), count, channel, target);
		}
		else if (what.equals_ci("AKICK"))
		{
			target_ci->ClearAkick();
			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				AutoKick *ak = ci->GetAkick(i);
				if (ak->GetAccount())
					target_ci->AddAkick(ak->GetCreator(), ak->GetAccount(), ak->GetReason(), ak->GetAddTime(), ak->GetLastUsed());
				else
					target_ci->AddAkick(ak->GetCreator(), ak->GetMask(), ak->GetReason(), ak->GetAddTime(), ak->GetLastUsed());
			}

			source.Reply(_("All auto kick entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target);
		}
		else if (what.equals_ci("BADWORDS"))
		{
			if (!badwords)
			{
				source.Reply(_("All badword entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target); // BotServ doesn't exist/badwords isn't loaded
				return;
			}

			badwords->ClearBadWords(target_ci);

			for (unsigned i = 0; i < badwords->GetBadWordCount(ci); ++i)
			{
				BadWord *bw = badwords->GetBadWord(ci, i);
				badwords->AddBadWord(target_ci, bw->GetWord(), bw->GetType());
			}

			source.Reply(_("All badword entries from \002{0}\002 have been cloned to \002{1}\002."), channel, target);
		}
		else
		{
			this->OnSyntaxError(source, "");
			return;
		}

		logger.Command(source, ci, _("{source} used {command} on {channel} to clone {0} to {1}"),
				what.empty() ? "everything from it" : what, target_ci->GetName());
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
