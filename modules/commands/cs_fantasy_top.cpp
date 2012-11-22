/* Chanstats core functions
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
#include "../extra/sql.h"

class MySQLInterface : public SQLInterface
{
 public:
	MySQLInterface(Module *o) : SQLInterface(o) { }

	void OnResult(const SQLResult &r) anope_override
	{
	}

	void OnError(const SQLResult &r) anope_override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Chanstats: Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Chanstats: Error executing query: " << r.GetError();
	}
};

class CommandCSTop : public Command
{
 public:
	CommandCSTop(Module *creator) : Command (creator, "chanserv/top", 0, 2)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Displays the top 3 users of a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};

class CommandCSTop10 : public Command
{
 public:
	CommandCSTop10(Module *creator) : Command (creator, "chanserv/top10", 0, 2)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Displays the top 10 users of a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};

class CommandCSGTop : public Command
{
 public:
	CommandCSGTop(Module *creator) : Command (creator, "chanserv/gtop", 0, 1)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Displays the top 3 users of the network"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};

class CommandCSGTop10 : public Command
{
 public:
	CommandCSGTop10(Module *creator) : Command (creator, "chanserv/gtop10", 0, 1)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Displays the top 10 users of the network"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);
};


class CSTop;
static CSTop *me;
class CSTop : public Module
{
	CommandCSTop commandcstop;
	CommandCSGTop commandcsgtop;
	CommandCSTop10 commandcstop10;
	CommandCSGTop10 commandcsgtop10;
	ServiceReference<SQLProvider> sql;
	MySQLInterface sqlinterface;
	Anope::string prefix;

 public:
	CSTop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcstop(this), commandcsgtop(this), commandcstop10(this), commandcsgtop10(this), sql("", ""),
		sqlinterface(this)
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
		this->sql = ServiceReference<SQLProvider>("SQLProvider", engine);
	}

	SQLResult RunQuery(const SQLQuery &query)
	{
		if (!this->sql)
			throw SQLException("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		SQLResult res = sql->RunQuery(query);
		if (!res.GetError().empty())
			throw SQLException(res.GetError());
		return res;
	}

	void DoTop(CommandSource &source, const std::vector<Anope::string> &params, bool is_global, int limit = 1)
	{
		if (!source.c || !source.c->ci)
			return;

		Anope::string channel;
		if (is_global || params.empty())
			channel = source.c->ci->name;
		else if (!params.empty())
			channel = params[0];
		else
		{
			source.Reply(_("%s not found."), params[0].c_str());
			return;
		}

		try
		{
			SQLQuery query;
			query = "SELECT nick, letters, words, line, actions,"
				"smileys_happy+smileys_sad+smileys_other as smileys "
				"FROM `" + prefix + "chanstats` "
				"WHERE `nick` != '' AND `chan` = @channel@ AND `type` = 'total' "
				"ORDER BY `letters` DESC LIMIT @limit@;";
			query.setValue("limit", limit, false);

			if (is_global)
				query.setValue("channel", "");
			else
				query.setValue("channel", channel.c_str());

			SQLResult res = this->RunQuery(query);

			if (res.Rows() > 0)
			{
				source.Reply(_("Top %i of %s"), limit, (is_global ? "Network" : channel.c_str()));
				for (int i = 0; i < res.Rows(); ++i)
				{
					source.Reply(_("%2lu \002%-16s\002 letters: %s, words: %s, lines: %s, smileys %s, actions: %s"),
						i+1, res.Get(i, "nick").c_str(), res.Get(i, "letters").c_str(),
						res.Get(i, "words").c_str(), res.Get(i, "line").c_str(), 
						res.Get(0, "smileys").c_str(), res.Get(0, "actions").c_str());
				}
			}
			else
				source.Reply(_("No stats for %s"), is_global ? "Network" : channel.c_str());
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}
	}
};

void CommandCSTop::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoTop(source, params, false, 3);
}

void CommandCSTop10::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoTop(source, params, false, 10);
}

void CommandCSGTop::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoTop(source, params, true, 3);
}

void CommandCSGTop10::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	me->DoTop(source, params, true, 10);
}


MODULE_INIT(CSTop)
