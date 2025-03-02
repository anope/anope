/* OperServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/os_session.h"

struct Stats final
	: Serializable
{
	static Stats *me;

	Stats() : Serializable("Stats")
	{
		me = this;
	}

	void Serialize(Serialize::Data &data) const override
	{
		data.Store("maxusercnt", MaxUserCount);
		data.Store("maxusertime", MaxUserTime);
	}

	static Serializable *Unserialize(Serializable *obj, Serialize::Data &data)
	{
		data["maxusercnt"] >> MaxUserCount;
		data["maxusertime"] >> MaxUserTime;
		return me;
	}
};

Stats *Stats::me;

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
	{
		for (auto *link : s->GetLinks())
			count += stats_count_servers(link);
	}

	return count;
}

class CommandOSStats final
	: public Command
{
	ServiceReference<XLineManager> akills, snlines, sqlines;
private:
	void DoStatsAkill(CommandSource &source)
	{
		int timeout;
		if (akills)
		{
			/* AKILLs */
			source.Reply(_("Current number of AKILLs: \002%zu\002"), akills->GetCount());
			timeout = Config->GetModule("operserv").Get<time_t>("autokillexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default AKILL expiry time: \002%d days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default AKILL expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default AKILL expiry time: \002%d hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default AKILL expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default AKILL expiry time: \002%d minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default AKILL expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default AKILL expiry time: \002No expiration\002"));
		}
		if (snlines)
		{
			/* SNLINEs */
			source.Reply(_("Current number of SNLINEs: \002%zu\002"), snlines->GetCount());
			timeout = Config->GetModule("operserv").Get<time_t>("snlineexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default SNLINE expiry time: \002%d days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default SNLINE expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default SNLINE expiry time: \002%d hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default SNLINE expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default SNLINE expiry time: \002%d minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default SNLINE expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default SNLINE expiry time: \002No expiration\002"));
		}
		if (sqlines)
		{
			/* SQLINEs */
			source.Reply(_("Current number of SQLINEs: \002%zu\002"), sqlines->GetCount());
			timeout = Config->GetModule("operserv").Get<time_t>("sglineexpiry", "30d") + 59;
			if (timeout >= 172800)
				source.Reply(_("Default SQLINE expiry time: \002%d days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default SQLINE expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default SQLINE expiry time: \002%d hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default SQLINE expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default SQLINE expiry time: \002%d minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default SQLINE expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default SQLINE expiry time: \002No expiration\002"));
		}
	}

	static void DoStatsReset(CommandSource &source)
	{
		MaxUserCount = UserListByNick.size();
		MaxUserTime = Anope::CurTime;
		Stats::me->QueueUpdate();
		source.Reply(_("Statistics reset."));
		return;
	}

	static void DoStatsUptime(CommandSource &source)
	{
		time_t uptime = Anope::CurTime - Anope::StartTime;
		source.Reply(_("Current users: \002%zu\002 (\002%d\002 ops)"), UserListByNick.size(), OperCount);
		source.Reply(_("Maximum users: \002%d\002 (%s)"), MaxUserCount, Anope::strftime(MaxUserTime, source.GetAccount()).c_str());
		source.Reply(_("Services up %s."), Anope::Duration(uptime, source.GetAccount()).c_str());

		return;
	}

	static void DoStatsUplink(CommandSource &source)
	{
		Anope::string buf;
		for (const auto &capab : Servers::Capab)
			buf += " " + capab;
		if (!buf.empty())
			buf.erase(buf.begin());

		source.Reply(_("Uplink server: %s"), Me->GetLinks().front()->GetName().c_str());
		source.Reply(_("Uplink capab: %s"), buf.c_str());
		source.Reply(_("Servers found: %d"), stats_count_servers(Me->GetLinks().front()));
		return;
	}

	template<typename T> void GetHashStats(const T &map, size_t &entries, size_t &buckets, size_t &max_chain)
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
		source.Reply(_("Users (nick): %lu entries, %lu buckets, longest chain is %zu"), entries, buckets, max_chain);

		if (!UserListByUID.empty())
		{
			GetHashStats(UserListByUID, entries, buckets, max_chain);
			source.Reply(_("Users (uid): %lu entries, %lu buckets, longest chain is %zu"), entries, buckets, max_chain);
		}

		GetHashStats(ChannelList, entries, buckets, max_chain);
		source.Reply(_("Channels: %zu entries, %zu buckets, longest chain is %zu"), entries, buckets, max_chain);

		GetHashStats(*RegisteredChannelList, entries, buckets, max_chain);
		source.Reply(_("Registered channels: %zu entries, %zu buckets, longest chain is %zu"), entries, buckets, max_chain);

		GetHashStats(*NickAliasList, entries, buckets, max_chain);
		source.Reply(_("Registered nicknames: %zu entries, %zu buckets, longest chain is %zu"), entries, buckets, max_chain);

		GetHashStats(*NickCoreList, entries, buckets, max_chain);
		source.Reply(_("Registered nick groups: %zu entries, %zu buckets, longest chain is %zu"), entries, buckets, max_chain);

		if (session_service)
		{
			GetHashStats(session_service->GetSessions(), entries, buckets, max_chain);
			source.Reply(_("Sessions: %zu entries, %zu buckets, longest chain is %zu"), entries, buckets, max_chain);
		}
	}

	void DoStatsPassword(CommandSource &source)
	{
		Anope::map<size_t> counts;
		size_t missing = 0;
		size_t unknown = 0;
		for (const auto &[_, nc] : *NickCoreList)
		{
			if (nc->pass.empty())
			{
				missing++;
				continue;
			}

			auto sep = nc->pass.find(':');
			if (sep == Anope::string::npos)
			{
				unknown++;
				continue;
			}

			counts[nc->pass.substr(0, sep)]++;
		}

		for (const auto &[algo, count] : counts)
			source.Reply(_("Passwords encrypted with %s: %zu"), algo.c_str(), count);
		if (missing)
			source.Reply(_("Missing passwords: %zu"), missing);
		if (unknown)
			source.Reply(_("Unknown passwords: %zu"), unknown);
	}

public:
	CommandOSStats(Module *creator) : Command(creator, "operserv/stats", 0, 1),
		akills("XLineManager", "xlinemanager/sgline"), snlines("XLineManager", "xlinemanager/snline"), sqlines("XLineManager", "xlinemanager/sqline")
	{
		this->SetDesc(_("Show status of services and network"));
		this->SetSyntax("[AKILL | HASH | PASSWORD | UPLINK | UPTIME | ALL | RESET]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string extra = !params.empty() ? params[0] : "";

		Log(LOG_ADMIN, source, this) << extra;

		if (extra.equals_ci("RESET"))
			return this->DoStatsReset(source);

		bool handled = false;
		if (extra.equals_ci("ALL") || extra.equals_ci("AKILL"))
		{
			this->DoStatsAkill(source);
			handled = true;
		}

		if (extra.equals_ci("ALL") || extra.equals_ci("HASH"))
		{
			this->DoStatsHash(source);
			handled = true;
		}

		if (extra.equals_ci("ALL") || extra.equals_ci("PASSWORD"))
		{
			this->DoStatsPassword(source);
			handled = true;
		}

		if (extra.equals_ci("ALL") || extra.equals_ci("UPLINK"))
		{
			this->DoStatsUplink(source);
			handled = true;
		}

		if (extra.empty() || extra.equals_ci("ALL") || extra.equals_ci("UPTIME"))
		{
			this->DoStatsUptime(source);
			handled = true;
		}

		if (!handled)
			source.Reply(_("Unknown STATS option: \002%s\002"), extra.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Without any option, shows the current number of users online,\n"
				"and the highest number of users online since services was\n"
				"started, and the length of time services has been running.\n"
				" \n"
				"With the \002AKILL\002 option, displays the current size of the\n"
				"AKILL list and the current default expiry time.\n"
				" \n"
				"The \002RESET\002 option currently resets the maximum user count\n"
				"to the number of users currently present on the network.\n"
				" \n"
				"The \002PASSWORD\002 option displays the encryption algorithms used\n"
				"for user passwords.\n"
				" \n"
				"The \002UPLINK\002 option displays information about the current\n"
				"server Anope uses as an uplink to the network.\n"
				" \n"
				"The \002HASH\002 option displays information about the hash maps.\n"
				" \n"
				"The \002ALL\002 option displays all of the above statistics."));
		return true;
	}
};

class OSStats final
	: public Module
{
	CommandOSStats commandosstats;
	Serialize::Type stats_type;
	Stats stats_saver;

public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosstats(this), stats_type("Stats", Stats::Unserialize)
	{

	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (UserListByNick.size() == MaxUserCount && Anope::CurTime == MaxUserTime)
			stats_saver.QueueUpdate();
	}
};

MODULE_INIT(OSStats)
