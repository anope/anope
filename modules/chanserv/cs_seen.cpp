// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

enum TypeInfo
{
	NEW, NICK_TO, NICK_FROM, JOIN, PART, QUIT, KICK
};

struct SeenInfo;
static SeenInfo *FindInfo(const Anope::string &nick);
typedef Anope::unordered_map<SeenInfo *> database_map;
database_map database;

struct SeenInfo final
	: Serializable
{
	Anope::string nick;
	Anope::string vhost;
	TypeInfo type;
	Anope::string nick2;    // for nickchanges and kicks
	Anope::string channel;  // for join/part/kick
	Anope::string message;  // for part/kick/quit
	time_t last;            // the time when the user was last seen

	SeenInfo() : Serializable("SeenInfo")
	{
	}

	~SeenInfo() override
	{
		auto iter = database.find(nick);
		if (iter != database.end() && iter->second == this)
			database.erase(iter);
	}
};

struct SeenInfoType final
	: Serialize::Type
{
	SeenInfoType()
		: Serialize::Type("SeenInfo")
	{
	}

	static Anope::string TypeToString(TypeInfo ti)
	{
		switch (ti)
		{
			case NEW:
				return "NEW";
			case NICK_TO:
				return "NICK_TO";
			case NICK_FROM:
				return "NICK_FROM";
			case JOIN:
				return "JOIN";
			case PART:
				return "PART";
			case QUIT:
				return "QUIT";
			case KICK:
				return "KICK";
		}
		return ""; // Should never happen.
	}

	static TypeInfo StringToType(const Anope::string &ti)
	{
		if (ti.equals_ci("NEW") || ti.equals_ci("0"))
			return NEW;
		if (ti.equals_ci("NICK_TO") || ti.equals_ci("1"))
			return NICK_TO;
		if (ti.equals_ci("NICK_FROM") || ti.equals_ci("2"))
			return NICK_FROM;
		if (ti.equals_ci("JOIN") || ti.equals_ci("3"))
			return JOIN;
		if (ti.equals_ci("PART") || ti.equals_ci("4"))
			return PART;
		if (ti.equals_ci("QUIT") || ti.equals_ci("5"))
			return QUIT;
		if (ti.equals_ci("KICK") || ti.equals_ci("6"))
			return KICK;

		return NEW; // Should never happen.
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *s = static_cast<const SeenInfo *>(obj);
		data.Store("nick", s->nick);
		data.Store("vhost", s->vhost);
		data.Store("type", TypeToString(s->type));
		data.Store("nick2", s->nick2);
		data.Store("channel", s->channel);
		data.Store("message", s->message);
		data.Store("last", s->last);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto snick = data.Load("nick");

		SeenInfo *s;
		if (obj)
			s = anope_dynamic_static_cast<SeenInfo *>(obj);
		else
		{
			SeenInfo *&info = database[snick];
			if (!info)
				info = new SeenInfo();
			s = info;
		}

		s->nick = snick;
		s->vhost = data.Load("vhost");
		s->type = StringToType(data.Load("type"));
		s->nick2 = data.Load("nick2");
		s->channel = data.Load("channel");
		s->message = data.Load("message");
		s->last = data.Load<time_t>("last");

		if (!obj)
			database[s->nick] = s;
		return s;
	}
};

static SeenInfo *FindInfo(const Anope::string &nick)
{
	auto iter = database.find(nick);
	if (iter != database.end())
		return iter->second;
	return NULL;
}

static bool ShouldHide(const Anope::string &channel, User *u)
{
	Channel *targetchan = Channel::Find(channel);
	const ChannelInfo *targetchan_ci = targetchan ? *targetchan->ci : ChannelInfo::Find(channel);

	if (targetchan && targetchan->HasMode("SECRET"))
		return true;
	else if (targetchan_ci && targetchan_ci->HasExt("CS_PRIVATE"))
		return true;
	else if (u && u->HasMode("PRIV"))
		return true;
	return false;
}

class CommandOSSeen final
	: public Command
{
public:
	CommandOSSeen(Module *creator) : Command(creator, "operserv/seen", 1, 2)
	{
		this->SetDesc(_("Statistics and maintenance for seen data"));
		this->SetSyntax("STATS");
		this->SetSyntax(_("CLEAR \037time\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (params[0].equals_ci("STATS"))
		{
			size_t mem_counter;
			mem_counter = sizeof(database_map);
			for (auto &[nick, si] : database)
			{
				mem_counter += (5 * sizeof(Anope::string)) + sizeof(TypeInfo) + sizeof(time_t);
				mem_counter += nick.capacity();
				mem_counter += si->vhost.capacity();
				mem_counter += si->nick2.capacity();
				mem_counter += si->channel.capacity();
				mem_counter += si->message.capacity();
			}
			source.Reply(_("%lu nicks are stored in the database, using %.2Lf kB of memory."), database.size(), static_cast<long double>(mem_counter) / 1024);
		}
		else if (params[0].equals_ci("CLEAR"))
		{
			time_t time = 0;
			if ((params.size() < 2) || (0 >= (time = Anope::DoTime(params[1]))))
			{
				this->OnSyntaxError(source, params[0]);
				return;
			}
			time = Anope::CurTime - time;
			database_map::iterator buf;
			size_t counter = 0;
			for (auto it = database.begin(), it_end = database.end(); it != it_end;)
			{
				buf = it;
				++it;
				if (time < buf->second->last)
				{
					Log(LOG_DEBUG) << buf->first << " was last seen " << Anope::strftime(buf->second->last) << ", deleting entry";
					delete buf->second;
					counter++;
				}
			}
			Log(LOG_ADMIN, source, this) << "CLEAR and removed " << counter << " nicks that were added after " << Anope::strftime(time, NULL, true);
			source.Reply(_("Database cleared, removed %lu nicks that were added after %s."), counter, Anope::strftime(time, source.nc, true).c_str());
		}
		else
			this->SendSyntax(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"The \002STATS\002 command prints out statistics about stored nicks and memory usage."
			"\n\n"
			"The \002CLEAR\002 command lets you clean the database by removing all entries from the "
			"database that were added within \037time\037."
		));

		ExampleWrapper examples;
		examples.AddEntry("CLEAR 30m", _(
			"Removes all entries that were added in the last 30 minutes."
		));
		return true;
	}
};

class CommandSeen final
	: public Command
{
public:
	CommandSeen(Module *creator) : Command(creator, "chanserv/seen", 1, 2)
	{
		this->SetDesc(_("Tells you about the last time a user was seen"));
		this->SetSyntax(_("\037nick\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &target = params[0];

		if (target.length() > IRCD->MaxNick)
		{
			source.Reply(_("Nick too long, max length is %zu characters."), IRCD->MaxNick);
			return;
		}

		if (BotInfo::Find(target, true) != NULL)
		{
			source.Reply(_("%s is a client on services."), target.c_str());
			return;
		}

		if (target.equals_ci(source.GetNick()))
		{
			source.Reply(_("You might see yourself in the mirror, %s."), source.GetNick().c_str());
			return;
		}

		SeenInfo *info = FindInfo(target);
		if (!info)
		{
			source.Reply(_("I have not seen %s."), target.c_str());
			return;
		}

		User *u2 = User::Find(target, true);
		Anope::string onlinestatus;
		if (u2)
			onlinestatus = ".";
		else
			onlinestatus = Anope::Format(Language::Translate(source.nc, _(" but %s mysteriously dematerialized.")), target.c_str());

		Anope::string timebuf = Anope::Duration(Anope::CurTime - info->last, source.nc);
		Anope::string timebuf2 = Anope::strftime(info->last, source.nc, true);

		if (info->type == NEW)
		{
			source.Reply(_("%s (%s) was last seen connecting %s ago (%s)%s"),
				target.c_str(), info->vhost.c_str(), timebuf.c_str(), timebuf2.c_str(), onlinestatus.c_str());
		}
		else if (info->type == NICK_TO)
		{
			u2 = User::Find(info->nick2, true);
			if (u2)
				onlinestatus = Anope::Format(Language::Translate(source.nc, _(". %s is still online.")), u2->nick.c_str());
			else
				onlinestatus = Anope::Format(Language::Translate(source.nc, _(", but %s mysteriously dematerialized.")), info->nick2.c_str());

			source.Reply(_("%s (%s) was last seen changing nick to %s %s ago%s"),
				target.c_str(), info->vhost.c_str(), info->nick2.c_str(), timebuf.c_str(), onlinestatus.c_str());
		}
		else if (info->type == NICK_FROM)
		{
			source.Reply(_("%s (%s) was last seen changing nick from %s to %s %s ago%s"),
				target.c_str(), info->vhost.c_str(), info->nick2.c_str(), target.c_str(), timebuf.c_str(), onlinestatus.c_str());
		}
		else if (info->type == JOIN)
		{
			if (ShouldHide(info->channel, u2))
				source.Reply(_("%s (%s) was last seen joining a secret channel %s ago%s"),
					target.c_str(), info->vhost.c_str(), timebuf.c_str(), onlinestatus.c_str());
			else
				source.Reply(_("%s (%s) was last seen joining %s %s ago%s"),
					target.c_str(), info->vhost.c_str(), info->channel.c_str(), timebuf.c_str(), onlinestatus.c_str());
		}
		else if (info->type == PART)
		{
			if (ShouldHide(info->channel, u2))
				source.Reply(_("%s (%s) was last seen parting a secret channel %s ago%s"),
					target.c_str(), info->vhost.c_str(), timebuf.c_str(), onlinestatus.c_str());
			else
				source.Reply(_("%s (%s) was last seen parting %s %s ago%s"),
					target.c_str(), info->vhost.c_str(), info->channel.c_str(), timebuf.c_str(), onlinestatus.c_str());
		}
		else if (info->type == QUIT)
		{
			source.Reply(_("%s (%s) was last seen quitting (%s) %s ago (%s)."),
					target.c_str(), info->vhost.c_str(), info->message.c_str(), timebuf.c_str(), timebuf2.c_str());
		}
		else if (info->type == KICK)
		{
			if (ShouldHide(info->channel, u2))
				source.Reply(_("%s (%s) was kicked from a secret channel %s ago%s"),
					target.c_str(), info->vhost.c_str(), timebuf.c_str(), onlinestatus.c_str());
			else
				source.Reply(_("%s (%s) was kicked from %s (\"%s\") %s ago%s"),
					target.c_str(), info->vhost.c_str(), info->channel.c_str(), info->message.c_str(), timebuf.c_str(), onlinestatus.c_str());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Checks for the last time \037nick\037 was seen joining, leaving, "
			"or changing nick on the network and tells you when and, depending "
			"on channel or user settings, where it was."
		));
		return true;
	}
};

class CSSeen final
	: public Module
{
	SeenInfoType seeninfo_type;
	CommandSeen commandseen;
	CommandOSSeen commandosseen;
public:
	CSSeen(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandseen(this)
		, commandosseen(this)
	{
	}

	void OnExpireTick() override
	{
		auto purgetime = Config->GetModule(this).Get<time_t>("purgetime", "90d");
		if (!purgetime)
			return;

		auto previous_size = database.size();
		for (auto it = database.begin(), it_end = database.end(); it != it_end;)
		{
			auto cur = it;
			++it;

			if ((Anope::CurTime - cur->second->last) > purgetime)
			{
				Log(LOG_DEBUG) << cur->first << " was last seen " << Anope::strftime(cur->second->last) << ", purging entries";
				delete cur->second;
			}
		}
		Log(LOG_DEBUG) << "cs_seen: Purged database, checked " << previous_size << " nicks and removed " << (previous_size - database.size()) << " old entries.";
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (!u->Quitting())
			UpdateUser(u, NEW, u->nick, "", "", "");
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) override
	{
		UpdateUser(u, NICK_TO, oldnick, u->nick, "", "");
		UpdateUser(u, NICK_FROM, u->nick, oldnick, "", "");
	}

	void OnUserQuit(User *u, const Anope::string &msg) override
	{
		UpdateUser(u, QUIT, u->nick, "", "", msg);
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		UpdateUser(u, JOIN, u->nick, "", c->name, "");
	}

	void OnPartChannel(User *u, Channel *c, const Anope::string &channel, const Anope::string &msg) override
	{
		UpdateUser(u, PART, u->nick, "", channel, msg);
	}

	void OnPreUserKicked(const MessageSource &source, Membership *memb, const Anope::string &msg) override
	{
		UpdateUser(memb->user, KICK, memb->user->nick, source.GetSource(), memb->chan->name, msg);
	}

private:
	static void UpdateUser(const User *u, const TypeInfo Type, const Anope::string &nick, const Anope::string &nick2, const Anope::string &channel, const Anope::string &message)
	{
		if (!u->server->IsSynced())
			return;

		SeenInfo *&info = database[nick];
		if (!info)
			info = new SeenInfo();
		info->nick = nick;
		info->vhost = u->GetVIdent() + "@" + u->GetDisplayedHost();
		info->type = Type;
		info->last = Anope::CurTime;
		info->nick2 = nick2;
		info->channel = channel;
		info->message = message;
	}
};

MODULE_INIT(CSSeen)
