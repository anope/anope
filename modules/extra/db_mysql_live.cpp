#include "module.h"
#include "async_commands.h"
#include "sql.h"

class SQLCache : public Timer
{
	typedef std::map<Anope::string, time_t, std::less<ci::string> > cache_map;
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

	void Tick(time_t)
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

class ChanInfoUpdater : public SQLInterface, public SQLCache
{
 public:
 	ChanInfoUpdater(Module *m) : SQLInterface(m) { }

	void OnResult(const SQLResult &r)
	{
		ChanInfoUpdater::Process(r);
	}

	static void Process(const SQLResult &res)
	{
		try
		{
			ChannelInfo *ci = cs_findchan(res.Get(0, "name"));
			if (!ci)
				ci = new ChannelInfo(res.Get(0, "name"));
			ci->founder = findcore(res.Get(0, "founder"));
			ci->successor = findcore(res.Get(0, "successor"));
			ci->desc = res.Get(0, "descr");
			ci->time_registered = convertTo<time_t>(res.Get(0, "time_registered"));
			ci->ClearFlags();
			ci->FromString(BuildStringVector(res.Get(0, "flags")));
			ci->forbidby = res.Get(0, "forbidby");
			ci->forbidreason = res.Get(0, "forbidreason");
			ci->bantype = convertTo<int>(res.Get(0, "bantype"));
			ci->memos.memomax = convertTo<unsigned>(res.Get(0, "memomax"));

			Anope::string mlock_on = res.Get(0, "mlock_on"),
					mlock_off = res.Get(0, "mlock_off"),
					mlock_params = res.Get(0, "mlock_params"),
					mlock_params_off = res.Get(0, "mlock_params_off");

			Anope::string mode;
			std::vector<Anope::string> modes;

			spacesepstream sep(mlock_on);
			while (sep.GetToken(mode))
				modes.push_back(mode);
			ci->Extend("db_mlock_modes_on", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));

			modes.clear();
			sep = mlock_off;
			while (sep.GetToken(mode))
				modes.push_back(mode);
			ci->Extend("db_mlock_modes_off", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));

			modes.clear();
			sep = mlock_params;
			while (sep.GetToken(mode))
				modes.push_back(mode);
			ci->Extend("mlock_params", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));

			modes.clear();
			sep = mlock_params_off;
			while (sep.GetToken(mode))
				modes.push_back(mode);
			ci->Extend("mlock_params_off", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));

			ci->LoadMLock();

			if (res.Get(0, "botnick").equals_cs(ci->bi ? ci->bi->nick : "") == false)
			{
				if (ci->bi)
					ci->bi->UnAssign(NULL, ci);
				BotInfo *bi = findbot(res.Get(0, "botnick"));
				if (bi)
					bi->Assign(NULL, ci);
			}

			ci->capsmin = convertTo<int16>(res.Get(0, "capsmin"));
			ci->capspercent = convertTo<int16>(res.Get(0, "capspercent"));
			ci->floodlines = convertTo<int16>(res.Get(0, "floodlines"));
			ci->floodsecs = convertTo<int16>(res.Get(0, "floodsecs"));
			ci->repeattimes = convertTo<int16>(res.Get(0, "repeattimes"));

			if (ci->c)
				check_modes(ci->c);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}
		catch (const ConvertException &) { }
	}
};

class NickInfoUpdater : public SQLInterface, public SQLCache
{
 public:
 	NickInfoUpdater(Module *m) : SQLInterface(m) { }

	void OnResult(const SQLResult &r)
	{
		NickInfoUpdater::Process(r);
	}

	static void Process(const SQLResult &res)
	{
		try
		{
			NickCore *nc = findcore(res.Get(0, "display"));
			if (!nc)
				return;
			NickAlias *na = findnick(res.Get(0, "nick"));
			if (!na)
				na = new NickAlias(res.Get(0, "nick"), nc);

			na->last_quit = res.Get(0, "last_quit");
			na->last_realname = res.Get(0, "last_realname");
			na->last_usermask = res.Get(0, "last_usermask");
			na->time_registered = convertTo<time_t>(res.Get(0, "time_registered"));
			na->last_seen = convertTo<time_t>(res.Get(0, "last_seen"));
			na->ClearFlags();
			na->FromString(BuildStringVector(res.Get(0, "flags")));

			if (na->nc != nc)
			{
				std::list<NickAlias *>::iterator it = std::find(na->nc->aliases.begin(), na->nc->aliases.end(), na);
				if (it != na->nc->aliases.end())
					na->nc->aliases.erase(it);

				na->nc = nc;
				na->nc->aliases.push_back(na);
			}
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}
		catch (const ConvertException &) { }
	}
};

class NickCoreUpdater : public SQLInterface, public SQLCache
{
 public:
 	NickCoreUpdater(Module *m) : SQLInterface(m) { }

	void OnResult(const SQLResult &r)
	{
		NickCoreUpdater::Process(r);
	}

	static void Process(const SQLResult &res)
	{
		try
		{
 			NickCore *nc = findcore(res.Get(0, "display"));
			if (!nc)
				nc = new NickCore(res.Get(0, "display"));

			nc->pass = res.Get(0, "pass");
			nc->email = res.Get(0, "email");
			nc->greet = res.Get(0, "greet");
			nc->ClearFlags();
			nc->FromString(BuildStringVector(res.Get(0, "flags")));
			nc->language = res.Get(0, "language");
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << ex.GetReason();
		}
		catch (const ConvertException &) { }
	}
};

class MySQLLiveModule : public Module
{
	service_reference<SQLProvider> SQL;
	service_reference<AsynchCommandsService> ACS;

	ChanInfoUpdater chaninfoupdater;
	NickInfoUpdater nickinfoupdater;
	NickCoreUpdater nickcoreupdater;

	SQLResult RunQuery(const Anope::string &query)
	{
		if (!this->SQL)
			throw SQLException("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		SQLResult res = SQL->RunQuery(query);
		if (!res.GetError().empty())
			throw SQLException(res.GetError());
		return res;
	}

	void RunQuery(SQLInterface *i, const Anope::string &query)
	{
		if (!this->SQL)
			throw SQLException("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		return SQL->Run(i, query);
	}

	const Anope::string Escape(const Anope::string &query)
	{
		return SQL ? SQL->Escape(query) : query;
	}

	CommandMutex *CurrentCommand()
	{
		if (this->ACS)
			return this->ACS->CurrentCommand();
		return NULL;
	}

 public:
	MySQLLiveModule(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator), SQL("mysql/main"), ACS("asynch_commands"),
		chaninfoupdater(this), nickinfoupdater(this), nickcoreupdater(this)
	{
		Implementation i[] = { I_OnFindChan, I_OnFindNick, I_OnFindCore, I_OnPreShutdown };
		ModuleManager::Attach(i, this, 4);
	}

	void OnPreShutdown()
	{
		Implementation i[] = { I_OnFindChan, I_OnFindNick, I_OnFindCore };
		for (size_t j = 0; j < 3; ++j)
			ModuleManager::Detach(i[j], this);
	}

	void OnFindChan(const Anope::string &chname)
	{
		if (chaninfoupdater.Check(chname))
			return;

		try
		{
			Anope::string query = "SELECT * FROM `anope_cs_info` WHERE `name` = '" + this->Escape(chname) + "'";
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					ChanInfoUpdater::Process(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
				this->RunQuery(&chaninfoupdater, query);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindChan: " << ex.GetReason();
		}
	}

	void OnFindNick(const Anope::string &nick)
	{
		if (nickinfoupdater.Check(nick))
			return;

		try
		{
			Anope::string query = "SELECT * FROM `anope_ns_alias` WHERE `nick` = '" + this->Escape(nick) + "'";
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					NickInfoUpdater::Process(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
				this->RunQuery(&nickinfoupdater, query);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindNick: " << ex.GetReason();
		}
	}

	void OnFindCore(const Anope::string &nick)
	{
		if (nickcoreupdater.Check(nick))
			return;

		try
		{
			Anope::string query = "SELECT * FROM `anope_ns_core` WHERE `display` = '" + this->Escape(nick) + "'";
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					NickCoreUpdater::Process(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
				this->RunQuery(&nickcoreupdater, query);
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindCore: " << ex.GetReason();
		}
	}
};

MODULE_INIT(MySQLLiveModule)
