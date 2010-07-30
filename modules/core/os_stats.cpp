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
		for (std::list<Server *>::const_iterator it = s->GetLinks().begin(), it_end = s->GetLinks().end(); it != it_end; ++it)
			count += stats_count_servers(*it);

	return count;
}

class CommandOSStats : public Command
{
 private:
	CommandReturn DoStatsAkill(User *u)
	{
		int timeout;
		/* AKILLs */
		notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_COUNT, SGLine->GetCount());
		timeout = Config.AutokillExpiry + 59;
		if (timeout >= 172800)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAYS, timeout / 86400);
		else if (timeout >= 86400)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAY);
		else if (timeout >= 7200)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOURS, timeout / 3600);
		else if (timeout >= 3600)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOUR);
		else if (timeout >= 120)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MINS, timeout / 60);
		else if (timeout >= 60)
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MIN);
		else
			notice_lang(Config.s_OperServ, u, OPER_STATS_AKILL_EXPIRE_NONE);
		if (ircd->snline)
		{
			/* SNLINEs */
			notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_COUNT, SNLine->GetCount());
			timeout = Config.SNLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_MIN);
			else
				notice_lang(Config.s_OperServ, u, OPER_STATS_SNLINE_EXPIRE_NONE);
		}
		if (ircd->sqline)
		{
			/* SQLINEs */
			notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_COUNT, SQLine->GetCount());
			timeout = Config.SQLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_MIN);
			else
				notice_lang(Config.s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_NONE);
		}
		if (ircd->szline)
		{
			/* SZLINEs */
			notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_COUNT, SZLine->GetCount());
			timeout = Config.SZLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_MIN);
			else
				notice_lang(Config.s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_NONE);
		}
		return MOD_CONT;
	}

	CommandReturn DoStatsReset(User *u)
	{
		maxusercnt = usercnt;
		notice_lang(Config.s_OperServ, u, OPER_STATS_RESET);
		return MOD_CONT;
	}

	CommandReturn DoStatsUptime(User *u)
	{
		char timebuf[64];
		time_t uptime = time(NULL) - start_time;
		int days = uptime / 86400, hours = (uptime / 3600) % 24, mins = (uptime / 60) % 60, secs = uptime % 60;
		notice_lang(Config.s_OperServ, u, OPER_STATS_CURRENT_USERS, usercnt, opcnt);
		struct tm *tm = localtime(&maxusertime);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(Config.s_OperServ, u, OPER_STATS_MAX_USERS, maxusercnt, timebuf);
		if (days > 1)
			notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_DHMS, days, hours, mins, secs);
		else if (days == 1)
			notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1DHMS, days, hours, mins, secs);
		else
		{
			if (hours > 1)
			{
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_HMS, hours, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_HM1S, hours, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_H1MS, hours, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_H1M1S, hours, mins, secs);
				}
			}
			else if (hours == 1)
			{
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1HMS, hours, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1HM1S, hours, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1H1MS, hours, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1H1M1S, hours, mins, secs);
				}
			}
			else
			{
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_MS, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_M1S, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1MS, mins, secs);
					else
						notice_lang(Config.s_OperServ, u, OPER_STATS_UPTIME_1M1S, mins, secs);
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

		notice_lang(Config.s_OperServ, u, OPER_STATS_UPLINK_SERVER, Me->GetUplink()->GetName().c_str());
		notice_lang(Config.s_OperServ, u, OPER_STATS_UPLINK_CAPAB, buf.c_str());
		notice_lang(Config.s_OperServ, u, OPER_STATS_UPLINK_SERVER_COUNT, stats_count_servers(Me->GetUplink()));
		return MOD_CONT;
	}

	CommandReturn DoStatsMemory(User *u)
	{
		long count, mem;

		notice_lang(Config.s_OperServ, u, OPER_STATS_BYTES_READ, TotalRead / 1024);
		notice_lang(Config.s_OperServ, u, OPER_STATS_BYTES_WRITTEN, TotalWritten / 1024);

		get_user_stats(count, mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_USER_MEM, count, (mem + 512) / 1024);
		get_channel_stats(&count, &mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_CHANNEL_MEM, count, (mem + 512) / 1024);
		get_core_stats(count, mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_GROUPS_MEM, count, (mem + 512) / 1024);
		get_aliases_stats(count, mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_ALIASES_MEM, count, (mem + 512) / 1024);
		get_chanserv_stats(&count, &mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_CHANSERV_MEM, count, (mem + 512) / 1024);
		if (!Config.s_BotServ.empty())
		{
			get_botserv_stats(&count, &mem);
			notice_lang(Config.s_OperServ, u, OPER_STATS_BOTSERV_MEM, count, (mem + 512) / 1024);
		}
		if (!Config.s_HostServ.empty())
		{
			get_hostserv_stats(&count, &mem);
			notice_lang(Config.s_OperServ, u, OPER_STATS_HOSTSERV_MEM, count, (mem + 512) / 1024);
		}
		get_operserv_stats(&count, &mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_OPERSERV_MEM, count, (mem + 512) / 1024);
		get_session_stats(count, mem);
		notice_lang(Config.s_OperServ, u, OPER_STATS_SESSIONS_MEM, count, (mem + 512) / 1024);

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
				notice_lang(Config.s_OperServ, u, OPER_STATS_UNKNOWN_OPTION, extra.c_str());
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
		notice_help(Config.s_OperServ, u, OPER_HELP_STATS);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_STATS);
	}
};

class OSStats : public Module
{
 public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSStats());
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
