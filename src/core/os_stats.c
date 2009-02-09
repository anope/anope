/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

void get_operserv_stats(long *nrec, long *memuse);
void myOperServHelp(User *u);

class CommandOSStats : public Command
{
 private:
	CommandResult DoStatsAkill(User *u, std::vector<std::string> &params)
	{
		int timeout;
		/* AKILLs */
		notice_lang(s_OperServ, u, OPER_STATS_AKILL_COUNT, akills.count);
		timeout = AutokillExpiry + 59;
		if (timeout >= 172800)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAYS, timeout / 86400);
		else if (timeout >= 86400)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAY);
		else if (timeout >= 7200)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOURS, timeout / 3600);
		else if (timeout >= 3600)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOUR);
		else if (timeout >= 120)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MINS, timeout / 60);
		else if (timeout >= 60)
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MIN);
		else
			notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_NONE);
		if (ircd->sgline)
		{
			/* SGLINEs */
			notice_lang(s_OperServ, u, OPER_STATS_SGLINE_COUNT, sglines.count);
			timeout = SGLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_MIN);
			else
				notice_lang(s_OperServ, u, OPER_STATS_SGLINE_EXPIRE_NONE);
		}
		if (ircd->sqline)
		{
			/* SQLINEs */
			notice_lang(s_OperServ, u, OPER_STATS_SQLINE_COUNT, sqlines.count);
			timeout = SQLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_MIN);
			else
				notice_lang(s_OperServ, u, OPER_STATS_SQLINE_EXPIRE_NONE);
		}
		if (ircd->szline)
		{
			/* SZLINEs */
			notice_lang(s_OperServ, u, OPER_STATS_SZLINE_COUNT, szlines.count);
			timeout = SZLineExpiry + 59;
			if (timeout >= 172800)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_DAYS, timeout / 86400);
			else if (timeout >= 86400)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_DAY);
			else if (timeout >= 7200)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_HOURS, timeout / 3600);
			else if (timeout >= 3600)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_HOUR);
			else if (timeout >= 120)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_MINS, timeout / 60);
			else if (timeout >= 60)
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_MIN);
			else
				notice_lang(s_OperServ, u, OPER_STATS_SZLINE_EXPIRE_NONE);
		}
		return MOD_CONT;
	}

	CommandResult DoStatsReset(User *u, std::vector<std::string> &params)
	{
		if (is_services_admin(u))
		{
			maxusercnt = usercnt;
			notice_lang(s_OperServ, u, OPER_STATS_RESET);
		}
		else
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
		return MOD_CONT;
	}

	CommandResult DoStatsUptime(User *u, std::vector<std::string> &params)
	{
		notice_lang(s_OperServ, u, OPER_STATS_CURRENT_USERS, usercnt, opcnt);
		tm = localtime(&maxusertime);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(s_OperServ, u, OPER_STATS_MAX_USERS, maxusercnt, timebuf);
		if (days > 1)
			notice_lang(s_OperServ, u, OPER_STATS_UPTIME_DHMS, days, hours, mins, secs);
		else if (days == 1)
			notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1DHMS, days, hours, mins, secs);
		else
		{
			if (hours > 1) {
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_HMS, hours, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_HM1S, hours, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_H1MS, hours, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_H1M1S, hours, mins, secs);
				}
			}
			else if (hours == 1)
			{
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1HMS, hours, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1HM1S, hours, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1H1MS, hours, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1H1M1S, hours, mins, secs);
				}
			}
			else
			{
				if (mins != 1)
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_MS, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_M1S, mins, secs);
				}
				else
				{
					if (secs != 1)
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1MS, mins, secs);
					else
						notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1M1S, mins, secs);
				}
			}
		}
	}

	CommandResult DoStatsUplink(User *u, std::vector<std::string> &params)
	{
		buf[0] = '\0';
		buflen = 511; /* How confusing, this is the amount of space left! */
		for (i = 0; capab_info[i].token; ++i)
		{
			if (uplink_capab & capab_info[i].flag)
			{
				strncat(buf, " ", buflen);
				--buflen;
				strncat(buf, capab_info[i].token, buflen);
				buflen -= strlen(capab_info[i].token);
				/* Special cases */
				if (capab_info[i].flag == CAPAB_CHANMODE)
				{
					strncat(buf, "=", buflen);
					--buflen;
					strncat(buf, ircd->chanmodes, buflen);
					buflen -= strlen(ircd->chanmodes);
				}
				if (capab_info[i].flag == CAPAB_NICKCHARS)
				{
					strncat(buf, "=", buflen);
					--buflen;
					if (ircd->nickchars)
					{
						strncat(buf, ircd->nickchars, buflen);
						buflen -= strlen(ircd->nickchars);
					} /* leave blank if it was null */
				}
			}
		}
		notice_lang(s_OperServ, u, OPER_STATS_UPLINK_SERVER, serv_uplink->name);
		notice_lang(s_OperServ, u, OPER_STATS_UPLINK_CAPAB, buf);
		notice_lang(s_OperServ, u, OPER_STATS_UPLINK_SERVER_COUNT, stats_count_servers(serv_uplink));
		return MOD_CONT;
	}

	CommandResult DoStatsMemory(User *u, std::vector<std::string> &params)
	{
		long count, mem;

		notice_lang(s_OperServ, u, OPER_STATS_BYTES_READ, total_read / 1024);
		notice_lang(s_OperServ, u, OPER_STATS_BYTES_WRITTEN, total_written / 1024);

		get_user_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_USER_MEM, count, (mem + 512) / 1024);
		get_channel_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_CHANNEL_MEM, count, (mem + 512) / 1024);
		get_core_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_GROUPS_MEM, count, (mem + 512) / 1024);
		get_aliases_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_ALIASES_MEM, count, (mem + 512) / 1024);
		get_chanserv_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_CHANSERV_MEM, count, (mem + 512) / 1024);
		if (s_BotServ)
		{
			get_botserv_stats(&count, &mem);
			notice_lang(s_OperServ, u, OPER_STATS_BOTSERV_MEM, count, (mem + 512) / 1024);
		}
		if (s_HostServ)
		{
			get_hostserv_stats(&count, &mem);
			notice_lang(s_OperServ, u, OPER_STATS_HOSTSERV_MEM, count, (mem + 512) / 1024);
		}
		get_operserv_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_OPERSERV_MEM, count, (mem + 512) / 1024);
		get_session_stats(&count, &mem);
		notice_lang(s_OperServ, u, OPER_STATS_SESSIONS_MEM, count, (mem + 512) / 1024);
	}
 public:
	CommandOSStats() : Command("STATS", 0, 1)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		time_t uptime = time(NULL) - start_time;
		const char *extra = params.size() ? params[0].c_str() : NULL;
		int days = uptime / 86400, hours = (uptime / 3600) % 24, mins = (uptime / 60) % 60, secs = uptime % 60;
		struct tm *tm;
		char timebuf[64];
		char buf[512];
		int buflen;
		int i;

		if (extra && stricmp(extra, "ALL"))
		{
			if (!stricmp(extra, "AKILL"))
				return this->DoStatsAkill(u, params);
			else if (!stricmp(extra, "RESET"))
				return this->DoStatsReset(u, params);
			else if (stricmp(extra, "MEMORY") && stricmp(extra, "UPLINK"))
				notice_lang(s_OperServ, u, OPER_STATS_UNKNOWN_OPTION, extra);
		}

		if (!extra || (stricmp(extra, "MEMORY") && stricmp(extra, "UPLINK")))
			return this->DoStatsUptime(u, params);

		if (extra && (!stricmp(extra, "ALL") || !stricmp(extra, "UPLINK")) && is_services_admin(u))
			return this->DoStatsUplink(u, params);

		if (extra && (!stricmp(extra, "ALL") || !stricmp(extra, "MEMORY")) && is_services_admin(u))
			return this->DoStatsMemory(u, params);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_OperServ, u, OPER_HELP_STATS);
		return true;
	}
};

class OSStats : public Module
{
 public:
	OSStats(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSStats(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	notice_lang(s_OperServ, u, OPER_HELP_CMD_STATS);
}

/**
 * Count servers connected to server s
 * @param s The server to start counting from
 * @return Amount of servers connected to server s
 **/
int stats_count_servers(Server *s)
{
	int count = 0;

	while (s)
	{
		++count;
		if (s->links)
			count += stats_count_servers(s->links);
		s = s->next;
	}

	return count;
}

void get_operserv_stats(long *nrec, long *memuse)
{
	int i;
	long mem = 0, count = 0, mem2 = 0, count2 = 0;
	Akill *ak;
	SXLine *sx;

	count += akills.count;
	mem += akills.capacity;
	mem += akills.count * sizeof(Akill);

	for (i = 0; i < akills.count; ++i)
	{
		ak = static_cast<Akill *>(akills.list[i]);
		mem += strlen(ak->user) + 1;
		mem += strlen(ak->host) + 1;
		mem += strlen(ak->by) + 1;
		mem += strlen(ak->reason) + 1;
	}

	if (ircd->sgline)
	{
		count += sglines.count;
		mem += sglines.capacity;
		mem += sglines.count * sizeof(SXLine);

		for (i = 0; i < sglines.count; ++i)
		{
			sx = static_cast<SXLine *>(sglines.list[i]);
			mem += strlen(sx->mask) + 1;
			mem += strlen(sx->by) + 1;
			mem += strlen(sx->reason) + 1;
		}
	}
	if (ircd->sqline)
	{
		count += sqlines.count;
		mem += sqlines.capacity;
		mem += sqlines.count * sizeof(SXLine);

		for (i = 0; i < sqlines.count; ++i)
		{
			sx = static_cast<SXLine *>(sqlines.list[i]);
			mem += strlen(sx->mask) + 1;
			mem += strlen(sx->by) + 1;
			mem += strlen(sx->reason) + 1;
		}
	}
	if (ircd->szline)
	{
		count += szlines.count;
		mem += szlines.capacity;
		mem += szlines.count * sizeof(SXLine);

		for (i = 0; i < szlines.count; ++i)
		{
			sx = static_cast<SXLine *>(szlines.list[i]);
			mem += strlen(sx->mask) + 1;
			mem += strlen(sx->by) + 1;
			mem += strlen(sx->reason) + 1;
		}
	}

	get_news_stats(&count2, &mem2);
	count += count2;
	mem += mem2;
	get_exception_stats(&count2, &mem2);
	count += count2;
	mem += mem2;

	*nrec = count;
	*memuse = mem;
}

MODULE_INIT("os_stats", OSStats)
