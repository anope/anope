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

class MChanstats : public Module
{
	ServiceReference<SQL::Provider> sql;
	MySQLInterface sqlinterface;
	SQL::Query query;
	Anope::string SmileysHappy, SmileysSad, SmileysOther, prefix;
	std::vector<Anope::string> TableList, ProcedureList, EventList;

	void RunQuery(const SQL::Query &q)
	{
		if (sql)
			sql->Run(&sqlinterface, q);
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

	const Anope::string GetDisplay(User *u)
	{
		if (u && u->Account() && u->Account()->HasExt("STATS"))
			return u->Account()->display;
		else
			return "";
	}

	void GetTables()
	{
		TableList.clear();
		ProcedureList.clear();
		EventList.clear();
		if (!sql)
			return;

		SQL::Result r = this->sql->RunQuery(this->sql->GetTables(prefix));
		for (int i = 0; i < r.Rows(); ++i)
		{
			const std::map<Anope::string, Anope::string> &map = r.Row(i);
			for (std::map<Anope::string, Anope::string>::const_iterator it = map.begin(); it != map.end(); ++it)
				TableList.push_back(it->second);
		}
		query = "SHOW PROCEDURE STATUS WHERE `Db` = Database();";
		r = this->sql->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			ProcedureList.push_back(r.Get(i, "Name"));
		}
		query = "SHOW EVENTS WHERE `Db` = Database();";
		r = this->sql->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			EventList.push_back(r.Get(i, "Name"));
		}
	}

	bool HasTable(const Anope::string &table)
	{
		for (std::vector<Anope::string>::const_iterator it = TableList.begin(); it != TableList.end(); ++it)
			if (*it == table)
				return true;
		return false;
	}

	bool HasProcedure(const Anope::string &table)
	{
		for (std::vector<Anope::string>::const_iterator it = ProcedureList.begin(); it != ProcedureList.end(); ++it)
			if (*it == table)
				return true;
		return false;
	}

	bool HasEvent(const Anope::string &table)
	{
		for (std::vector<Anope::string>::const_iterator it = EventList.begin(); it != EventList.end(); ++it)
			if (*it == table)
				return true;
		return false;
	}


	void CheckTables()
	{
		this->GetTables();
		if (!this->HasTable(prefix +"chanstats"))
		{
			query = "CREATE TABLE `" + prefix + "chanstats` ("
				"`id` int(11) NOT NULL AUTO_INCREMENT,"
				"`chan` varchar(255) NOT NULL DEFAULT '',"
				"`nick` varchar(255) NOT NULL DEFAULT '',"
				"`type` ENUM('total', 'monthly', 'weekly', 'daily') NOT NULL,"
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
				"`time0` int(10) unsigned NOT NULL default '0',"
				"`time1` int(10) unsigned NOT NULL default '0',"
				"`time2` int(10) unsigned NOT NULL default '0',"
				"`time3` int(10) unsigned NOT NULL default '0',"
				"`time4` int(10) unsigned NOT NULL default '0',"
				"`time5` int(10) unsigned NOT NULL default '0',"
				"`time6` int(10) unsigned NOT NULL default '0',"
				"`time7` int(10) unsigned NOT NULL default '0',"
				"`time8` int(10) unsigned NOT NULL default '0',"
				"`time9` int(10) unsigned NOT NULL default '0',"
				"`time10` int(10) unsigned NOT NULL default '0',"
				"`time11` int(10) unsigned NOT NULL default '0',"
				"`time12` int(10) unsigned NOT NULL default '0',"
				"`time13` int(10) unsigned NOT NULL default '0',"
				"`time14` int(10) unsigned NOT NULL default '0',"
				"`time15` int(10) unsigned NOT NULL default '0',"
				"`time16` int(10) unsigned NOT NULL default '0',"
				"`time17` int(10) unsigned NOT NULL default '0',"
				"`time18` int(10) unsigned NOT NULL default '0',"
				"`time19` int(10) unsigned NOT NULL default '0',"
				"`time20` int(10) unsigned NOT NULL default '0',"
				"`time21` int(10) unsigned NOT NULL default '0',"
				"`time22` int(10) unsigned NOT NULL default '0',"
				"`time23` int(10) unsigned NOT NULL default '0',"
				"PRIMARY KEY (`id`),"
				"UNIQUE KEY `chan` (`chan`,`nick`,`type`),"
				"KEY `nick` (`nick`),"
				"KEY `chan_` (`chan`),"
				"KEY `type` (`type`)"
				") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
			this->RunQuery(query);
		}
		/* There is no CREATE OR REPLACE PROCEDURE in MySQL */
		if (this->HasProcedure(prefix + "chanstats_proc_update"))
		{
			query = "DROP PROCEDURE " + prefix + "chanstats_proc_update";
			this->RunQuery(query);
		}
		query = "CREATE PROCEDURE `" + prefix + "chanstats_proc_update`"
			"(chan_ VARCHAR(255), nick_ VARCHAR(255), line_ INT(10), letters_ INT(10),"
			"words_ INT(10), actions_ INT(10), sm_h_ INT(10), sm_s_ INT(10), sm_o_ INT(10),"
			"kicks_ INT(10), kicked_ INT(10), modes_ INT(10), topics_ INT(10))"
			"BEGIN "
				"DECLARE time_ VARCHAR(20);"
				"SET time_ = CONCAT('time', hour(now()));"
				"INSERT IGNORE INTO `" + prefix + "chanstats` (`nick`,`chan`, `type`) VALUES "
					"('', chan_, 'total'), ('', chan_, 'monthly'),"
					"('', chan_, 'weekly'), ('', chan_, 'daily');"
				"IF nick_ != '' THEN "
					"INSERT IGNORE INTO `" + prefix + "chanstats` (`nick`,`chan`, `type`) VALUES "
						"(nick_, chan_, 'total'), (nick_, chan_, 'monthly'),"
						"(nick_, chan_, 'weekly'),(nick_, chan_, 'daily'),"
						"(nick_, '', 'total'), (nick_, '', 'monthly'),"
						"(nick_, '', 'weekly'), (nick_, '', 'daily');"
				"END IF;"
				"SET @update_query = CONCAT('UPDATE `" + prefix + "chanstats` SET line=line+', line_, ',"
				"letters=letters+', letters_, ' , words=words+', words_, ', actions=actions+', actions_, ', "
				"smileys_happy=smileys_happy+', sm_h_, ', smileys_sad=smileys_sad+', sm_s_, ', "
				"smileys_other=smileys_other+', sm_o_, ', kicks=kicks+', kicks_, ', kicked=kicked+', kicked_, ', "
				"modes=modes+', modes_, ', topics=topics+', topics_, ', ', time_ , '=', time_, '+', line_ ,' "
				"WHERE (nick='''' OR nick=''', nick_, ''') AND (chan='''' OR chan=''', chan_, ''')');"
				"PREPARE update_query FROM @update_query;"
				"EXECUTE update_query;"
				"DEALLOCATE PREPARE update_query;"
			"END";
		this->RunQuery(query);

		if (this->HasProcedure(prefix + "chanstats_proc_chgdisplay"))
		{
			query = "DROP PROCEDURE " + prefix + "chanstats_proc_chgdisplay;";
			this->RunQuery(query);
		}
		query = "CREATE PROCEDURE `" + prefix + "chanstats_proc_chgdisplay`"
			"(old_nick varchar(255), new_nick varchar(255))"
			"BEGIN "
			"DECLARE res_count int(10) unsigned;"
			"SELECT COUNT(nick) INTO res_count FROM `" + prefix + "chanstats` WHERE nick = new_nick;"
			"IF res_count = 0 THEN "
				"UPDATE `" + prefix + "chanstats` SET `nick` = new_nick WHERE `nick` = old_nick;"
			"ELSE "
			"my_cursor: BEGIN "
				"DECLARE no_more_rows BOOLEAN DEFAULT FALSE;"
				"DECLARE chan_ VARCHAR(255);"
				"DECLARE type_ ENUM('total', 'monthly', 'weekly', 'daily');"
				"DECLARE letters_, words_, line_, actions_, smileys_happy_,"
					"smileys_sad_, smileys_other_, kicks_, kicked_, modes_, topics_,"
					"time0_, time1_, time2_, time3_, time4_, time5_, time6_, time7_, time8_, time9_,"
					"time10_, time11_, time12_, time13_, time14_, time15_, time16_, time17_, time18_,"
					"time19_, time20_, time21_, time22_, time23_ INT(10) unsigned;"
				"DECLARE stats_cursor CURSOR FOR "
					"SELECT chan, type, letters, words, line, actions, smileys_happy,"
						"smileys_sad, smileys_other, kicks, kicked, modes, topics, time0, time1,"
						"time2, time3, time4, time5, time6, time7, time8, time9, time10, time11,"
						"time12, time13, time14, time15, time16, time17, time18, time19, time20,"
						"time21, time22, time23 "
					"FROM `" + prefix + "chanstats` "
					"WHERE `nick` = old_nick;"
				"DECLARE CONTINUE HANDLER FOR NOT FOUND "
					"SET no_more_rows = TRUE;"
				"OPEN stats_cursor;"
				"the_loop: LOOP "
					"FETCH stats_cursor "
					"INTO chan_, type_, letters_, words_, line_, actions_, smileys_happy_,"
						"smileys_sad_, smileys_other_, kicks_, kicked_, modes_, topics_,"
						"time0_, time1_, time2_, time3_, time4_, time5_, time6_, time7_, time8_,"
						"time9_, time10_, time11_, time12_, time13_, time14_, time15_, time16_,"
						"time17_, time18_, time19_, time20_, time21_, time22_, time23_;"
					"IF no_more_rows THEN "
						"CLOSE stats_cursor;"
						"LEAVE the_loop;"
					"END IF;"
					"INSERT INTO `" + prefix + "chanstats` "
						"(chan, nick, type, letters, words, line, actions, smileys_happy, "
						"smileys_sad, smileys_other, kicks, kicked, modes, topics, time0, time1, "
						"time2, time3, time4, time5, time6, time7, time8, time9, time10, time11,"
						"time12, time13, time14, time15, time16, time17, time18, time19, time20,"
						"time21, time22, time23)"
					"VALUES (chan_, new_nick, type_, letters_, words_, line_, actions_, smileys_happy_,"
						"smileys_sad_, smileys_other_, kicks_, kicked_, modes_, topics_,"
						"time0_, time1_, time2_, time3_, time4_, time5_, time6_, time7_, time8_, "
						"time9_, time10_, time11_, time12_, time13_, time14_, time15_, time16_, "
						"time17_, time18_, time19_, time20_, time21_, time22_, time23_)"
					"ON DUPLICATE KEY UPDATE letters=letters+VALUES(letters), words=words+VALUES(words),"
						"line=line+VALUES(line), actions=actions+VALUES(actions),"
						"smileys_happy=smileys_happy+VALUES(smileys_happy),"
						"smileys_sad=smileys_sad+VALUES(smileys_sad),"
						"smileys_other=smileys_other+VALUES(smileys_other),"
						"kicks=kicks+VALUES(kicks), kicked=kicked+VALUES(kicked),"
						"modes=modes+VALUES(modes), topics=topics+VALUES(topics),"
						"time1=time1+VALUES(time1), time2=time2+VALUES(time2), time3=time3+VALUES(time3),"
						"time4=time4+VALUES(time4), time5=time5+VALUES(time5), time6=time6+VALUES(time6),"
						"time7=time7+VALUES(time7), time8=time8+VALUES(time8), time9=time9+VALUES(time9),"
						"time10=time10+VALUES(time10), time11=time11+VALUES(time11), time12=time12+VALUES(time12),"
						"time13=time13+VALUES(time13), time14=time14+VALUES(time14), time15=time15+VALUES(time15),"
						"time16=time16+VALUES(time16), time17=time17+VALUES(time17), time18=time18+VALUES(time18),"
						"time19=time19+VALUES(time19), time20=time20+VALUES(time20), time21=time21+VALUES(time21),"
						"time22=time22+VALUES(time22), time23=time23+VALUES(time23);"
				"END LOOP;"
				"DELETE FROM `" + prefix + "chanstats` WHERE `nick` = old_nick;"
			"END my_cursor;"
			"END IF;"
			"END;";
		this->RunQuery(query);

		/* dont prepend any database prefix to events so we can always delete/change old events */
		if (this->HasEvent("chanstats_event_cleanup_daily"))
		{
			query = "DROP EVENT chanstats_event_cleanup_daily";
			this->RunQuery(query);
		}
		query = "CREATE EVENT `chanstats_event_cleanup_daily` "
			"ON SCHEDULE EVERY 1 DAY STARTS CURRENT_DATE "
			"DO UPDATE `" + prefix + "chanstats` SET letters=0, words=0, line=0, actions=0, smileys_happy=0,"
				"smileys_sad=0, smileys_other=0, kicks=0, modes=0, topics=0, time0=0, time1=0, time2=0,"
				"time3=0, time4=0, time5=0, time6=0, time7=0, time8=0, time9=0, time10=0, time11=0,"
				"time12=0, time13=0, time14=0, time15=0, time16=0, time17=0, time18=0, time19=0,"
				"time20=0, time21=0, time22=0, time23=0 "
			"WHERE type='daily';";
		this->RunQuery(query);

		if (this->HasEvent("chanstats_event_cleanup_weekly"))
		{
			query = "DROP EVENT `chanstats_event_cleanup_weekly`";
			this->RunQuery(query);
		}
		query = "CREATE EVENT `chanstats_event_cleanup_weekly` "
			"ON SCHEDULE EVERY 1 WEEK STARTS (DATE(CURRENT_TIMESTAMP)-WEEKDAY(CURRENT_TIMESTAMP)) "
			"DO UPDATE `" + prefix + "chanstats` SET letters=0, words=0, line=0, actions=0, smileys_happy=0,"
				"smileys_sad=0, smileys_other=0, kicks=0, modes=0, topics=0, time0=0, time1=0, time2=0,"
				"time3=0, time4=0, time5=0, time6=0, time7=0, time8=0, time9=0, time10=0, time11=0,"
				"time12=0, time13=0, time14=0, time15=0, time16=0, time17=0, time18=0, time19=0,"
				"time20=0, time21=0, time22=0, time23=0 "
			"WHERE type='weekly';";
		this->RunQuery(query);

		if (this->HasEvent("chanstats_event_cleanup_monthly"))
		{
			query = "DROP EVENT `chanstats_event_cleanup_monthly`;";
			this->RunQuery(query);
		}
		query = "CREATE EVENT `chanstats_event_cleanup_monthly` "
			"ON SCHEDULE EVERY 1 MONTH STARTS LAST_DAY(CURRENT_TIMESTAMP) + INTERVAL 1 DAY "
			"DO BEGIN "
			"UPDATE `" + prefix + "chanstats` SET letters=0, words=0, line=0, actions=0, smileys_happy=0,"
				"smileys_sad=0, smileys_other=0, kicks=0, modes=0, topics=0, time0=0, time1=0, time2=0,"
				"time3=0, time4=0, time5=0, time6=0, time7=0, time8=0, time9=0, time10=0, time11=0,"
				"time12=0, time13=0, time14=0, time15=0, time16=0, time17=0, time18=0, time19=0, "
				"time20=0, time21=0, time22=0, time23=0 "
			"WHERE type='monthly';"
			"OPTIMIZE TABLE `" + prefix + "chanstats`;"
			"END;";
		this->RunQuery(query);
	}


 public:
	MChanstats(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, EXTRA | VENDOR), sql("", ""), sqlinterface(this)
	{

		Implementation i[] = {	I_OnPrivmsg,
					I_OnUserKicked,
					I_OnChannelModeSet,
					I_OnChannelModeUnset,
					I_OnTopicUpdated,
					I_OnDelCore,
					I_OnChangeCoreDisplay,
					I_OnChanDrop,
					I_OnReload};
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnReload(ServerConfig *conf, ConfigReader &reader) anope_override
	{
		prefix = reader.ReadValue("chanstats", "prefix", "anope_", 0);
		SmileysHappy = reader.ReadValue("chanstats", "SmileysHappy", ":) :-) ;) :D :-D", 0);
		SmileysSad = reader.ReadValue("chanstats", "SmileysSad", ":( :-( ;( ;-(", 0);
		SmileysOther = reader.ReadValue("chanstats", "SmileysOther", ":/", 0);

		Anope::string engine = reader.ReadValue("chanstats", "engine", "", 0);
		this->sql = ServiceReference<SQL::Provider>("SQL::Provider", engine);
		if (sql)
			this->CheckTables();
		else
			Log(this) << "no database connection to " << engine;
	}

	void OnTopicUpdated(Channel *c, const Anope::string &user, const Anope::string &topic) anope_override
	{
		User *u = User::Find(user);
		if (!u || !u->Account() || !c->ci || !c->ci->HasExt("STATS"))
			return;
		query = "CALL " + prefix + "chanstats_proc_update(@channel@, @nick@, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);";
		query.SetValue("channel", c->name);
		query.SetValue("nick", GetDisplay(u));
		this->RunQuery(query);
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, const Anope::string &, const Anope::string &param) anope_override
	{
		this->OnModeChange(c, setter.GetUser());
		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, MessageSource &setter, const Anope::string &, const Anope::string &param) anope_override
	{
		this->OnModeChange(c, setter.GetUser());
		return EVENT_CONTINUE;
	}

 private:
	void OnModeChange(Channel *c, User *u)
	{
		if (!u || !u->Account() || !c->ci || !c->ci->HasExt("STATS"))
			return;

		query = "CALL " + prefix + "chanstats_proc_update(@channel@, @nick@, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);";
		query.SetValue("channel", c->name);
		query.SetValue("nick", GetDisplay(u));
		this->RunQuery(query);
	}
 public:
	void OnUserKicked(Channel *c, User *target, MessageSource &source, const Anope::string &kickmsg) anope_override
	{
		if (!c->ci || !c->ci->HasExt("STATS"))
			return;

		query = "CALL " + prefix + "chanstats_proc_update(@channel@, @nick@, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);";
		query.SetValue("channel", c->name);
		query.SetValue("nick", GetDisplay(target));
		this->RunQuery(query);

		query = "CALL " + prefix + "chanstats_proc_update(@channel@, @nick@, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);";
		query.SetValue("channel", c->name);
		query.SetValue("nick", GetDisplay(source.GetUser()));
		this->RunQuery(query);
	}
	void OnPrivmsg(User *u, Channel *c, Anope::string &msg) anope_override
	{
		if (!c->ci || !c->ci->HasExt("STATS") || (msg[0] == Config->BSFantasyCharacter[0]))
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
		query = "CALL " + prefix + "chanstats_proc_update(@channel@, @nick@, 1, @letters@, @words@, @action@, "
		"@smileys_happy@, @smileys_sad@, @smileys_other@, '0', '0', '0', '0');";
		query.SetValue("channel", c->name);
		query.SetValue("nick", GetDisplay(u));
		query.SetValue("letters", letters);
		query.SetValue("words", words);
		query.SetValue("action", action);
		query.SetValue("smileys_happy", smileys_happy);
		query.SetValue("smileys_sad", smileys_sad);
		query.SetValue("smileys_other", smileys_other);
		this->RunQuery(query);
	}
	void OnDelCore(NickCore *nc) anope_override
	{
		query = "DELETE FROM `" + prefix + "chanstats` WHERE `nick` = @nick@;";
		query.SetValue("nick", nc->display);
		this->RunQuery(query);
	}
	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay) anope_override
	{
		query = "CALL " + prefix + "chanstats_proc_chgdisplay(@old_display@, @new_display@);";
		query.SetValue("old_display", nc->display);
		query.SetValue("new_display", newdisplay);
		this->RunQuery(query);
	}
	void OnChanDrop(ChannelInfo *ci) anope_override
	{
		query = "DELETE FROM `" + prefix + "chanstats` WHERE `chan` = @channel@;";
		query.SetValue("channel", ci->name);
		this->RunQuery(query);
	}
};

MODULE_INIT(MChanstats)

