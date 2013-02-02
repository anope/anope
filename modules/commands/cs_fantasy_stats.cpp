/* Chanstats core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "../extra/sql.h"

class MySQLInterface : public SQL::Interface
{
 public:
	MySQLInterface(Module *o) : SQL::Interface(o) { }

	void OnResult(const SQL::Result &r) anope_override
	{
	}

	void OnError(const SQL::Result &r) anope_override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Chanstats: Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Chanstats: Error executing query: " << r.GetError();
	}
};


class CommandCSStats : public Command
{
 public:
	CommandCSStats(Module *creator) : Command (creator, "chanserv/stats", 0, 2)
	{
		this->SetDesc(_("Displays your Channel Stats"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};

class CommandCSGStats : public Command
{
 public:
	CommandCSGStats(Module *creator) : Command (creator, "chanserv/gstats", 0, 2)
	{
		this->SetDesc(_("Displays your Global Stats"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};


class CSStats;
static CSStats *me;
class CSStats : public Module
{
	CommandCSStats commandcsstats;
	CommandCSGStats commandcsgstats;
	ServiceReference<SQL::Provider> sql;
	MySQLInterface sqlinterface;
	Anope::string prefix;
 public:
	CSStats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsstats(this), commandcsgstats(this), sql("", ""), sqlinterface(this)
	{
		me = this;
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		prefix = config.ReadValue("chanstats", "prefix", "anope_", 0);
		Anope::string engine = config.ReadValue("chanstats", "engine", "", 0);
		this->sql = ServiceReference<SQL::Provider>("SQL::Provider", engine);
	}

	SQL::Result RunQuery(const SQL::Query &query)
	{
		if (!this->sql)
			throw SQL::Exception("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		SQL::Result res = this->sql->RunQuery(query);
		if (!res.GetError().empty())
			throw SQL::Exception(res.GetError());
		return res;
	}

	void DoStats(CommandSource &source, const bool is_global, const std::vector<Anope::string> &params)
	{
		if (!source.c)
			return;

		Anope::string display;
		if (params.empty())
			display = source.nc->display;
		else if (const NickAlias *na = NickAlias::Find(params[0]))
			display = na->nc->display;
		else
		{
			source.Reply(_("%s not found."), params[0].c_str());
			return;
		}

		try
		{
			SQL::Query query;
			query = "SELECT letters, words, line, smileys_happy+smileys_sad+smileys_other as smileys,"
				"actions FROM `" + prefix + "chanstats` "
				"WHERE `nick` = @nick@ AND `chan` = @channel@ AND `type` = 'total';";
			if (is_global)
				query.SetValue("channel", "");
			else
				query.SetValue("channel", source.c->ci->name);
			query.SetValue("nick", display);
			SQL::Result res = this->RunQuery(query);

			if (res.Rows() > 0)
			{
				if (is_global)
					source.Reply(_("Network stats for %s:"), display.c_str());
				else
					source.Reply(_("Channel stats for %s on %s:"), display.c_str(), source.c->name.c_str());

				source.Reply(_("letters: %s, words: %s, lines: %s, smileys %s, actions: %s"),
						res.Get(0, "letters").c_str(), res.Get(0, "words").c_str(),
						res.Get(0, "line").c_str(), res.Get(0, "smileys").c_str(),
						res.Get(0, "actions").c_str());
			}
			else
				source.Reply(_("No stats for %s."), display.c_str());
		}
		catch (const SQL::Exception &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}

	}
};

void CommandCSStats::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoStats(source, false, params);
}

void CommandCSGStats::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoStats(source, true, params);
}

MODULE_INIT(CSStats)

