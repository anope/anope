#include "module.h"
#include "../extra/sql.h"
#include "../commands/os_session.h"

class MySQLInterface : public SQLInterface
{
 public:
	MySQLInterface(Module *o) : SQLInterface(o) { }

	void OnResult(const SQLResult &r) anope_override
	{
		Log(LOG_DEBUG) << "SQL successfully executed query: " << r.finished_query;
	}

	void OnError(const SQLResult &r) anope_override
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
	service_reference<SQLProvider> SQL;
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
			query_text += "`" + it->first + "`=VALUES(`" + it->first + "`),";
		query_text.erase(query_text.end() - 1);

		SQLQuery query(query_text);
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunQuery(query);
	}

	void Delete(const Anope::string &table, const Serializable::serialized_data &data)
	{
		Anope::string query_text = "DELETE FROM `" + table + "` WHERE ", arg_text;
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
		{
			if (!arg_text.empty())
				arg_text += " AND ";
			arg_text += "`" + it->first + "` = @" + it->first + "@";
		}

		query_text += arg_text;

		SQLQuery query(query_text);
		for (Serializable::serialized_data::const_iterator it = data.begin(), it_end = data.end(); it != it_end; ++it)
			query.setValue(it->first, it->second.astr());

		this->RunQuery(query);
	}

 public:
	time_t lastwarn;
	bool ro;

	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), sqlinterface(this), SQL("", "")
	{
		this->lastwarn = 0;
		this->ro = false;

		Implementation i[] = { I_OnReload, I_OnServerConnect };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();

		if (CurrentUplink)
			OnServerConnect();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		Anope::string engine = config.ReadValue("db_sql", "engine", "", 0);
		this->SQL = service_reference<SQLProvider>("SQLProvider", engine);
	}

	void OnServerConnect() anope_override
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

	void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;

		if (command->name.find("nickserv/set/") == 0)
		{
			NickAlias *na = findnick(source.u->nick);
			if (na)
				this->Insert(na->nc->serialize_name(), na->nc->serialize());

		}
		else if (command->name.find("nickserv/saset/") == 0)
		{
			NickAlias *na = findnick(params[0]);
			if (na)
				this->Insert(na->nc->serialize_name(), na->nc->serialize());
		}
		else if (command->name.find("chanserv/set") == 0 || command->name.find("chanserv/saset") == 0)
		{
			ChannelInfo *ci = params.size() > 0 ? cs_findchan(params[0]) : NULL;
			if (ci)
				this->Insert(ci->serialize_name(), ci->serialize());
		}
		else if (command->name == "botserv/kick" && params.size() > 2)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			if (!ci)
				return;
			if (!ci->AccessFor(u).HasPriv("SET") && !u->HasPriv("botserv/administration"))
				return;
			this->Insert(ci->serialize_name(), ci->serialize());
		}
		else if (command->name == "botserv/set" && params.size() > 1)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			BotInfo *bi = NULL;
			if (!ci)
				bi = findbot(params[0]);
			if (bi && params[1].equals_ci("PRIVATE") && u->HasPriv("botserv/set/private"))
				this->Insert(bi->serialize_name(), bi->serialize());
			else if (ci && !ci->AccessFor(u).HasPriv("SET") && !u->HasPriv("botserv/administration"))
				this->Insert(ci->serialize_name(), ci->serialize());
		}
		else if (command->name == "memoserv/ignore" && params.size() > 0)
		{
			Anope::string target = params[0];
			if (target[0] != '#')
			{
				NickCore *nc = u->Account();
				if (nc)
					this->Insert(nc->serialize_name(), nc->serialize());
			}
			else
			{
				ChannelInfo *ci = cs_findchan(target);
				if (ci && ci->AccessFor(u).HasPriv("MEMO"))
					this->Insert(ci->serialize_name(), ci->serialize());
			}
		}
	}

	void OnNickAddAccess(NickCore *nc, const Anope::string &entry) anope_override
	{
		this->Insert(nc->serialize_name(), nc->serialize());
	}

	void OnNickEraseAccess(NickCore *nc, const Anope::string &entry) anope_override
	{
		this->Insert(nc->serialize_name(), nc->serialize());
	}

	void OnNickClearAccess(NickCore *nc) anope_override
	{
		this->Insert(nc->serialize_name(), nc->serialize());
	}

	void OnDelCore(NickCore *nc) anope_override
	{
		this->Delete(nc->serialize_name(), nc->serialize());
	}

	void OnNickForbidden(NickAlias *na) anope_override
	{
		this->Insert(na->serialize_name(), na->serialize());
	}

	void OnNickGroup(User *u, NickAlias *) anope_override
	{
		OnNickRegister(findnick(u->nick));
	}

	void InsertAlias(NickAlias *na)
	{
		this->Insert(na->serialize_name(), na->serialize());
	}

	void InsertCore(NickCore *nc)
	{
		this->Insert(nc->serialize_name(), nc->serialize());
	}

	void OnNickRegister(NickAlias *na) anope_override
	{
		this->InsertCore(na->nc);
		this->InsertAlias(na);
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay) anope_override
	{
		Serializable::serialized_data data = nc->serialize();
		this->Delete(nc->serialize_name(), data);
		data.erase("display");
		data["display"] << newdisplay;
		this->Insert(nc->serialize_name(), data);

		for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *na = *it;
			data = na->serialize();

			this->Delete(na->serialize_name(), data);
			data.erase("nc");
			data["nc"] << newdisplay;
			this->Insert(na->serialize_name(), data);
		}
	}

	void OnNickSuspend(NickAlias *na) anope_override
	{
		this->Insert(na->serialize_name(), na->serialize());
	}

	void OnDelNick(NickAlias *na) anope_override
	{
		this->Delete(na->serialize_name(), na->serialize());
	}

	void OnAccessAdd(ChannelInfo *ci, User *, ChanAccess *access) anope_override
	{
		this->Insert(access->serialize_name(), access->serialize());
	}

	void OnAccessDel(ChannelInfo *ci, User *u, ChanAccess *access) anope_override
	{
		this->Delete(access->serialize_name(), access->serialize());
	}

	void OnAccessClear(ChannelInfo *ci, User *u) anope_override
	{
		for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			this->OnAccessDel(ci, NULL, ci->GetAccess(i));
	}

	void OnLevelChange(User *u, ChannelInfo *ci, const Anope::string &priv, int16_t what) anope_override
	{
		this->Insert(ci->serialize_name(), ci->serialize());
	}

	void OnDelChan(ChannelInfo *ci) anope_override
	{
		this->Delete(ci->serialize_name(), ci->serialize());
	}

	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		this->Insert(ci->serialize_name(), ci->serialize());
	}

	void OnChanSuspend(ChannelInfo *ci) anope_override
	{
		this->Insert(ci->serialize_name(), ci->serialize());
	}

	void OnAkickAdd(User *u, ChannelInfo *ci, AutoKick *ak) anope_override
	{
		this->Insert(ak->serialize_name(), ak->serialize());
	}

	void OnAkickDel(User *u, ChannelInfo *ci, AutoKick *ak) anope_override
	{
		this->Delete(ak->serialize_name(), ak->serialize());
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		this->Insert(lock->serialize_name(), lock->serialize());
		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		this->Delete(lock->serialize_name(), lock->serialize());
		return EVENT_CONTINUE;
	}

	void OnBotCreate(BotInfo *bi) anope_override
	{
		this->Insert(bi->serialize_name(), bi->serialize());
	}

	void OnBotChange(BotInfo *bi) anope_override
	{
		OnBotCreate(bi);
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		this->Delete(bi->serialize_name(), bi->serialize());
	}

	EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi) anope_override
	{
		this->Insert(ci->serialize_name(), ci->serialize());
		return EVENT_CONTINUE;
	}

	EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci) anope_override
	{
		this->Insert(ci->serialize_name(), ci->serialize());
		return EVENT_CONTINUE;
	}

	void OnBadWordAdd(ChannelInfo *ci, BadWord *bw) anope_override
	{
		this->Insert(bw->serialize_name(), bw->serialize());
	}

	void OnBadWordDel(ChannelInfo *ci, BadWord *bw) anope_override
	{
		this->Delete(bw->serialize_name(), bw->serialize());
	}

	void OnMemoSend(const Anope::string &source, const Anope::string &target, MemoInfo *mi, Memo *m) anope_override
	{
		this->Insert(m->serialize_name(), m->serialize());
	}

	void OnMemoDel(const NickCore *nc, MemoInfo *mi, Memo *m) anope_override
	{
		this->Delete(m->serialize_name(), m->serialize());
	}

	void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, Memo *m) anope_override
	{
		this->Delete(m->serialize_name(), m->serialize());
	}

	EventReturn OnExceptionAdd(Exception *ex) anope_override
	{
		this->Insert(ex->serialize_name(), ex->serialize());
		return EVENT_CONTINUE;
	}

	void OnExceptionDel(User *, Exception *ex) anope_override
	{
		this->Delete(ex->serialize_name(), ex->serialize());
	}

	EventReturn OnAddXLine(User *u, XLine *x, XLineManager *xlm) anope_override
	{
		this->Insert(x->serialize_name(), x->serialize());
		return EVENT_CONTINUE;
	}

	void OnDelXLine(User *, XLine *x, XLineManager *xlm) anope_override
	{
		this->Delete(x->serialize_name(), x->serialize());
	}

	void OnDeleteVhost(NickAlias *na) anope_override
	{
		this->Insert(na->serialize_name(), na->serialize());
	}

	void OnSetVhost(NickAlias *na) anope_override
	{
		this->Insert(na->serialize_name(), na->serialize());
	}
};

MODULE_INIT(DBMySQL)

