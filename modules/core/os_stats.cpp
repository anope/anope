/* OperServ core functions
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

void get_operserv_stats(long *nrec, long *memuse);

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
 private:
	CommandReturn DoStatsAkill(User *u)
	{
		int timeout;
		/* AKILLs */
		u->SendMessage(OperServ, OPER_STATS_AKILL_COUNT, SGLine->GetCount());
		timeout = Config->AutokillExpiry + 59;
		if (timeout >= 172800)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_DAYS, timeout / 86400);
		else if (timeout >= 86400)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_DAY);
		else if (timeout >= 7200)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_HOURS, timeout / 3600);
		else if (timeout >= 3600)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_HOUR);
		else if (timeout >= 120)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_MINS, timeout / 60);
		else if (timeout >= 60)
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_MIN);
		else
			u->SendMessage(OperServ, OPER_STATS_AKILL_EXPIRE_NONE);
		if (ircd->snline)
		{
			/* SNLINEs */
			u->SendMessage(OperServ, OPER_STATS_SNLINE_COUNT, SNLine->GetCount());
			timeout = Config->SNLineExpiry + 59;
			if (timeout >= 172800)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_MIN);
			else
				u->SendMessage(OperServ, OPER_STATS_SNLINE_EXPIRE_NONE);
		}
		if (ircd->sqline)
		{
			/* SQLINEs */
			u->SendMessage(OperServ, OPER_STATS_SQLINE_COUNT, SQLine->GetCount());
			timeout = Config->SQLineExpiry + 59;
			if (timeout >= 172800)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_MIN);
			else
				u->SendMessage(OperServ, OPER_STATS_SQLINE_EXPIRE_NONE);
		}
		if (ircd->szline)
		{
			/* SZLINEs */
			u->SendMessage(OperServ, OPER_STATS_SZLINE_COUNT, SZLine->GetCount());
			timeout = Config->SZLineExpiry + 59;
			if (timeout >= 172800)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_MIN);
			else
				u->SendMessage(OperServ, OPER_STATS_SZLINE_EXPIRE_NONE);
		}
		return MOD_CONT;
	}

	CommandReturn DoStatsReset(User *u)
	{
		maxusercnt = usercnt;
		u->SendMessage(OperServ, OPER_STATS_RESET);
		return MOD_CONT;
	}

	CommandReturn DoStatsUptime(User *u)
	{
		time_t uptime = Anope::CurTime - start_time;
		int days = uptime / 86400, hours = (uptime / 3600) % 24, mins = (uptime / 60) % 60, secs = uptime % 60;
		u->SendMessage(OperServ, OPER_STATS_CURRENT_USERS, usercnt, opcnt);
		u->SendMessage(OperServ, OPER_STATS_MAX_USERS, maxusercnt, do_strftime(maxusertime).c_str());
		if (days > 1)
			u->SendMessage(OperServ, OPER_STATS_UPTIME_DHMS, days, hours, mins, secs);
		else if (days == 1)
			u->SendMessage(OperServ, OPER_STATS_UPTIME_1DHMS, days, hours, mins, secs);
		else
		{
			if (hours > 1)
			{
				if (mins != 1)
					u->SendMessage(OperServ, OPER_STATS_UPTIME_HMS, hours, mins);
				else
					u->SendMessage(OperServ, OPER_STATS_UPTIME_H1MS, hours, mins, secs);
			}
			else if (hours == 1)
			{
				if (mins != 1)
					u->SendMessage(OperServ, OPER_STATS_UPTIME_1HMS, hours, mins, secs);
				else
					u->SendMessage(OperServ, OPER_STATS_UPTIME_1H1MS, hours, mins, secs);
			}
			else
			{
				if (mins != 1)
				{
					if (secs != 1)
						u->SendMessage(OperServ, OPER_STATS_UPTIME_MS, mins, secs);
					else
						u->SendMessage(OperServ, OPER_STATS_UPTIME_M1S, mins, secs);
				}
				else
				{
					if (secs != 1)
						u->SendMessage(OperServ, OPER_STATS_UPTIME_1MS, mins, secs);
					else
						u->SendMessage(OperServ, OPER_STATS_UPTIME_1M1S, mins, secs);
				}
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoStatsUplink(User *u)
	{
		Anope::string buf;

		for (unsigned j = 0; !Capab_Info[j].Token.empty(); ++j)
			if (Capab.HasFlag(Capab_Info[j].Flag))
				buf += " " + Capab_Info[j].Token;

		if (!buf.empty())
			buf.erase(buf.begin());

		u->SendMessage(OperServ, OPER_STATS_UPLINK_SERVER, Me->GetLinks().front()->GetName().c_str());
		u->SendMessage(OperServ, OPER_STATS_UPLINK_CAPAB, buf.c_str());
		u->SendMessage(OperServ, OPER_STATS_UPLINK_SERVER_COUNT, stats_count_servers(Me->GetLinks().front()));
		return MOD_CONT;
	}

	CommandReturn DoStatsMemory(User *u)
	{
		long count, mem;

		u->SendMessage(OperServ, OPER_STATS_BYTES_READ, TotalRead / 1024);
		u->SendMessage(OperServ, OPER_STATS_BYTES_WRITTEN, TotalWritten / 1024);

		get_user_stats(count, mem);
		u->SendMessage(OperServ, OPER_STATS_USER_MEM, count, (mem + 512) / 1024);
		get_channel_stats(&count, &mem);
		u->SendMessage(OperServ, OPER_STATS_CHANNEL_MEM, count, (mem + 512) / 1024);
		get_core_stats(count, mem);
		u->SendMessage(OperServ, OPER_STATS_GROUPS_MEM, count, (mem + 512) / 1024);
		get_aliases_stats(count, mem);
		u->SendMessage(OperServ, OPER_STATS_ALIASES_MEM, count, (mem + 512) / 1024);
		get_chanserv_stats(&count, &mem);
		u->SendMessage(OperServ, OPER_STATS_CHANSERV_MEM, count, (mem + 512) / 1024);
		if (!Config->s_BotServ.empty())
		{
			get_botserv_stats(&count, &mem);
			u->SendMessage(OperServ, OPER_STATS_BOTSERV_MEM, count, (mem + 512) / 1024);
		}
		if (!Config->s_HostServ.empty())
		{
			get_hostserv_stats(&count, &mem);
			u->SendMessage(OperServ, OPER_STATS_HOSTSERV_MEM, count, (mem + 512) / 1024);
		}
		get_operserv_stats(&count, &mem);
		u->SendMessage(OperServ, OPER_STATS_OPERSERV_MEM, count, (mem + 512) / 1024);
		get_session_stats(count, mem);
		u->SendMessage(OperServ, OPER_STATS_SESSIONS_MEM, count, (mem + 512) / 1024);

		return MOD_CONT;
	}
 public:
	CommandOSStats() : Command("STATS", 0, 1, "operserv/stats")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string extra = !params.empty() ? params[0] : "";

		if (!extra.empty() && !extra.equals_ci("ALL"))
		{
			if (extra.equals_ci("AKILL"))
				return this->DoStatsAkill(u);
			else if (extra.equals_ci("RESET"))
				return this->DoStatsReset(u);
			else if (!extra.equals_ci("MEMORY") && !extra.equals_ci("UPLINK"))
				u->SendMessage(OperServ, OPER_STATS_UNKNOWN_OPTION, extra.c_str());
		}

		if (extra.empty() || (!extra.equals_ci("MEMORY") && !extra.equals_ci("UPLINK")))
			this->DoStatsUptime(u);

		if (!extra.empty() && (extra.equals_ci("ALL") || extra.equals_ci("UPLINK")))
			this->DoStatsUplink(u);

		if (!extra.empty() && (extra.equals_ci("ALL") || extra.equals_ci("MEMORY")))
			this->DoStatsMemory(u);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_STATS);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_STATS);
	}
};

class OSStats : public Module
{
	CommandOSStats commandosstats;

 public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosstats);
	}
};

void get_operserv_stats(long *nrec, long *memuse)
{
	unsigned i, end;
	long mem = 0, count = 0, mem2 = 0, count2 = 0;
	XLine *x;

	end = SGLine->GetCount();
	count += end;
	mem += end * sizeof(XLine);

	for (i = 0; i < end; ++i)
	{
		x = SGLine->GetEntry(i);

		mem += x->GetNick().length() + 1;
		mem += x->GetUser().length() + 1;
		mem += x->GetHost().length() + 1;
		mem += x->Mask.length() + 1;
		mem += x->By.length() + 1;
		mem += x->Reason.length() + 1;
	}

	if (ircd->snline)
	{
		end = SNLine->GetCount();
		count += end;
		mem += end * sizeof(XLine);

		for (i = 0; i < end; ++i)
		{
			x = SNLine->GetEntry(i);

			mem += x->GetNick().length() + 1;
			mem += x->GetUser().length() + 1;
			mem += x->GetHost().length() + 1;
			mem += x->Mask.length() + 1;
			mem += x->By.length() + 1;
			mem += x->Reason.length() + 1;
		}
	}
	if (ircd->sqline)
	{
		end = SQLine->GetCount();
		count += end;
		mem += end * sizeof(XLine);

		for (i = 0; i < end; ++i)
		{
			x = SNLine->GetEntry(i);

			mem += x->GetNick().length() + 1;
			mem += x->GetUser().length() + 1;
			mem += x->GetHost().length() + 1;
			mem += x->Mask.length() + 1;
			mem += x->By.length() + 1;
			mem += x->Reason.length() + 1;
		}
	}
	if (ircd->szline)
	{
		end = SZLine->GetCount();
		count += end;
		mem += end * sizeof(XLine);

		for (i = 0; i < end; ++i)
		{
			x = SZLine->GetEntry(i);

			mem += x->GetNick().length() + 1;
			mem += x->GetUser().length() + 1;
			mem += x->GetHost().length() + 1;
			mem += x->Mask.length() + 1;
			mem += x->By.length() + 1;
			mem += x->Reason.length() + 1;
		}
	}

	get_exception_stats(count2, mem2);
	count += count2;
	mem += mem2;

	*nrec = count;
	*memuse = mem;
}

MODULE_INIT(OSStats)
