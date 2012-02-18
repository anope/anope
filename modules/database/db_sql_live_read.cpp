#include "module.h"
#include "../extra/sql.h"

class SQLCache : public Timer
{
	typedef std::map<Anope::string, time_t, ci::less> cache_map;
	cache_map cache;
 public:

	SQLCache() : Timer(300, Anope::CurTime, true) { }

	bool Check(const Anope::string &item)
	{
		cache_map::iterator it = this->cache.find(item);
		if (it != this->cache.end() && Anope::CurTime - it->second < 5)
			return true;

		this->cache[item] = Anope::CurTime;
		return false;
	}

	void Tick(time_t) anope_override
	{
		for (cache_map::iterator it = this->cache.begin(), next_it; it != this->cache.end(); it = next_it)
		{
			next_it = it;
			++next_it;

			if (Anope::CurTime - it->second > 5)
				this->cache.erase(it);
		}
	}
};

class MySQLLiveModule : public Module
{
	service_reference<SQLProvider> SQL;

	SQLCache chan_cache, nick_cache, core_cache;

	SQLResult RunQuery(const SQLQuery &query)
	{
		if (!this->SQL)
			throw SQLException("Unable to locate SQL reference, is db_sql loaded and configured correctly?");

		SQLResult res = SQL->RunQuery(query);
		if (!res.GetError().empty())
			throw SQLException(res.GetError());
		return res;
	}

 public:
	MySQLLiveModule(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, DATABASE), SQL("", "")
	{
		this->OnReload();

		Implementation i[] = { I_OnReload, I_OnFindChan, I_OnFindNick, I_OnFindCore, I_OnShutdown };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->SQL = service_reference<SQLProvider>("SQLProvider", engine);
	}

	void OnShutdown() anope_override
	{
		Implementation i[] = { I_OnFindChan, I_OnFindNick, I_OnFindCore };
		for (size_t j = 0; j < 3; ++j)
			ModuleManager::Detach(i[j], this);
	}

	void OnFindChan(const Anope::string &chname) anope_override
	{
		if (chan_cache.Check(chname))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `ChannelInfo` WHERE `name` = @name@");
			query.setValue("name", chname);
			SQLResult res = this->RunQuery(query);

			if (res.Rows() == 0)
			{
				delete cs_findchan(chname);
				return;
			}
			
			SerializeType *sb = SerializeType::Find("ChannelInfo");
			if (sb == NULL)
				return;

			Serializable::serialized_data data;
			const std::map<Anope::string, Anope::string> &row = res.Row(0);
			for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
				data[rit->first] << rit->second;

			sb->Create(data);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindChan: " << ex.GetReason();
		}
	}

	void OnFindNick(const Anope::string &nick) anope_override
	{
		if (nick_cache.Check(nick))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `NickAlias` WHERE `nick` = @nick@");
			query.setValue("nick", nick);
			SQLResult res = this->RunQuery(query);

			if (res.Rows() == 0)
			{
				delete findnick(nick);
				return;
			}

			SerializeType *sb = SerializeType::Find("NickAlias");
			if (sb == NULL)
				return;

			Serializable::serialized_data data;
			const std::map<Anope::string, Anope::string> &row = res.Row(0);
			for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
				data[rit->first] << rit->second;

			sb->Create(data);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindNick: " << ex.GetReason();
		}
	}

	void OnFindCore(const Anope::string &nick) anope_override
	{
		if (core_cache.Check(nick))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `NickCore` WHERE `display` = @display@");
			query.setValue("display", nick);
			SQLResult res = this->RunQuery(query);

			if (res.Rows() == 0)
			{
				delete findcore(nick);
				return;
			}

			SerializeType *sb = SerializeType::Find("NickCore");
			if (sb == NULL)
				return;

			Serializable::serialized_data data;
			const std::map<Anope::string, Anope::string> &row = res.Row(0);
			for (std::map<Anope::string, Anope::string>::const_iterator rit = row.begin(), rit_end = row.end(); rit != rit_end; ++rit)
				data[rit->first] << rit->second;

			sb->Create(data);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindCore: " << ex.GetReason();
		}
	}
};

MODULE_INIT(MySQLLiveModule)
