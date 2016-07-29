/* OperServ core functions
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
#include "modules/operserv/session.h"
#include "modules/operserv/stats.h"

class StatsImpl : public Stats
{
	friend class StatsType;

	unsigned int maxusercnt = 0;
	time_t maxusertime = 0;

 public:

	using Stats::Stats;

	unsigned int GetMaxUserCount();
	void SetMaxUserCount(unsigned int i);

	time_t GetMaxUserTime();
	void SetMaxUserTime(time_t t);
};

class StatsType : public Serialize::Type<StatsImpl>
{
 public:
	Serialize::Field<StatsImpl, unsigned int> maxusercount;
	Serialize::Field<StatsImpl, time_t> maxusertime;

	StatsType(Module *me) : Serialize::Type<StatsImpl>(me)
		, maxusercount(this, "maxusercount", &StatsImpl::maxusercnt)
		, maxusertime(this, "maxusertime", &StatsImpl::maxusertime)
	{
	}
};

unsigned int StatsImpl::GetMaxUserCount()
{
	return Get(&StatsType::maxusercount);
}

void StatsImpl::SetMaxUserCount(unsigned int i)
{
	Set(&StatsType::maxusercount, i);
}

time_t StatsImpl::GetMaxUserTime()
{
	return Get(&StatsType::maxusertime);
}

void StatsImpl::SetMaxUserTime(time_t t)
{
	Set(&StatsType::maxusertime, t);
}

/**
 * Count servers connected to server s
 * @param s The server to start counting from
 * @return Amount of servers connected to server s
 **/
static int stats_count_servers(Server *s)
{
	if (!s)
		return 0;

	int count = 1;

	if (!s->GetLinks().empty())
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
			count += stats_count_servers(s->GetLinks()[i]);

	return count;
}

class CommandOSStats : public Command
{
	ServiceReference<XLineManager> akills, snlines, sqlines;
	ServiceReference<SessionService> session_service;

 private:
	void DoStatsAkill(CommandSource &source)
	{
		int timeout;
		if (akills)
		{
			/* AKILLs */
			source.Reply(_("Current number of AKILLs: \002{0}\002"), akills->GetCount());
			timeout = Config->GetModule("operserv")->Get<time_t>("autokillexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default AKILL expiry time: \002{0} days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default AKILL expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default AKILL expiry time: \002{0} hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default AKILL expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default AKILL expiry time: \002{0} minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default AKILL expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default AKILL expiry time: \002No expiration\002"));
		}
		if (snlines)
		{
			/* SNLINEs */
			source.Reply(_("Current number of SNLINEs: \002{0}\002"), snlines->GetCount());
			timeout = Config->GetModule("operserv")->Get<time_t>("snlineexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default SNLINE expiry time: \002{0} days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default SNLINE expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default SNLINE expiry time: \002{0} hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default SNLINE expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default SNLINE expiry time: \002{0} minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default SNLINE expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default SNLINE expiry time: \002No expiration\002"));
		}
		if (sqlines)
		{
			/* SQLINEs */
			source.Reply(_("Current number of SQLINEs: \002{0}\002"), sqlines->GetCount());
			timeout = Config->GetModule("operserv")->Get<time_t>("sglineexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default SQLINE expiry time: \002{0} days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default SQLINE expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default SQLINE expiry time: \002{0} hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default SQLINE expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default SQLINE expiry time: \002{0} minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default SQLINE expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default SQLINE expiry time: \002No expiration\002"));
		}
	}

	void DoStatsReset(CommandSource &source)
	{
		Stats *stats = Serialize::GetObject<Stats *>();
		stats->SetMaxUserCount(UserListByNick.size());
		source.Reply(_("Statistics reset."));
		return;
	}

	void DoStatsUptime(CommandSource &source)
	{
		Stats *stats = Serialize::GetObject<Stats *>();
		time_t uptime = Anope::CurTime - Anope::StartTime;
		
		source.Reply(_("Current users: \002{0}\002 (\002{1}\002 ops)"), UserListByNick.size(), OperCount);
		source.Reply(_("Maximum users: \002{0}\002 ({1})"), stats->GetMaxUserCount(), Anope::strftime(stats->GetMaxUserTime(), source.GetAccount()));
		source.Reply(_("Services up \002{0}\002."), Anope::Duration(uptime, source.GetAccount()));
	}

	void DoStatsUplink(CommandSource &source)
	{
		Anope::string buf;
		for (std::set<Anope::string>::iterator it = Servers::Capab.begin(); it != Servers::Capab.end(); ++it)
			buf += " " + *it;
		if (!buf.empty())
			buf.erase(buf.begin());

		source.Reply(_("Uplink server: \002{0}\002"), Me->GetLinks().front()->GetName());
		source.Reply(_("Uplink capab: {0}"), buf);
		source.Reply(_("Servers found: {0}"), stats_count_servers(Me->GetLinks().front()));
	}

	template<typename T> void GetHashStats(const T& map, size_t& entries, size_t& buckets, size_t& max_chain)
	{
		entries = map.size(), buckets = map.bucket_count(), max_chain = 0;
		for (size_t i = 0; i < buckets; ++i)
			if (map.bucket_size(i) > max_chain)
				max_chain = map.bucket_size(i);
	}

	void DoStatsHash(CommandSource &source)
	{
		size_t entries, buckets, max_chain;

		GetHashStats(UserListByNick, entries, buckets, max_chain);
		source.Reply(_("Users (nick): {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);

		if (!UserListByUID.empty())
		{
			GetHashStats(UserListByUID, entries, buckets, max_chain);
			source.Reply(_("Users (uid): {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);
		}

		GetHashStats(ChannelList, entries, buckets, max_chain);
		source.Reply(_("Channels: {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);

		GetHashStats(ChanServ::service->GetChannels(), entries, buckets, max_chain);
		source.Reply(_("Registered channels: {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);

		GetHashStats(NickServ::service->GetNickMap(), entries, buckets, max_chain);
		source.Reply(_("Registered nicknames: {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);

		GetHashStats(NickServ::service->GetAccountMap(), entries, buckets, max_chain);
		source.Reply(_("Registered nick groups: {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);

		if (session_service)
		{
			GetHashStats(session_service->GetSessions(), entries, buckets, max_chain);
			source.Reply(_("Sessions: {0} entries, {1} buckets, longest chain is {2}"), entries, buckets, max_chain);
		}
	}

 public:
	CommandOSStats(Module *creator) : Command(creator, "operserv/stats", 0, 1)
		, akills("sgline")
		, snlines("snline")
		, sqlines("sqline")
	{
		this->SetDesc(_("Show status of Services and network"));
		this->SetSyntax("[AKILL | HASH | UPLINK | UPTIME | ALL | RESET]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string extra = !params.empty() ? params[0] : "";

		Log(LOG_ADMIN, source, this) << extra;

		if (extra.equals_ci("RESET"))
			return this->DoStatsReset(source);

		if (extra.equals_ci("ALL") || extra.equals_ci("AKILL"))
			this->DoStatsAkill(source);

		if (extra.equals_ci("ALL") || extra.equals_ci("HASH"))
			this->DoStatsHash(source);

		if (extra.equals_ci("ALL") || extra.equals_ci("UPLINK"))
			this->DoStatsUplink(source);

		if (extra.empty() || extra.equals_ci("ALL") || extra.equals_ci("UPTIME"))
			this->DoStatsUptime(source);

		if (!extra.empty() && !extra.equals_ci("ALL") && !extra.equals_ci("AKILL") && !extra.equals_ci("HASH") && !extra.equals_ci("UPLINK") && !extra.equals_ci("UPTIME"))
			source.Reply(_("Unknown STATS option: \002{0}\002"), extra);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Without any option, shows the current number of users online, and the highest number of users online since Services was started, and the length of time Services has been running.\n"
		               "\n"
		               "With the \002AKILL\002 option, displays the current size of the AKILL list and the current default expiry time.\n"
		               "\n"
		               "The \002RESET\002 option currently resets the maximum user count to the number of users currently present on the network.\n"
		               "\n"
		               "The \002UPLINK\002 option displays information about the current server Anope uses as an uplink to the network.\n"
		               "\n"
		               "The \002HASH\002 option displays information about the hash maps.\n"
		               "\n"
		               "The \002ALL\002 option displays all of the above statistics."));
		return true;
	}
};

class OSStats : public Module
	, public EventHook<Event::UserConnect>
{
	CommandOSStats commandosstats;
	StatsType stats_type;

	Stats *GetStats()
	{
		Stats *stats = Serialize::GetObject<Stats *>();
		if (stats)
			return stats;
		else
			return Serialize::New<Stats *>();
	}

 public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserConnect>(this)
		, commandosstats(this)
		, stats_type(this)
	{
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		Stats *stats = GetStats();

		if (stats && UserListByNick.size() > stats->GetMaxUserCount())
		{
			stats->SetMaxUserCount(UserListByNick.size());
			stats->SetMaxUserTime(Anope::CurTime);

			Server *sserver = u->server;
			if (sserver && sserver->IsSynced())
				Log(this, "maxusers") << "connected - new maximum user count: " << UserListByNick.size();
		}
	}
};

MODULE_INIT(OSStats)
