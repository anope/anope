/* OperServ core functions
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
	service_reference<XLineManager> akills, snlines, sqlines;
 private:
	void DoStatsAkill(CommandSource &source)
	{
		int timeout;
		if (akills)
		{
			/* AKILLs */
			source.Reply(_("Current number of AKILLs: \002%d\002"), akills->GetCount());
			timeout = Config->AutokillExpiry + 59;
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
		if (ircd->snline && snlines)
		{
			/* SNLINEs */
			source.Reply(_("Current number of SNLINEs: \002%d\002"), snlines->GetCount());
			timeout = Config->SNLineExpiry + 59;
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
		if (ircd->sqline && sqlines)
		{
			/* SQLINEs */
			source.Reply(_("Current number of SQLINEs: \002%d\002"), sqlines->GetCount());
			timeout = Config->SQLineExpiry + 59;
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
		return;
	}

	void DoStatsReset(CommandSource &source)
	{
		maxusercnt = usercnt;
		source.Reply(_("Statistics reset."));
		return;
	}

	void DoStatsUptime(CommandSource &source)
	{
		time_t uptime = Anope::CurTime - start_time;
		source.Reply(_("Current users: \002%d\002 (\002%d\002 ops)"), usercnt, opcnt);
		source.Reply(_("Maximum users: \002%d\002 (%s)"), maxusercnt, do_strftime(maxusertime).c_str());
		source.Reply(_("Services up %s"), duration(uptime).c_str());

		return;
	}

	void DoStatsUplink(CommandSource &source)
	{
		Anope::string buf;
		for (std::set<Anope::string>::iterator it = Capab.begin(); it != Capab.end(); ++it)
			buf += *it;
		if (!buf.empty())
			buf.erase(buf.begin());

		source.Reply(_("Uplink server: %s"), Me->GetLinks().front()->GetName().c_str());
		source.Reply(_("Uplink capab: %s"), buf.c_str());
		source.Reply(_("Servers found: %d"), stats_count_servers(Me->GetLinks().front()));
		return;
	}

 public:
	CommandOSStats(Module *creator) : Command(creator, "operserv/stats", 0, 1),
		akills("XLineManager", "xlinemanager/sgline"), snlines("XLineManager", "xlinemanager/snline"), sqlines("XLineManager", "xlinemanager/sqline")
	{
		this->SetDesc(_("Show status of Services and network"));
		this->SetSyntax(_("[AKILL | ALL | RESET | UPLINK]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string extra = !params.empty() ? params[0] : "";

		if (extra.equals_ci("RESET"))
			return this->DoStatsReset(source);

		if (extra.equals_ci("ALL") || extra.equals_ci("AKILL"))
			this->DoStatsAkill(source);

		if (extra.empty() || extra.equals_ci("ALL") || extra.equals_ci("UPTIME"))
			this->DoStatsUptime(source);

		if (extra.equals_ci("ALL") || extra.equals_ci("UPLINK"))
			this->DoStatsUplink(source);

		if (!extra.empty() && !extra.equals_ci("ALL") && !extra.equals_ci("AKILL") && !extra.equals_ci("UPLINK"))
			source.Reply(_("Unknown STATS option \002%s\002."), extra.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Without any option, shows the current number of users online,\n"
				"and the highest number of users online since Services was\n"
				"started, and the length of time Services has been running.\n"
				" \n"
				"With the \002AKILL\002 option, displays the current size of the\n"
				"AKILL list and the current default expiry time.\n"
				" \n"
				"The \002RESET\002 option currently resets the maximum user count\n"
				"to the number of users currently present on the network.\n"
				" \n"
				"The \002UPLINK\002 option displays information about the current\n"
				"server Anope uses as an uplink to the network.\n"
				" \n"
				"The \002ALL\002 displays the user and uptime statistics, and\n"
				"everything you'd see with the \002UPLINK\002 option."));
		return true;
	}
};

class OSStats : public Module
{
	CommandOSStats commandosstats;

 public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosstats(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(OSStats)
