/* cs_seen: provides a seen command by tracking all users
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/


#include "module.h"

enum TypeInfo
{
	NEW, NICK_TO, NICK_FROM, JOIN, PART, QUIT, KICK
};

struct SeenInfo;
typedef Anope::insensitive_map<SeenInfo *> database_map;
database_map database;

struct SeenInfo : Serializable
{
	Anope::string nick;
	Anope::string vhost;
	TypeInfo type;
	Anope::string nick2;    // for nickchanges and kicks
	Anope::string channel;  // for join/part/kick
	Anope::string message;  // for part/kick/quit
	time_t last;            // the time when the user was last seen

	SeenInfo()
	{
	}

	const Anope::string serialize_name() const anope_override
	{
		return "SeenInfo";
	}

	Serialize::Data serialize() const anope_override
	{
		Serialize::Data data;

		data["nick"] << nick;
		data["vhost"] << vhost;
		data["type"] << type;
		data["nick2"] << nick2;
		data["channel"] << channel;
		data["message"] << message;
		data["last"].setType(Serialize::DT_INT) << last;

		return data;
	}

	static Serializable* unserialize(Serializable *obj, Serialize::Data &data)
	{
		SeenInfo *s;
		if (obj)
			s = anope_dynamic_static_cast<SeenInfo *>(obj);
		else
			s = new SeenInfo();

		data["nick"] >> s->nick;
		data["vhost"] >> s->vhost;
		unsigned int n;
		data["type"] >> n;
		s->type = static_cast<TypeInfo>(n);
		data["nick2"] >> s->nick2;
		data["channel"] >> s->channel;
		data["message"] >> s->message;
		data["last"] >> s->last;

		if (!s)
			database[s->nick] = s;
		return s;
	}
};

static time_t purgetime;
static time_t expiretimeout;

static SeenInfo *FindInfo(const Anope::string &nick)
{
	database_map::iterator iter = database.find(nick);
	if (iter != database.end())
		return iter->second;
	return NULL;
}

static bool ShouldHide(const Anope::string &channel, User *u)
{
	Channel *targetchan = findchan(channel);
	const ChannelInfo *targetchan_ci = targetchan ? *targetchan->ci : cs_findchan(channel);

	if (targetchan && targetchan->HasMode(CMODE_SECRET))
		return true;
	else if (targetchan_ci && targetchan_ci->HasFlag(CI_PRIVATE))
		return true;
	else if (u && u->HasMode(UMODE_PRIV))
		return true;
	return false;
}

class CommandOSSeen : public Command
{
 public:
	CommandOSSeen(Module *creator) : Command(creator, "operserv/seen", 1, 2)
	{
		this->SetDesc(_("Statistics and maintenance for seen data"));
		this->SetSyntax(_("\037STATS\037"));
		this->SetSyntax(_("\037CLEAR\037 \037time\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0].equals_ci("STATS"))
		{
			size_t mem_counter;
			mem_counter = sizeof(database_map);
			for (database_map::iterator it = database.begin(), it_end = database.end(); it != it_end; ++it)
			{
				mem_counter += (5 * sizeof(Anope::string)) + sizeof(TypeInfo) + sizeof(time_t);
				mem_counter += it->first.capacity();
				mem_counter += it->second->vhost.capacity();
				mem_counter += it->second->nick2.capacity();
				mem_counter += it->second->channel.capacity();
				mem_counter += it->second->message.capacity();
			}
			source.Reply(_("%lu nicks are stored in the database, using %.2Lf kB of memory"), database.size(), static_cast<long double>(mem_counter) / 1024);
		}
		else if (params[0].equals_ci("CLEAR"))
		{
			time_t time = 0;
			if ((params.size() < 2) || (0 >= (time = dotime(params[1]))))
			{
				this->OnSyntaxError(source, params[0]);
				return;
			}
			time = Anope::CurTime - time;
			database_map::iterator buf;
			size_t counter = 0;
			for (database_map::iterator it = database.begin(), it_end = database.end(); it != it_end;)
			{
				buf = it;
				++it;
				if (time < buf->second->last)
				{
					Log(LOG_DEBUG) << buf->first << " was last seen " << do_strftime(buf->second->last) << ", deleting entry";
					database.erase(buf);
					counter++;
				}
			}
			Log(LOG_ADMIN, source, this) << "CLEAR and removed " << counter << " nicks that were added after " << do_strftime(time, NULL, true);
			source.Reply(_("Database cleared, removed %lu nicks that were added after %s"), counter, do_strftime(time, source.nc, true).c_str());
		}
		else
			this->SendSyntax(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("The \002STATS\002 command prints out statistics about stored nicks and memory usage."));
		source.Reply(_("The \002CLEAR\002 command lets you clean the database by removing all entries from the\n"
				"entries from the database that were added within \037time\037.\n"
				" \n"
				"Example:\n"
				"%s CLEAR 30m\n"
				"will remove all entries that were added within the last 30 minutes."), source.command.c_str());
		return true;
	}
};

class CommandSeen : public Command
{
 public:
	CommandSeen(Module *creator) : Command(creator, "chanserv/seen", 1, 2)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Tells you about the last time a user was seen"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &target = params[0];

		if (target.length() > Config->NickLen)
		{
			source.Reply(_("Nick too long, max length is %u chars"), Config->NickLen);
			return;
		}

		if (findbot(target) != NULL)
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
			source.Reply(_("Sorry, I have not seen %s."), target.c_str());
			return;
		}

		User *u2 = finduser(target);
		Anope::string onlinestatus;
		if (u2)
			onlinestatus = ".";
		else
			onlinestatus = Anope::printf(_(" but %s mysteriously dematerialized."), target.c_str());

		Anope::string timebuf = duration(Anope::CurTime - info->last, source.nc);
		Anope::string timebuf2 = do_strftime(info->last, source.nc, true);

		if (info->type == NEW)
		{
			source.Reply(_("%s (%s) was last seen connecting %s ago (%s)%s"),
				target.c_str(), info->vhost.c_str(), timebuf.c_str(), timebuf2.c_str(), onlinestatus.c_str());
		}
		else if (info->type == NICK_TO)
		{
			u2 = finduser(info->nick2);
			if (u2)
				onlinestatus = Anope::printf( _(". %s is still online."), u2->nick.c_str());
			else
				onlinestatus = Anope::printf(_(", but %s mysteriously dematerialized"), info->nick2.c_str());

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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Checks for the last time \037nick\037 was seen joining, leaving,\n"
				"or changing nick on the network and tells you when and, depending\n"
				"on channel or user settings, where it was."));
		return true;
	}
};

class DataBasePurger : public CallBack
{
 public:
	DataBasePurger(Module *owner) : CallBack(owner, 300, Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		size_t previous_size = database.size();
		for (database_map::iterator it = database.begin(), it_end = database.end(); it != it_end;)
		{
			database_map::iterator cur = it;
			++it;

			if ((Anope::CurTime - cur->second->last) > purgetime)
			{
				Log(LOG_DEBUG) << cur->first << " was last seen " << do_strftime(cur->second->last) << ", purging entry";
				cur->second->destroy();
				database.erase(cur);
			}
		}
		Log(LOG_DEBUG) << "cs_seen: Purged Database, checked " << previous_size << " nicks and removed " << (previous_size - database.size()) << " old entries.";
	}
};

class CSSeen : public Module
{
	SerializeType seeninfo_type;
	CommandSeen commandseen;
	CommandOSSeen commandosseen;
	DataBasePurger purger;
 public:
	CSSeen(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), seeninfo_type("SeenInfo", SeenInfo::unserialize), commandseen(this), commandosseen(this), purger(this)
	{
		this->SetAuthor("Anope");

		Implementation eventlist[] =  { I_OnReload,
						I_OnUserConnect,
						I_OnUserNickChange,
						I_OnUserQuit,
						I_OnJoinChannel,
						I_OnPartChannel,
						I_OnUserKicked };
		ModuleManager::Attach(eventlist, this, sizeof(eventlist)/sizeof(Implementation));

		OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		purgetime = dotime(config.ReadValue("cs_seen", "purgetime", "30d", 0));
		expiretimeout = dotime(config.ReadValue("cs_seen", "expiretimeout", "1d", 0));

		if (purger.GetSecs() != expiretimeout)
			purger.SetSecs(expiretimeout);
	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt) anope_override
	{
		if (u)
			UpdateUser(u, NEW, u->nick, "", "", "");
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		UpdateUser(u, NICK_TO, oldnick, u->nick, "", "");
		UpdateUser(u, NICK_FROM, u->nick, oldnick, "", "");
	}

	void OnUserQuit(User *u, const Anope::string &msg) anope_override
	{
		UpdateUser(u, QUIT, u->nick, "", "", msg);
	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		UpdateUser(u, JOIN, u->nick, "", c->name, "");
	}

	void OnPartChannel(User *u, Channel *c, const Anope::string &channel, const Anope::string &msg) anope_override
	{
		UpdateUser(u, PART, u->nick, "", channel, msg);
	}

	void OnUserKicked(Channel *c, User *target, MessageSource &source, const Anope::string &msg) anope_override
	{
		UpdateUser(target, KICK, target->nick, source.GetSource(), c->name, msg);
	}

 private:
	void UpdateUser(const User *u, const TypeInfo Type, const Anope::string &nick, const Anope::string &nick2, const Anope::string &channel, const Anope::string &message)
	{
		SeenInfo *info = FindInfo(nick);
		if (!info)
		{
			info = new SeenInfo;
			database.insert(std::pair<Anope::string, SeenInfo *>(nick, info));
		}
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
