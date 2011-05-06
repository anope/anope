#include "module.h"
#include "../extra/async_commands.h"
#include "../extra/sql.h"

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

static void ChanInfoUpdate(const SQLResult &res)
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
		ci->bantype = convertTo<int>(res.Get(0, "bantype"));
		ci->memos.memomax = convertTo<unsigned>(res.Get(0, "memomax"));

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

static void NickInfoUpdate(const SQLResult &res)
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

static void NickCoreUpdate(const SQLResult &res)
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

class MySQLLiveModule : public Module
{
	service_reference<SQLProvider> SQL;
	service_reference<AsynchCommandsService> ACS;

	SQLCache chan_cache, nick_cache, core_cache;

	SQLResult RunQuery(const SQLQuery &query)
	{
		if (!this->SQL)
			throw SQLException("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		SQLResult res = SQL->RunQuery(query);
		if (!res.GetError().empty())
			throw SQLException(res.GetError());
		return res;
	}

	CommandMutex *CurrentCommand()
	{
		if (this->ACS)
			return this->ACS->CurrentCommand();
		return NULL;
	}

 public:
	MySQLLiveModule(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, DATABASE), SQL("mysql/main"), ACS("asynch_commands")
	{
		Implementation i[] = { I_OnFindChan, I_OnFindNick, I_OnFindCore, I_OnShutdown };
		ModuleManager::Attach(i, this, 4);
	}

	void OnShutdown()
	{
		Implementation i[] = { I_OnFindChan, I_OnFindNick, I_OnFindCore };
		for (size_t j = 0; j < 3; ++j)
			ModuleManager::Detach(i[j], this);
	}

	void OnFindChan(const Anope::string &chname)
	{
		if (chan_cache.Check(chname))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `anope_cs_info` WHERE `name` = ?");
			query.setValue(1, chname);
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					ChanInfoUpdate(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
			{
				SQLResult res = this->RunQuery(query);
				ChanInfoUpdate(res);
			}
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindChan: " << ex.GetReason();
		}
	}

	void OnFindNick(const Anope::string &nick)
	{
		if (nick_cache.Check(nick))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `anope_ns_alias` WHERE `nick` = ?");
			query.setValue(1, nick);
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					NickInfoUpdate(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
			{
				SQLResult res = this->RunQuery(query);
				NickInfoUpdate(res);
			}
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindNick: " << ex.GetReason();
		}
	}

	void OnFindCore(const Anope::string &nick)
	{
		if (core_cache.Check(nick))
			return;

		try
		{
			SQLQuery query("SELECT * FROM `anope_ns_core` WHERE `display` = ?");
			query.setValue(1, nick);
			CommandMutex *current_command = this->CurrentCommand();
			if (current_command)
			{
				current_command->Unlock();
				try
				{
					SQLResult res = this->RunQuery(query);
					current_command->Lock();
					NickCoreUpdate(res);
				}
				catch (const SQLException &)
				{
					current_command->Lock();
					throw;
				}
			}
			else
			{
				SQLResult res = this->RunQuery(query);
				NickCoreUpdate(res);
			}
		}
		catch (const SQLException &ex)
		{
			Log(LOG_DEBUG) << "OnFindCore: " << ex.GetReason();
		}
	}
};

MODULE_INIT(MySQLLiveModule)
