#include "module.h"
#include "sql.h"

class CommandMutex;
static std::list<CommandMutex *> commands;
static CommandMutex *current_command = NULL;

class CommandMutex : public Thread
{
 public:
	Mutex mutex;
	Command *command;
	CommandSource source;
	std::vector<Anope::string> params;

	CommandMutex() : Thread()
	{
		commands.push_back(this);
		current_command = this;
	}

	~CommandMutex()
	{
		std::list<CommandMutex *>::iterator it = std::find(commands.begin(), commands.end(), this);
		if (it != commands.end())
			commands.erase(it);
		if (this == current_command)
			current_command = NULL;
	}

	void Run()
	{
		User *u = this->source.u;
		BotInfo *bi = this->source.owner;

		if (!command->permission.empty() && !u->Account()->HasCommand(command->permission))
		{
			u->SendMessage(bi, ACCESS_DENIED);
			Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command;
		}
		else
		{
			CommandReturn ret = command->Execute(source, params);

			if (ret == MOD_CONT)
			{
				FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, command, params));
			}
		}

		this->mutex.Unlock();
	}
};

class MySQLLiveModule : public Module, public Pipe
{
	service_reference<SQLProvider> SQL;

	SQLResult RunQuery(const Anope::string &query)
	{
		if (!this->SQL)
			throw SQLException("Unable to locate SQL reference, is m_mysql loaded and configured correctly?");

		return SQL->RunQuery(query);
	}

	const Anope::string Escape(const Anope::string &query)
	{
		return SQL ? SQL->Escape(query) : query;
	}

 public:
	MySQLLiveModule(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), SQL("mysql/main")
	{
		Implementation i[] = { I_OnPreCommand, I_OnFindBot, I_OnFindChan, I_OnFindNick, I_OnFindCore };
		ModuleManager::Attach(i, this, 5);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		CommandMutex *cm = new CommandMutex();
		try
		{
			cm->mutex.Lock();
			cm->command = command;
			cm->source = source;
			cm->params = params;

			commands.push_back(cm);

			// Give processing to the command thread
			Log(LOG_DEBUG_2) << "db_mysql_live: Waiting for command thread " << cm->command->name << " from " << source.u->nick;
			threadEngine.Start(cm);
			cm->mutex.Lock();
		}
		catch (const CoreException &ex)
		{
			delete cm;
			Log() << "db_mysql_live: Unable to thread for command: " << ex.GetReason();

			return EVENT_CONTINUE;
		}

		return EVENT_STOP;
	}

	void OnNotify()
	{
		for (std::list<CommandMutex *>::iterator it = commands.begin(), it_end = commands.end(); it != it_end; ++it)
		{
			CommandMutex *cm  = *it;

			// Thread engine will pick this up later
			if (cm->GetExitState())
				continue;

			Log(LOG_DEBUG_2) << "db_mysql_live: Waiting for command thread " << cm->command->name << " from " << cm->source.u->nick;
			current_command = cm;

			// Unlock to give processing back to the command thread
			cm->mutex.Unlock();
			// Relock to regain processing once the command thread hangs for any reason
			cm->mutex.Lock();

			current_command = NULL;
		}
	}


	void OnFindBot(const Anope::string &nick)
	{
		if (!current_command)
			return;

		CommandMutex *cm = current_command;

		// Give it back to the core
		cm->mutex.Unlock();
		SQLResult res = this->RunQuery("SELECT * FROM `anope_bs_core` WHERE `nick` = '" + this->Escape(nick) + "'");
		// And take it back...
		this->Notify();
		cm->mutex.Lock();

		try
		{
			current_command = NULL;
			BotInfo *bi = findbot(res.Get(0, "nick"));
			if (!bi)
				bi = new BotInfo(res.Get(0, "nick"), res.Get(0, "user"), res.Get(0, "host"), res.Get(0, "rname"));
			else
			{
				bi->SetIdent(res.Get(0, "user"));
				bi->host = res.Get(0, "host");
				bi->realname = res.Get(0, "rname");
			}

			if (res.Get(0, "flags").equals_cs("PRIVATE"))
				bi->SetFlag(BI_PRIVATE);
			bi->created = convertTo<time_t>(res.Get(0, "created"));
			bi->chancount = convertTo<uint32>(res.Get(0, "chancount"));
		}
		catch (const SQLException &) { }
		catch (const ConvertException &) { }
	}

	void OnFindChan(const Anope::string &chname)
	{
		if (!current_command)
			return;

		CommandMutex *cm = current_command;

		cm->mutex.Unlock();
		SQLResult res = this->RunQuery("SELECT * FROM `anope_cs_info` WHERE `name` = '" + this->Escape(chname) + "'");
		this->Notify();
		cm->mutex.Lock();

		try
		{
			current_command = NULL;
			ChannelInfo *ci = cs_findchan(res.Get(0, "name"));
			if (!ci)
				ci = new ChannelInfo(res.Get(0, "name"));
			ci->founder = findcore(res.Get(0, "founder"));
			ci->successor = findcore(res.Get(0, "successor"));
			ci->desc = res.Get(0, "descr");
			ci->time_registered = convertTo<time_t>(res.Get(0, "time_registered"));
			// XXX flags, we need ChannelInfo::ProcessFlags or similar?
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
		catch (const SQLException &) { }
		catch (const ConvertException &) { }
	}

	void OnFindNick(const Anope::string &nick)
	{
		if (!current_command)
			return;

		CommandMutex *cm = current_command;

		cm->mutex.Unlock();
		SQLResult res = this->RunQuery("SELECT * FROM `anope_ns_alias` WHERE `nick` = '" + this->Escape(nick) + "'");
		this->Notify();
		cm->mutex.Lock();

		try
		{
			// Make OnFindCore trigger and look up the core too
			NickCore *nc = findcore(res.Get(0, "display"));
			if (!nc)
				return;
			current_command = NULL;
			NickAlias *na = findnick(res.Get(0, "nick"));
			if (!na)
				na = new NickAlias(res.Get(0, "nick"), nc);

			na->last_quit = res.Get(0, "last_quit");
			na->last_realname = res.Get(0, "last_realname");
			na->last_usermask = res.Get(0, "last_usermask");
			na->time_registered = convertTo<time_t>(res.Get(0, "time_registered"));
			na->last_seen = convertTo<time_t>(res.Get(0, "last_seen"));
			// XXX flags

			if (na->nc != nc)
			{
				std::list<NickAlias *>::iterator it = std::find(na->nc->aliases.begin(), na->nc->aliases.end(), na);
				if (it != na->nc->aliases.end())
					na->nc->aliases.erase(it);

				na->nc = nc;
				na->nc->aliases.push_back(na);
			}
		}
		catch (const SQLException &) { }
		catch (const ConvertException &) { }
	}

	void OnFindCore(const Anope::string &nick)
	{
		if (!current_command)
			return;

		CommandMutex *cm = current_command;

		cm->mutex.Unlock();
		SQLResult res = this->RunQuery("SELECT * FROM `anope_ns_core` WHERE `name` = '" + this->Escape(nick) + "'");
		this->Notify();
		cm->mutex.Lock();

		try
		{
  			current_command = NULL;
 			NickCore *nc = findcore(res.Get(0, "display"));
			if (!nc)
				nc = new NickCore(res.Get(0, "display"));

			nc->pass = res.Get(0, "pass");
			nc->email = res.Get(0, "email");
			nc->greet = res.Get(0, "greet");
			// flags
			nc->language = res.Get(0, "language");
		}
		catch (const SQLException &) { }
		catch (const ConvertException &) { }
	}
};

MODULE_INIT(MySQLLiveModule)
