#include "module.h"
#include "../extra/sql.h"
#include "../commands/os_session.h"

class MySQLInterface : public SQLInterface
{
 public:
	MySQLInterface(Module *o) : SQLInterface(o) { }

	void OnResult(const SQLResult &r)
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const SQLResult &r)
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
	}
};

class DBMySQL : public Module
{
 private:
	MySQLInterface sqlinterface;
	service_reference<SQLProvider, Base> SQL;
 	std::set<Anope::string> tables;

	void RunQuery(const SQLQuery &query)
	{
		if (SQL)
		{
			if (readonly && this->ro)
			{
				readonly = this->ro = false;
				BotInfo *bi = findbot(Config->OperServ);
				if (bi)
					ircdproto->SendGlobops(bi, "Found SQL again, going out of readonly mode...");
			}

			SQL->Run(&sqlinterface, query);
		}
		else
		{
			if (Anope::CurTime - Config->UpdateTimeout > lastwarn)
			{
				BotInfo *bi = findbot(Config->OperServ);
				if (bi)
					ircdproto->SendGlobops(bi, "Unable to locate SQL reference, going to readonly...");
				readonly = this->ro = true;
				this->lastwarn = Anope::CurTime;
			}
		}
	}

	void Insert(const Anope::string &table, const Serializable::serialized_data &data)
	{
		if (tables.count(table) == 0 && SQL)
		{
			this->RunQuery(this->SQL->CreateTable(table, data));
			tables.insert(table);
		}

		Anope::string query_text = "INSERT INTO `" + table + "` (";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "`,";
		query_text.erase(query_text.end() - 1);
		query_text += ") VALUES (";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "@" + it->first + "@,";
		query_text.erase(query_text.end() - 1);
		query_text += ") ON DUPLICATE KEY UPDATE ";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += it->first + "=VALUES(" + it->first + "),";
		query_text.erase(query_text.end() - 1);

		SQLQuery query(query_text);
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunQuery(query);
	}

	void Delete(const Anope::string &table, const Serializable::serialized_data &data)
	{
		Anope::string query_text = "DELETE FROM `" + table + "` WHERE ";
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query_text += "`" + it->first + "` = @" + it->first + "@";

		SQLQuery query(query_text);
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunQuery(query);
	}

 public:
	time_t lastwarn;
	bool ro;

	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sqlinterface(this), SQL("")
	{
		this->lastwarn = 0;
		this->ro = false;

		Implementation i[] = { I_OnReload, I_OnServerConnect };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();

		if (CurrentUplink)
			OnServerConnect();
	}

	void OnReload()
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->SQL = engine;
	}

	void OnServerConnect()
	{
		Implementation i[] = {
			/* Misc */
			I_OnPostCommand,
			/* NickServ */
			I_OnNickAddAccess, I_OnNickEraseAccess, I_OnNickClearAccess,
			I_OnDelCore, I_OnNickForbidden, I_OnNickGroup,
			I_OnNickRegister, I_OnChangeCoreDisplay,
			I_OnNickSuspended, I_OnDelNick,
			/* ChanServ */
			I_OnAccessAdd, I_OnAccessDel, I_OnAccessClear, I_OnLevelChange,
			I_OnDelChan, I_OnChanRegistered, I_OnChanSuspend,
			I_OnAkickAdd, I_OnAkickDel, I_OnMLock, I_OnUnMLock,
			/* BotServ */
			I_OnBotCreate, I_OnBotChange, I_OnBotDelete,
			I_OnBotAssign, I_OnBotUnAssign,
			I_OnBadWordAdd, I_OnBadWordDel,
			/* MemoServ */
			I_OnMemoSend, I_OnMemoDel,
			/* OperServ */
			I_OnExceptionAdd, I_OnExceptionDel,
			I_OnAddXLine, I_OnDelXLine,
			/* HostServ */
			I_OnSetVhost, I_OnDeleteVhost
		};
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (command->name.find("nickserv/set/") == 0)
		{
			NickAlias *na = findnick(source.u->nick);
			if (!na)
				this->Insert(na->nc->GetSerializeName(), na->nc->serialize());

		}
		else if (command->name.find("nickserv/saset/") == 0)
		{
			NickAlias *na = findnick(params[1]);
			if (!na)
				this->Insert(na->nc->GetSerializeName(), na->nc->serialize());
		}
		else if (command->name.find("chanserv/set") == 0 || command->name.find("chanserv/saset") == 0)
		{
			ChannelInfo *ci = params.size() > 0 ? cs_findchan(params[0]) : NULL;
			if (!ci)
				this->Insert(ci->GetSerializeName(), ci->serialize());
		}
		else if (command->name == "botserv/kick" && params.size() > 2)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			if (!ci)
				return;
			if (!ci->AccessFor(u).HasPriv("SET") && !u->HasPriv("botserv/administration"))
				return;
			this->Insert(ci->GetSerializeName(), ci->serialize());
		}
		else if (command->name == "botserv/set" && params.size() > 1)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			BotInfo *bi = NULL;
			if (!ci)
				bi = findbot(params[0]);
			if (bi && params[1].equals_ci("PRIVATE") && u->HasPriv("botserv/set/private"))
				this->Insert(bi->GetSerializeName(), bi->serialize());
			else if (ci && !ci->AccessFor(u).HasPriv("SET") && !u->HasPriv("botserv/administration"))
				this->Insert(ci->GetSerializeName(), ci->serialize());
		}
		else if (command->name == "memoserv/ignore" && params.size() > 0)
		{
			Anope::string target = params[0];
			if (target[0] != '#')
			{
				NickCore *nc = u->Account();
				if (nc)
					this->Insert(nc->GetSerializeName(), nc->serialize());
			}
			else
			{
				ChannelInfo *ci = cs_findchan(target);
				if (ci && ci->AccessFor(u).HasPriv("MEMO"))
					this->Insert(ci->GetSerializeName(), ci->serialize());
			}
		}
	}

	void OnNickAddAccess(NickCore *nc, const Anope::string &entry)
	{
		this->Insert(nc->GetSerializeName(), nc->serialize());
	}

	void OnNickEraseAccess(NickCore *nc, const Anope::string &entry)
	{
		this->Insert(nc->GetSerializeName(), nc->serialize());
	}

	void OnNickClearAccess(NickCore *nc)
	{
		this->Insert(nc->GetSerializeName(), nc->serialize());
	}

	void OnDelCore(NickCore *nc)
	{
		this->Delete(nc->GetSerializeName(), nc->serialize());
	}

	void OnNickForbidden(NickAlias *na)
	{
		this->Insert(na->GetSerializeName(), na->serialize());
	}

	void OnNickGroup(User *u, NickAlias *)
	{
		OnNickRegister(findnick(u->nick));
	}

	void InsertAlias(NickAlias *na)
	{
		this->Insert(na->GetSerializeName(), na->serialize());
	}

	void InsertCore(NickCore *nc)
	{
		this->Insert(nc->GetSerializeName(), nc->serialize());
	}

	void OnNickRegister(NickAlias *na)
	{
		this->InsertCore(na->nc);
		this->InsertAlias(na);
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay)
	{
		Serializable::serialized_data data = nc->serialize();
		this->Delete(nc->GetSerializeName(), data);
		data.erase("display");
		data["display"] << newdisplay;
		this->Insert(nc->GetSerializeName(), data);

		for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *na = *it;
			data = na->serialize();

			this->Delete(na->GetSerializeName(), data);
			data.erase("nc");
			data["nc"] << newdisplay;
			this->Insert(na->GetSerializeName(), data);
		}
	}

	void OnNickSuspend(NickAlias *na)
	{
		this->Insert(na->GetSerializeName(), na->serialize());
	}

	void OnDelNick(NickAlias *na)
	{
		this->Delete(na->GetSerializeName(), na->serialize());
	}

	void OnAccessAdd(ChannelInfo *ci, User *, ChanAccess *access)
	{
		this->Insert(access->GetSerializeName(), access->serialize());
	}

	void OnAccessDel(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		this->Delete(access->GetSerializeName(), access->serialize());
	}

	void OnAccessClear(ChannelInfo *ci, User *u)
	{
		for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			this->OnAccessDel(ci, NULL, ci->GetAccess(i));
	}

	void OnLevelChange(User *u, ChannelInfo *ci, const Anope::string &priv, int16_t what)
	{
		this->Insert(ci->GetSerializeName(), ci->serialize());
	}

	void OnDelChan(ChannelInfo *ci)
	{
		this->Delete(ci->GetSerializeName(), ci->serialize());
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		this->Insert(ci->GetSerializeName(), ci->serialize());
	}

	void OnChanSuspend(ChannelInfo *ci)
	{
		this->Insert(ci->GetSerializeName(), ci->serialize());
	}

	void OnAkickAdd(ChannelInfo *ci, AutoKick *ak)
	{
		this->Insert(ak->GetSerializeName(), ak->serialize());
	}

	void OnAkickDel(ChannelInfo *ci, AutoKick *ak)
	{
		this->Delete(ak->GetSerializeName(), ak->serialize());
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock)
	{
		this->Insert(lock->GetSerializeName(), lock->serialize());
		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock)
	{
		this->Delete(lock->GetSerializeName(), lock->serialize());
		return EVENT_CONTINUE;
	}

	void OnBotCreate(BotInfo *bi)
	{
		this->Insert(bi->GetSerializeName(), bi->serialize());
	}

	void OnBotChange(BotInfo *bi)
	{
		OnBotCreate(bi);
	}

	void OnBotDelete(BotInfo *bi)
	{
		this->Delete(bi->GetSerializeName(), bi->serialize());
	}

	EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi)
	{
		this->Insert(ci->GetSerializeName(), ci->serialize());
		return EVENT_CONTINUE;
	}

	EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci)
	{
		this->Insert(ci->GetSerializeName(), ci->serialize());
		return EVENT_CONTINUE;
	}

	void OnBadWordAdd(ChannelInfo *ci, BadWord *bw)
	{
		this->Insert(bw->GetSerializeName(), bw->serialize());
	}

	void OnBadWordDel(ChannelInfo *ci, BadWord *bw)
	{
		this->Delete(bw->GetSerializeName(), bw->serialize());
	}

	void OnMemoSend(const Anope::string &source, const Anope::string &target, MemoInfo *mi, Memo *m)
	{
		this->Insert(m->GetSerializeName(), m->serialize());
	}

	void OnMemoDel(const NickCore *nc, MemoInfo *mi, Memo *m)
	{
		this->Delete(m->GetSerializeName(), m->serialize());
	}

	void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, Memo *m)
	{
		this->Delete(m->GetSerializeName(), m->serialize());
	}

	EventReturn OnExceptionAdd(Exception *ex)
	{
		this->Insert(ex->GetSerializeName(), ex->serialize());
		return EVENT_CONTINUE;
	}

	void OnExceptionDel(User *, Exception *ex)
	{
		this->Delete(ex->GetSerializeName(), ex->serialize());
	}

	EventReturn OnAddXLine(XLine *x, XLineManager *xlm)
	{
		this->Insert(x->GetSerializeName(), x->serialize());
		return EVENT_CONTINUE;
	}

	void OnDelXLine(User *, XLine *x, XLineManager *xlm)
	{
		this->Delete(x->GetSerializeName(), x->serialize());
	}

	void OnDeleteVhost(NickAlias *na)
	{
		this->Insert(na->GetSerializeName(), na->serialize());
	}

	void OnSetVhost(NickAlias *na)
	{
		this->Insert(na->GetSerializeName(), na->serialize());
	}
};

MODULE_INIT(DBMySQL)

