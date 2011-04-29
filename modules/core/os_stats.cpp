/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "operserv.h"

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
	CommandReturn DoStatsAkill(CommandSource &source)
	{
		int timeout;
		/* AKILLs */
		source.Reply(_("Current number of AKILLs: \002%d\002"), SGLine->GetCount());
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
		if (ircd->snline)
		{
			/* SNLINEs */
			source.Reply(_("Current number of SNLINEs: \002%d\002"), SNLine->GetCount());
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
		if (ircd->sqline)
		{
			/* SQLINEs */
			source.Reply(_("Current number of SQLINEs: \002%d\002"), SQLine->GetCount());
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
		if (ircd->szline)
		{
			/* SZLINEs */
			source.Reply(_("Current number of SZLINEs: \002%d\002"), SZLine->GetCount());
			timeout = Config->SZLineExpiry + 59;
			if (timeout >= 172800)
				source.Reply(_("Default SZLINE expiry time: \002%d days\002"), timeout / 86400);
			else if (timeout >= 86400)
				source.Reply(_("Default SZLINE expiry time: \0021 day\002"));
			else if (timeout >= 7200)
				source.Reply(_("Default SZLINE expiry time: \002%d hours\002"), timeout / 3600);
			else if (timeout >= 3600)
				source.Reply(_("Default SZLINE expiry time: \0021 hour\002"));
			else if (timeout >= 120)
				source.Reply(_("Default SZLINE expiry time: \002%d minutes\002"), timeout / 60);
			else if (timeout >= 60)
				source.Reply(_("Default SZLINE expiry time: \0021 minute\002"));
			else
				source.Reply(_("Default SZLINE expiry time: \002No expiration\002"));
		}
		return MOD_CONT;
	}

	CommandReturn DoStatsReset(CommandSource &source)
	{
		maxusercnt = usercnt;
		source.Reply(_("Statistics reset."));
		return MOD_CONT;
	}

	CommandReturn DoStatsUptime(CommandSource &source)
	{
		time_t uptime = Anope::CurTime - start_time;
		source.Reply(_("Current users: \002%d\002 (\002%d\002 ops)"), usercnt, opcnt);
		source.Reply(_("Maximum users: \002%d\002 (%s)"), maxusercnt, do_strftime(maxusertime).c_str());
		source.Reply(_("Services up %s"), duration(uptime).c_str());

		return MOD_CONT;
	}

	CommandReturn DoStatsUplink(CommandSource &source)
	{
		Anope::string buf;

		for (unsigned j = 0; !Capab_Info[j].Token.empty(); ++j)
			if (Capab.HasFlag(Capab_Info[j].Flag))
				buf += " " + Capab_Info[j].Token;

		if (!buf.empty())
			buf.erase(buf.begin());

		source.Reply(_("Uplink server: %s"), Me->GetLinks().front()->GetName().c_str());
		source.Reply(_("Uplink capab: %s"), buf.c_str());
		source.Reply(_("Servers found: %d"), stats_count_servers(Me->GetLinks().front()));
		return MOD_CONT;
	}

 public:
	CommandOSStats() : Command("STATS", 0, 1, "operserv/stats")
	{
		this->SetDesc(_("Show status of Services and network"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string extra = !params.empty() ? params[0] : "";

		if (extra.equals_ci("ALL"))
			extra.clear();

		if (extra.empty() || extra.equals_ci("AKILL"))
			return this->DoStatsAkill(source);
		else if (extra.equals_ci("RESET"))
			return this->DoStatsReset(source);
		else if (extra.empty() || extra.equals_ci("UPTIME"))
			this->DoStatsUptime(source);
		else if (extra.empty() || extra.equals_ci("UPLINK"))
			this->DoStatsUplink(source);
			else if (!extra.equals_ci("UPLINK"))
				source.Reply(_("Unknown STATS option \002%s\002."), extra.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STATS [AKILL | ALL | RESET | UPLINK]\002\n"
				" \n"
				"Without any option, shows the current number of users and\n"
				"IRCops online (excluding Services), the highest number of\n"
				"users online since Services was started, and the length of\n"
				"time Services has been running.\n"
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
				"everything you'd see with \002MEMORY\002 and \002UPLINK\002 options."));
		return true;
	}
};

class OSStats : public Module
{
	CommandOSStats commandosstats;

 public:
	OSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosstats);
	}
};

MODULE_INIT(OSStats)
