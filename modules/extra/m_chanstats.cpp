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

class MChanstats : public Module
{
	service_reference<SQLProvider> sql;
	MySQLInterface sqlinterface;
	SQLQuery query;
	Anope::string SmileysHappy, SmileysSad, SmileysOther;
	std::vector<Anope::string> TableList;

	void RunQuery(const SQLQuery &q)
	{
		if (sql)
			sql->Run(&sqlinterface, q);
	}

	void SetQuery()
	{
		query = "INSERT DELAYED INTO `anope_bs_chanstats` (chanserv_name, nickserv_display, day, hour, @what@) "
			"VALUES (@chanserv_name@, @nickserv_display@, CURRENT_DATE, HOUR(NOW()), '1') "
			"ON DUPLICATE KEY UPDATE @what@=@what@+1;";
	}
	size_t CountWords(const Anope::string &msg)
	{
		size_t words = 0;
		for (size_t pos = 0; pos != Anope::string::npos; pos = msg.find(" ", pos+1))
			words++;
		return words;
	}
	size_t CountSmileys(const Anope::string &msg, const Anope::string &smileylist)
	{
		size_t smileys = 0;
		spacesepstream sep(smileylist);
		Anope::string buf;

		while (sep.GetToken(buf) && !buf.empty())
		{
			for (size_t pos = msg.find(buf, 0); pos != Anope::string::npos; pos = msg.find(buf, pos+1))
				smileys++;
		}
		return smileys;
	}

	void GetTables()
	{
		TableList.clear();
		SQLResult r = this->sql->RunQuery(this->sql->GetTables());
		for (int i = 0; i < r.Rows(); ++i)
		{
			const std::map<Anope::string, Anope::string> &map = r.Row(i);
			for (std::map<Anope::string, Anope::string>::const_iterator it = map.begin(); it != map.end(); ++it)
				TableList.push_back(it->second);
		}
	}

	bool HasTable(const Anope::string &table)
	{
		for (std::vector<Anope::string>::const_iterator it = TableList.begin(); it != TableList.end(); ++it)
			if (*it == table)
				return true;
		return false;
	}

	void CheckTables()
	{
		this->GetTables();
		if (!this->HasTable("anope_bs_chanstats"))
		{
			query = "CREATE TABLE `anope_bs_chanstats` ("
				"`id` int(11) NOT NULL AUTO_INCREMENT,"
				"`chanserv_name` varchar(255) NOT NULL DEFAULT '',"
				"`nickserv_display` varchar(255) NOT NULL DEFAULT '',"
				"`day` date NOT NULL,"
				"`hour` tinyint(2) NOT NULL,"
				"`letters` int(10) unsigned NOT NULL DEFAULT '0',"
				"`words` int(10) unsigned NOT NULL DEFAULT '0',"
				"`line` int(10) unsigned NOT NULL DEFAULT '0',"
				"`actions` int(10) unsigned NOT NULL DEFAULT '0',"
				"`smileys_happy` int(10) unsigned NOT NULL DEFAULT '0',"
				"`smileys_sad` int(10) unsigned NOT NULL DEFAULT '0',"
				"`smileys_other` int(10) unsigned NOT NULL DEFAULT '0',"
				"`kicks` int(10) unsigned NOT NULL DEFAULT '0',"
				"`kicked` int(10) unsigned NOT NULL DEFAULT '0',"
				"`modes` int(10) unsigned NOT NULL DEFAULT '0',"
				"`topics` int(10) unsigned NOT NULL DEFAULT '0',"
				"PRIMARY KEY (`id`),"
				"UNIQUE KEY `chanserv_name` (`chanserv_name`,`nickserv_display`,`day`,`hour`),"
				"KEY `nickserv_display` (`nickserv_display`), "
				"KEY `day` (`day`),"
				"KEY `hour` (`hour`)"
				") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
			this->RunQuery(query);
		}
		if (!this->HasTable("anope_bs_chanstats_view_sum_all"))
		{
			query = "CREATE OR REPLACE VIEW `anope_bs_chanstats_view_sum_all` AS "
				"SELECT `anope_bs_chanstats`.`chanserv_name` AS `chanserv_name`,"
				"`anope_bs_chanstats`.`nickserv_display` AS `nickserv_display`,"
				"sum(`anope_bs_chanstats`.`letters`) AS `letters`,"
				"sum(`anope_bs_chanstats`.`words`) AS `words`,"
				"sum(`anope_bs_chanstats`.`line`) AS `line`,"
				"sum(`anope_bs_chanstats`.`actions`) AS `actions`,"
				"((sum(`anope_bs_chanstats`.`smileys_happy`) "
				"+ sum(`anope_bs_chanstats`.`smileys_sad`)) "
				"+ sum(`anope_bs_chanstats`.`smileys_other`)) AS `smileys`,"
				"sum(`anope_bs_chanstats`.`smileys_happy`) AS `smileys_happy`,"
				"sum(`anope_bs_chanstats`.`smileys_sad`) AS `smileys_sad`,"
				"sum(`anope_bs_chanstats`.`smileys_other`) AS `smileys_other`,"
				"sum(`anope_bs_chanstats`.`kicks`) AS `kicks`,"
				"sum(`anope_bs_chanstats`.`kicked`) AS `kicked`,"
				"sum(`anope_bs_chanstats`.`modes`) AS `modes`,"
				"sum(`anope_bs_chanstats`.`topics`) AS `topics` "
				"FROM `anope_bs_chanstats` "
				"GROUP BY `anope_bs_chanstats`.`chanserv_name`,`anope_bs_chanstats`.`nickserv_display`;";
				this->RunQuery(query);
		}
	}


 public:
	MChanstats(const Anope::string &modname, const Anope::string &creator) : 
		Module(modname, creator, CORE), sql("", ""), sqlinterface(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = {	I_OnPrivmsg,
					I_OnUserKicked,
					I_OnChannelModeSet,
					I_OnChannelModeUnset,
					I_OnTopicUpdated,
					I_OnReload};
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		this->OnReload();
	}
	void OnReload() anope_override
	{
		ConfigReader config;
		SmileysHappy = config.ReadValue("chanstats", "SmileysHappy", ":) :-) ;) :D :-D", 0);
		SmileysSad = config.ReadValue("chanstats", "SmileysSad", ":( :-( ;( ;-(", 0);
		SmileysOther = config.ReadValue("chanstats", "SmileysOther", ":/", 0);

		Anope::string engine = config.ReadValue("chanstats", "engine", "", 0);
		this->sql = service_reference<SQLProvider>("SQLProvider", engine);
		this->CheckTables();

	}
	void OnTopicUpdated(Channel *c, User *u, const Anope::string &topic) anope_override
	{
		if (!u || !u->Account() || !c->ci || !c->ci->HasFlag(CI_STATS))
			return;

		bool has_display = u->Account()->HasFlag(NI_STATS);

		this->SetQuery();
		query.setValue("chanserv_name", c->name);
		query.setValue("nickserv_display", has_display ? u->Account()->display : "");
		query.setValue("what", "topics", false);
		this->RunQuery(query);
	}
	EventReturn OnChannelModeSet(Channel *c, User *setter, ChannelModeName Name, const Anope::string &param) anope_override
	{
		this->OnModeChange(c, setter);
		return EVENT_CONTINUE;
	}
	EventReturn OnChannelModeUnset(Channel *c, User *setter, ChannelModeName Name, const Anope::string &param) anope_override
	{
		this->OnModeChange(c, setter);
		return EVENT_CONTINUE;
	}
	void OnModeChange(Channel *c, User *u)
	{
		if (!u || !u->Account() || !c->ci || !c->ci->HasFlag(CI_STATS))
			return;

		bool has_display = u->Account()->HasFlag(NI_STATS);

		this->SetQuery();
		query.setValue("chanserv_name", c->name);
		query.setValue("nickserv_display", has_display ? u->Account()->display : "");
		query.setValue("what", "modes", false);
		this->RunQuery(query);
	}
	void OnUserKicked(Channel *c, User *target, const Anope::string &source, const Anope::string &kickmsg) anope_override
	{
		if (!c->ci || !c->ci->HasFlag(CI_STATS))
			return;


		bool has_display = target && target->Account() && target->Account()->HasFlag(NI_STATS);

		this->SetQuery();
		query.setValue("chanserv_name", c->name);
		query.setValue("nickserv_display", has_display ? target->Account()->display : "");
		query.setValue("what", "kicked", false);
		this->RunQuery(query);

		User *kicker = finduser(source);
		has_display = kicker && kicker->Account() && kicker->Account()->HasFlag(NI_STATS);

		this->SetQuery();
		query.setValue("chanserv_name", c->name);
		query.setValue("nickserv_display", has_display ? kicker->Account()->display : "");
		query.setValue("what", "kicks", false);
		this->RunQuery(query);
	}
	void OnPrivmsg(User *u, Channel *c, Anope::string &msg) anope_override
	{
		if (!c->ci || !c->ci->HasFlag(CI_STATS) || (msg[0] == Config->BSFantasyCharacter[0]))
			return;

		size_t letters = msg.length();
		size_t words = this->CountWords(msg);

		size_t action = 0;
		if (msg.find("\01ACTION")!=Anope::string::npos)
		{
			action = 1;
			letters = letters - 7;
			words--;
		}

		// count smileys
		size_t smileys_happy = CountSmileys(msg, SmileysHappy);
		size_t smileys_sad = CountSmileys(msg, SmileysSad);
		size_t smileys_other = CountSmileys(msg, SmileysOther);

		// do not count smileys as words
		words = words - smileys_happy - smileys_sad - smileys_other;
		bool has_display = u && u->Account() && u->Account()->HasFlag(NI_STATS);
		query = "INSERT DELAYED INTO `anope_bs_chanstats` (chanserv_name, nickserv_display, day, hour, letters, words, line, actions, smileys_happy, smileys_sad, smileys_other) "
			"VALUES (@chanserv_name@, @nickserv_display@, CURRENT_DATE, HOUR(NOW()), @letters@, @words@, 1, @actions@, @smileys_happy@, @smileys_sad@, @smileys_other@) "
			"ON DUPLICATE KEY UPDATE letters=letters+VALUES(letters), words=words+VALUES(words), line=line+1, actions=actions+VALUES(actions), "
			"smileys_happy=smileys_happy+VALUES(smileys_happy), smileys_sad=smileys_sad+VALUES(smileys_sad), smileys_other=smileys_other+VALUES(smileys_other);";
		query.setValue("nickserv_display", has_display ? u->Account()->display : "");
		query.setValue("chanserv_name", c->name);
		query.setValue("letters", letters);
		query.setValue("words", words);
		query.setValue("actions", action);
		query.setValue("smileys_happy", smileys_happy);
		query.setValue("smileys_sad", smileys_sad);
		query.setValue("smileys_other", smileys_other);
		this->RunQuery(query);
	}
};



MODULE_INIT(MChanstats)

