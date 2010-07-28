/* RequiredLibraries: mysqlpp */

#include "db_mysql.h"

static std::string BuildFlagsList(ChannelInfo *ci)
{
	std::string ret;

	for (int i = 0; ChannelFlags[i].Flag != -1; ++i)
		if (ci->HasFlag(ChannelFlags[i].Flag))
			ret += " " + ChannelFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static std::string BuildFlagsList(NickAlias *na)
{
	std::string ret;

	for (int i = 0; NickAliasFlags[i].Flag != -1; ++i)
		if (na->HasFlag(NickAliasFlags[i].Flag))
			ret += " " + NickAliasFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static std::string BuildFlagsList(NickCore *nc)
{
	std::string ret;

	for (int i = 0; NickCoreFlags[i].Flag != -1; ++i)
		if (nc->HasFlag(NickCoreFlags[i].Flag))
			ret += " " + NickCoreFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static std::string BuildFlagsList(Memo *m)
{
	std::string ret;

	for (int i = 0; MemoFlags[i].Flag != -1; ++i)
		if (m->HasFlag(MemoFlags[i].Flag))
			ret += " " + MemoFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static std::string MakeMLock(ChannelInfo *ci, bool status)
{
	std::string ret;

	for (std::list<Mode *>::iterator it = ModeManager::Modes.begin(), it_end = ModeManager::Modes.end(); it != it_end; ++it)
	{
		if ((*it)->Class == MC_CHANNEL)
		{
			ChannelMode *cm = debug_cast<ChannelMode *>(*it);

			if (ci->HasMLock(cm->Name, status))
				ret += " " + cm->NameAsString;
		}
	}

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static inline std::string GetMLockOn(ChannelInfo *ci)
{
	return MakeMLock(ci, true);
}

static inline std::string GetMLockOff(ChannelInfo *ci)
{
	return MakeMLock(ci, false);
}

static std::string GetMLockParams(ChannelInfo *ci)
{
	std::string ret;

	for (std::list<Mode *>::iterator it = ModeManager::Modes.begin(), it_end = ModeManager::Modes.end(); it != it_end; ++it)
	{
		if ((*it)->Class == MC_CHANNEL)
		{
			ChannelMode *cm = debug_cast<ChannelMode *>(*it);

			std::string param;
			if (ci->GetParam(cm->Name, param))
				ret += " " + cm->NameAsString + " " + param;
		}
	}

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static std::string GetBotFlags(Flags<BotServFlag>& Flags)
{
	std::string buf;

	for (int i = 0; BotFlags[i].Flag != -1; ++i)
		if (Flags.HasFlag(BotFlags[i].Flag))
			buf += " " + BotFlags[i].Name;

	if (!buf.empty())
		buf.erase(buf.begin());

	return buf;
}

static std::string GetBotServFlags(BotInfo *bi)
{
	std::string buf;

	for (int i = 0; BotServFlags[i].Flag != -1; ++i)
		if (bi->HasFlag(BotServFlags[i].Flag))
			buf += " " + BotServFlags[i].Name;

	if (!buf.empty())
		buf.erase(buf.begin());;

	return buf;
}

static NickAlias *CurNick = NULL;
static NickCore *CurCore = NULL;
static ChannelInfo *CurChannel = NULL;
static BotInfo *CurBot = NULL;

void Write(const std::string &data)
{
	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_extra` (data) VALUES(" << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

void WriteMetadata(const std::string &key, const std::string &data)
{
	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_metadata` (name, value) VALUES(" << mysqlpp::quote << key << ", " << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

void WriteNickMetadata(const std::string &key, const std::string &data)
{
	if (!CurNick)
		throw CoreException(Anope::string("WriteNickMetadata without a nick to write"));

	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_ns_alias_metadata` (nick, name, value) VALUES(" << mysqlpp::quote << CurNick->nick << ", " << mysqlpp::quote << key << ", " << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

void WriteCoreMetadata(const std::string &key, const std::string &data)
{
	if (!CurCore)
		throw CoreException(Anope::string("WritCoreMetadata without a core to write"));

	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_ns_core_metadata` (nick, name, value) VALUES(" << mysqlpp::quote << CurCore->display << ", " << mysqlpp::quote << key << ", " << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

void WriteChannelMetadata(const std::string &key, const std::string &data)
{
	if (!CurChannel)
		throw CoreException(Anope::string("WriteChannelMetadata without a channel to write"));

	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_cs_info_metadata` (channel, name, value) VALUES(" << mysqlpp::quote << CurChannel->name << ", " << mysqlpp::quote << key << ", " << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

void WriteBotMetadata(const std::string &key, const std::string &data)
{
	if (!CurBot)
		throw CoreException(Anope::string("WriteBotMetadata without a bot to write"));

	mysqlpp::Query query(me->Con);
	query << "INSERT DELAYED INTO `anope_bs_info_metadata` (botname, name, value) VALUES(" << mysqlpp::quote << CurBot->nick << ", " << mysqlpp::quote << key << ", " << mysqlpp::quote << data << ")";
	ExecuteQuery(query);
}

static void SaveDatabases()
{
	mysqlpp::Query query(me->Con);

	query << "TRUNCATE TABLE `anope_ns_alias`";
	ExecuteQuery(query);

	for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		me->OnNickRegister(it->second);

	query << "TRUNCATE TABLE `anope_ns_core`";
	ExecuteQuery(query);
	query << "TRUNCATE TABLE `anope_ms_info`";
	ExecuteQuery(query);

	for (nickcore_map::const_iterator nit = NickCoreList.begin(), nit_end = NickCoreList.end(); nit != nit_end; ++nit)
	{
		NickCore *nc = nit->second;

		for (std::vector<std::string>::iterator it = nc->access.begin(), it_end = nc->access.end(); it != it_end; ++it)
		{
			query << "INSERT DELAYED INTO `anope_ns_access` (display, access) VALUES(" << mysqlpp::quote << nc->display << ", " << mysqlpp::quote << *it << ")";
			ExecuteQuery(query);
		}

		for (unsigned j = 0, end = nc->memos.memos.size(); j < end; ++j)
		{
			Memo *m = nc->memos.memos[j];

			me->OnMemoSend(NULL, nc, m);
		}
	}


	query << "TRUNCATE TABLE `anope_bs_core`";
	ExecuteQuery(query);

	for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		me->OnBotCreate(it->second);

	query << "TRUNCATE TABLE `anope_cs_info`";
	ExecuteQuery(query);
	query << "TRUNCATE TABLE `anope_bs_badwords`";
	ExecuteQuery(query);
	query << "TRUNCATE TABLE `anope_cs_access`";
	ExecuteQuery(query);
	query << "TRUNCATE TABLE `anope_cs_akick`";
	ExecuteQuery(query);
	query << "TRUNCATE TABLE `anope_cs_levels`";
	ExecuteQuery(query);

	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		me->OnChanRegistered(ci);

		for (unsigned j = 0, end = ci->GetBadWordCount(); j < end; ++j)
		{
			BadWord *bw = ci->GetBadWord(j);

			me->OnBadWordAdd(ci, bw);
		}

		for (unsigned j = 0, end = ci->GetAccessCount(); j < end; ++j)
		{
			ChanAccess *access = ci->GetAccess(j);

			query << "INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES('" << access->level << "', " << mysqlpp::quote << access->nc->display << ", " << mysqlpp::quote << ci->name << ", " << access->last_seen << ", " << mysqlpp::quote << access->creator << ") ON DUPLICATE KEY UPDATE level=VALUES(level), last_seen=VALUES(last_seen), creator=VALUES(creator)";
			ExecuteQuery(query);
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);

			me->OnAkickAdd(NULL, ci, ak);
		}

		for (int k = 0; k < CA_SIZE; ++k)
		{
			query << "INSERT DELAYED INTO `anope_cs_levels` (channel, position, level) VALUES(" << mysqlpp::quote << ci->name << ", '" << k << "', '" << ci->levels[k] << "') ON DUPLICATE KEY UPDATE position=VALUES(position), level=VALUES(level)";
			ExecuteQuery(query);
		}

		for (unsigned j = 0, end = ci->memos.memos.size(); j < end; ++j)
		{
			Memo *m = ci->memos.memos[j];

			me->OnMemoSend(NULL, ci, m);
		}
	}

	query << "TRUNCATE TABLE `anope_ns_request`";
	ExecuteQuery(query);

	for (nickrequest_map::const_iterator it = NickRequestList.begin(), it_end = NickRequestList.end(); it != it_end; ++it)
		me->OnMakeNickRequest(it->second);

	if (SGLine)
		for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			me->OnAddAkill(NULL, SGLine->GetEntry(i));

	if (SZLine)
		for (unsigned i = 0, end = SZLine->GetCount(); i < end; ++i)
			me->OnAddXLine(NULL, SZLine->GetEntry(i), X_SZLINE);

	if (SQLine)
		for (unsigned i = 0, end = SQLine->GetCount(); i < end; ++i)
			me->OnAddXLine(NULL, SQLine->GetEntry(i), X_SQLINE);

	if (SNLine)
		for (unsigned i = 0, end = SNLine->GetCount(); i < end; ++i)
			me->OnAddXLine(NULL, SNLine->GetEntry(i), X_SNLINE);

	for (int i = 0; i < nexceptions; ++i)
	{
		Exception *ex = &exceptions[i];

		me->OnExceptionAdd(NULL, ex);
	}
}

class CommandSyncSQL : public Command
{
 public:
	CommandSyncSQL(const ci::string &cname) : Command(cname, 0, 0, "operserv/sqlsync")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		notice_lang(Config.s_OperServ, u, OPER_SYNC_UPDATING);
		SaveDatabases();
		notice_lang(Config.s_OperServ, u, OPER_SYNC_UPDATED);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SYNC);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SQLSYNC);
	}
};

class DBMySQLWrite : public DBMySQL
{
 public:
	DBMySQLWrite(const Anope::string &modname, const Anope::string &creator) : DBMySQL(modname, creator)
	{
		me = this;

		ModuleManager::Attach(I_OnServerConnect, this);

		this->AddCommand(OperServ, new CommandSyncSQL("SQLSYNC"));

		if (uplink_server)
			OnServerConnect();
	}

	~DBMySQLWrite()
	{
	}

	void OnServerConnect()
	{
		Implementation i[] = {
			/* Misc */
			I_OnSaveDatabase, I_OnPostCommand,
			/* NickServ */
			I_OnNickAddAccess, I_OnNickEraseAccess, I_OnNickClearAccess,
			I_OnDelCore, I_OnNickForbidden, I_OnNickGroup, I_OnMakeNickRequest,
			I_OnDelNickRequest, I_OnNickRegister, I_OnChangeCoreDisplay,
			I_OnNickSuspended,
			/* ChanServ */
			I_OnAccessAdd, I_OnAccessDel, I_OnAccessChange, I_OnAccessClear, I_OnLevelChange,
			I_OnChanForbidden, I_OnDelChan, I_OnChanRegistered, I_OnChanSuspend,
			I_OnAkickAdd, I_OnAkickDel,
			/* BotServ */
			I_OnBotCreate, I_OnBotChange, I_OnBotDelete,
			I_OnBotAssign, I_OnBotUnAssign,
			I_OnBadWordAdd, I_OnBadWordDel,
			/* MemoServ */
			I_OnMemoSend, I_OnMemoDel,
			/* OperServ */
			I_OnAddAkill, I_OnDelAkill, I_OnExceptionAdd, I_OnExceptionDel,
			I_OnAddXLine, I_OnDelXLine
		};
		ModuleManager::Attach(i, this, 39);
	}

	EventReturn OnSaveDatabase()
	{
		mysqlpp::Query query(me->Con);

		query << "TRUNCATE TABLE `anope_os_core`";
		ExecuteQuery(query);
		query << "INSERT DELAYED INTO `anope_os_core` (maxusercnt, maxusertime, akills_count, sglines_count, sqlines_count, szlines_count) VALUES(";
		query << maxusercnt << ", " << maxusertime << ", " << (SGLine ? SGLine->GetCount() : 0) << ", " << (SQLine ? SQLine->GetCount() : 0) << ", " << (SNLine ? SNLine->GetCount() : 0) << ", " << (SZLine ? SZLine->GetCount() : 0) << ")";
		ExecuteQuery(query);

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != end; ++it)
		{
			CurCore = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteCoreMetadata, CurCore));
		}

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			CurNick = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteNickMetadata, CurNick));
		}

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChanelList.end(); it != it_end; ++it)
		{
			CurChannel = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteChannelMetadata, CurChannel));
		}

		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			CurBot = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteBotMetadata, CurBot));

			/* This is for the core bots, bots added by users are already handled by an event */
			query << "INSERT DELAYED INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES(";
			query << mysqlpp::quote << CurBot->nick << ", " << mysqlpp::quote << CurBot->user << ", " << mysqlpp::quote << CurBot->host;
			query << ", " << mysqlpp::quote << CurBot->real << ", '" << GetBotServFlags(CurBot) << "', " << CurBot->created << ", ";
			query << CurBot->chancount << ") ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(created)";
			ExecuteQuery(query);
		}

		FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		return EVENT_CONTINUE;
	}

	void OnPostCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params)
	{
		mysqlpp::Query query(me->Con);

		if (service == Config.s_NickServ)
		{
			if (u->Account() && ((command == "SET" && !params.empty()) || (command == "SASET" && u->Account()->HasCommand("nickserv/saset") && params.size() > 1)))
			{
				ci::string cmd = (command == "SET" ? params[0] : params[1]);
				NickCore *nc = (command == "SET" ? u->Account() : findcore(params[0]));
				if (!nc)
					return;
				if (cmd == "PASSWORD" && params.size() > 1)
				{
					query << "UPDATE `anope_ns_core` SET `pass` = " << mysqlpp::quote << nc->pass << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "LANGUAGE" && params.size() > 1)
				{
					query << "UPDATE `anope_ns_core` SET `language` = " << nc->language << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "URL")
				{
					query << "UPDATE `anope_ns_core` SET `url` = " << mysqlpp::quote << (nc->url ? nc->url : "") << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "EMAIL")
				{
					query << "UPDATE `anope_ns_core` SET `email` = " << mysqlpp::quote << (nc->email ? nc->email : "") << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "ICQ")
				{
					query << "UPDATE `anope_ns_core` SET `icq` = " << (nc->icq ? nc->icq : 0) << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "GREET")
				{
					query << "UPDATE `anope_ns_core` SET `greet` = " << mysqlpp::quote << (nc->greet ? nc->greet : "") << " WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
				else if (cmd == "KILL" || cmd == "SECURE" || cmd == "PRIVATE" || cmd == "MSG" || cmd == "HIDE" || cmd == "AUTOOP")
				{
					query << "UPDATE `anope_ns_core` SET `flags` = '" << BuildFlagsList(nc) << "' WHERE `display` = " << mysqlpp::quote << nc->display;
					ExecuteQuery(query);
				}
			}
		}
		else if (service == Config.s_ChanServ)
		{
			if (command == "SET" && u->Account() && params.size() > 1)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (!ci)
					return;
				if (!u->Account()->HasPriv("chanserv/set") && check_access(u, ci, CA_SET))
					return;
				if (params[1] == "FOUNDER" && ci->founder)
				{
					query << "UPDATE `anope_cs_info` SET `founder` = " << mysqlpp::quote << ci->founder->display << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "SUCCESSOR")
				{
					query << "UPDATE `anope_cs_info` SET `successor` = " << mysqlpp::quote << (ci->successor ? ci->successor->display : "") << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "DESC")
				{
					query << "UPDATE `anope_cs_info` SET `descr` = " << mysqlpp::quote << ci->desc << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "URL")
				{
					query << "UPDATE `anope_cs_info` SET `url` = " << mysqlpp::quote << (ci->url ? ci->url : "") << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "EMAIL")
				{
					query << "UPDATE `anope_cs_info` SET `email` = " << mysqlpp::quote << (ci->email ? ci->email : "") << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "ENTRYMSG")
				{
					query << "UPDATE `anope_cs_info` SET `entry_message` = " << mysqlpp::quote << (ci->entry_message ? ci->entry_message : "") << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "MLOCK")
				{
					query << "UPDATE `anope_cs_info` SET `mlock_on` = '" << GetMLockOn(ci) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
					query << "UPDATE `anope_cs_info` SET `mlock_off` = '" << GetMLockOff(ci) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
					query << "UPDATE `anope_cs_info` SET `mlock_params` = '" << GetMLockParams(ci) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "BANTYPE")
				{
					query << "UPDATE `anope_cs_info` SET `bantype` = " << ci->bantype << " WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
				else if (params[1] == "KEEPTOPIC" || params[1] == "TOPICLOCK" || params[1] == "PRIVATE" || params[1] == "SECUREOPS" || params[1] == "SECUREFOUNDER" || params[1] == "RESTRICTED" || params[1] == "SECURE" || params[1] == "SIGNKICK" || params[1] == "OPNOTICE" || params[1] == "XOP" || params[1] == "PEACE" || params[1] == "PERSIST" || params[1] == "NOEXPIRE")
				{
					query << "UPDATE `anope_cs_info` SET `flags` = '" << BuildFlagsList(ci) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
			}
		}
		else if (Config.s_BotServ && service == Config.s_BotServ)
		{
			if (command == "KICK" && params.size() > 2)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (!ci)
					return;
				if (!check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
					return;
				if (params[1] == "BADWORDS" || params[1] == "BOLDS" || params[1] == "CAPS" || params[1] == "COLORS" || params[1] == "FLOOD" || params[1] == "REPEAT" || params[1] == "REVERSES" || params[1] == "UNDERLINES")
				{
					if (params[2] == "ON" || params[2] == "OFF")
					{
						for (int i = 0; i < TTB_SIZE; ++i)
						{
							query << "INSERT DELAYED INTO `anope_cs_ttb` (channel, ttb_id, value) VALUES(" << mysqlpp::quote << ci->name << ", " << i << ", " << ci->ttb[i] << ") ON DUPLICATE KEY UPDATE channel=VALUES(channel), ttb_id=VALUES(ttb_id), value=VALUES(value)";
							ExecuteQuery(query);
						}
						query << "UPDATE `anope_cs_info` SET `botflags` = '" << GetBotFlags(ci->botflags) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
						ExecuteQuery(query);

						if (params[1] == "CAPS")
						{
							query << "UPDATE `anope_cs_info` SET `capsmin` = " << ci->capsmin << ", `capspercent` = " << ci->capspercent << " WHERE `name` = " << mysqlpp::quote << ci->name;
							ExecuteQuery(query);
						}
						else if (params[1] == "FLOOD")
						{
							query << "UPDATE `anope_cs_info` SET `floodlines` = " << ci->floodlines << ", `floodsecs` = " << ci->floodsecs << " WHERE `name` = " << mysqlpp::quote << ci->name;
							ExecuteQuery(query);
						}
						else if (params[1] == "REPEAT")
						{
							query << "UPDATE `anope_cs_info` SET `repeattimes` = " << ci->repeattimes << " WHERE `name` = " << mysqlpp::quote << ci->name;
							ExecuteQuery(query);
						}
					}
				}
			}
			else if (command == "SET" && params.size() > 2)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (ci && !check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
					return;
				BotInfo *bi = NULL;
				if (!ci)
					bi = findbot(params[0]);
				if (bi && params[1] == "PRIVATE" && u->Account()->HasPriv("botserv/set/private"))
				{
					query << "UPDATE `anope_bs_core` SET `flags` = '" << GetBotServFlags(bi) << "' WHERE `nick` = " << mysqlpp::quote << bi->nick;
					ExecuteQuery(query);
				}
				else if (!ci)
					return;
				else if (params[1] == "DONTKICKOPS" || params[1] == "DONTKICKVOICES" || params[1] == "FANTASY" || params[1] == "GREET" || params[1] == "SYMBIOSIS" || params[1] == "NOBOT")
				{
					query << "UPDATE `anope_cs_info` SET `botflags` = '" << GetBotFlags(ci->botflags) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
					ExecuteQuery(query);
				}
			}
		}
	}

	void OnNickAddAccess(NickCore *nc, const Anope::string &entry)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_ns_access` (display, access) VALUES(" << mysqlpp::quote << nc->display << ", " << mysqlpp::quote << entry << ")";
		ExecuteQuery(query);
	}

	void OnNickEraseAccess(NickCore *nc, const Anope::string &entry)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_ns_access` WHERE `display` = " << mysqlpp::quote << nc->display << " AND `access` = " << mysqlpp::quote << entry;
		ExecuteQuery(query);
	}

	void OnNickClearAccess(NickCore *nc)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_ns_access` WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
	}

	void OnDelCore(NickCore *nc)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_cs_access` WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_cs_akick` WHERE `mask` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_ns_access` WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_ns_alias` WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_ns_core` WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_ms_info` WHERE `receiver` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
	}

	void OnNickForbidden(NickAlias *na)
	{
		std::string flags = BuildFlagsList(na);
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_ns_alias` SET `flags` = '" << (!flags.empty() ? flags : "") << "' WHERE `nick` = " << mysqlpp::quote << na->nick;
		ExecuteQuery(query);
	}

	void OnNickGroup(User *u, NickAlias *)
	{
		OnNickRegister(findnick(u->nick));
	}

	void OnMakeNickRequest(NickRequest *nr)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_ns_request` (nick, passcode, password, email, requested) VALUES(" << mysqlpp::quote << nr->nick << ", ";
		query << mysqlpp::quote << nr->passcode << ", " << mysqlpp::quote << nr->password << ", " << mysqlpp::quote << nr->email << ", '";
		query << nr->requested << "')";
		ExecuteQuery(query);
	}

	void OnDelNickRequest(NickRequest *nr)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_ns_request` WHERE `nick` = " << mysqlpp::quote << nr->nick;
		ExecuteQuery(query);
	}

	void OnNickRegister(NickAlias *na)
	{
		std::string flags = BuildFlagsList(na);
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_ns_alias` (nick, last_quit, last_realname, last_usermask, time_registered, last_seen, flags, display) VALUES(";
		query << mysqlpp::quote << na->nick << ", " << mysqlpp::quote << (na->last_quit ? na->last_quit : "") << ", ";
		query << mysqlpp::quote << (na->last_realname ? na->last_realname : "") << ", ";
		query << mysqlpp::quote << (na->last_usermask ? na->last_usermask : "") << ", " << na->time_registered << ", " << na->last_seen;
		query << ", '" << (!flags.empty() ? flags : "") << "', " << mysqlpp::quote << na->nc->display << ") ";
		query << "ON DUPLICATE KEY UPDATE last_quit=VALUES(last_quit), last_realname=VALUES(last_realname), last_usermask=VALUES(last_usermask), time_registered=VALUES(time_registered), last_seen=VALUES(last_seen), flags=VALUES(flags), display=VALUES(display)";
		ExecuteQuery(query);

		flags = BuildFlagsList(na->nc);
		query << "INSERT DELAYED INTO `anope_ns_core` (display, pass, email, greet, icq, url, flags, language, channelcount, memomax) VALUES(";
		query << mysqlpp::quote << na->nc->display << ", " << mysqlpp::quote << na->nc->pass << ", ";
		query << mysqlpp::quote << (na->nc->email ? na->nc->email : "") << ", " << mysqlpp::quote << (na->nc->greet ? na->nc->greet : "");
		query << ", " << na->nc->icq << ", " << mysqlpp::quote << (na->nc->url ? na->nc->url : "");
		query << ", '" << (!flags.empty() ? flags : "") << "', " << na->nc->language << ", " << na->nc->channelcount << ", ";
		query << na->nc->memos.memomax << ") ";
		query << "ON DUPLICATE KEY UPDATE pass=VALUES(pass), email=VALUES(email), greet=VALUES(greet), icq=VALUES(icq), flags=VALUES(flags), language=VALUES(language), ";
		query << "channelcount=VALUES(channelcount), memomax=VALUES(memomax)";
		ExecuteQuery(query);
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay)
	{
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_ns_core` SET `display` = " << mysqlpp::quote << newdisplay << " WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_ns_alias` SET `display` = " << mysqlpp::quote << newdisplay << " WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_ns_access` SET `display` = " << mysqlpp::quote << newdisplay << " WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_access` SET `display` = " << mysqlpp::quote << newdisplay << " WHERE `display` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_info` SET `founder` = " << mysqlpp::quote << newdisplay << " WHERE `founder` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_info` SET `successor` = " << mysqlpp::quote << newdisplay << " WHERE `successor` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
		query << "UPDATE `anope_ms_info` SET `receiver` = " << mysqlpp::quote << newdisplay << " WHERE `receiver` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
	}

	void OnNickSuspend(NickAlias *na)
	{
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_ns_core` SET `flags` = '" << BuildFlagsList(na->nc) << "' WHERE `display` = " << mysqlpp::quote << na->nc->display;
		ExecuteQuery(query);
	}

	void OnAccessAdd(ChannelInfo *ci, User *u, NickAlias *na, int level)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES (" << level << ", " << mysqlpp::quote << na->nc->display << ", " << mysqlpp::quote << ci->name << ", " << time(NULL) << ", " << mysqlpp::quote << u->nick << ")";
		ExecuteQuery(query);
	}

	void OnAccessDel(ChannelInfo *ci, User *u, NickCore *nc)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_cs_access` WHERE `display` = " << mysqlpp::quote << nc->display << " AND `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
	}

	void OnAccessChange(ChannelInfo *ci, User *u, NickAlias *na, int level)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES (" << level << ", " << mysqlpp::quote << na->nc->display << ", " << mysqlpp::quote << ci->name << ", " << time(NULL) << ", " << mysqlpp::quote << u->nick << ") ON DUPLICATE KEY UPDATE level=VALUES(level), display=VALUES(display), channel=VALUES(channel), last_seen=VALUES(last_seen), creator=VALUES(creator)";
		ExecuteQuery(query);
	}

	void OnAccessClear(ChannelInfo *ci, User *u)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_cs_access` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
	}

	void OnLevelChange(User *u, ChannelInfo *ci, int pos, int what)
	{
		mysqlpp::Query query(me->Con);

		if (pos >= 0)
		{
			query << "UPDATE `anope_cs_levels` SET `level` = " << what << " WHERE `channel` = " << mysqlpp::quote << ci->name << " AND `position` = " << pos;
			ExecuteQuery(query);
		}
		else
			for (int i = 0; i < CA_SIZE; ++i)
			{
				query << "UPDATE `anope_cs_levels` SET `level` = " << ci->levels[i] << " WHERE `channel` = " << mysqlpp::quote << ci->name << " AND `position` = " << i;
				ExecuteQuery(query);
			}
	}

	void OnChanForbidden(ChannelInfo *ci)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_cs_info` (name, time_registered, last_used, flags, forbidby, forbidreason) VALUES (";
		query << mysqlpp::quote << ci->name << ", " << ci->time_registered << ", " << ci->last_used << ", '" << BuildFlagsList(ci) << "', " << mysqlpp::quote << ci->forbidby << ", " << mysqlpp::quote << ci->forbidreason << ")";
		ExecuteQuery(query);
	}

	void OnDelChan(ChannelInfo *ci)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_cs_access` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_cs_akick` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_cs_info` WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_cs_levels` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "DELETE From `anope_cs_ttb` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "DELETE FROM `anope_bs_badwords` WHERE `channel` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		mysqlpp::Query query(me->Con);
		std::string flags = BuildFlagsList(ci), mlockon = GetMLockOn(ci), mlockoff = GetMLockOff(ci), mlockparams = GetMLockParams(ci);
		query << "INSERT DELAYED INTO `anope_cs_info` (name, founder, successor, descr, url, email, time_registered, last_used, last_topic,  last_topic_setter, last_topic_time, flags, forbidby, forbidreason, bantype, mlock_on, mlock_off, mlock_params, entry_message, memomax, botnick, botflags, capsmin, capspercent, floodlines, floodsecs, repeattimes) VALUES(";
		query << mysqlpp::quote << ci->name << ", " << mysqlpp::quote << (ci->founder ? ci->founder->display : "") << ", ";
		query << mysqlpp::quote << (ci->successor ? ci->successor->display : "") << ", " << mysqlpp::quote << ci->desc << ", ";
		query << mysqlpp::quote << (ci->url ? ci->url : "") << ", " << mysqlpp::quote << (ci->email ? ci->email : "") << ", ";
		query << ci->time_registered << ", " << ci->last_used << ", " << mysqlpp::quote << (ci->last_topic ? ci->last_topic : "");
		query << ", " << mysqlpp::quote << (!ci->last_topic_setter.empty() ? ci->last_topic_setter : "");
		query << ", " << ci->last_topic_time << ", '" << (!flags.empty() ? flags : "") << "', ";
		query << mysqlpp::quote << (ci->forbidby ? ci->forbidby : "") << ", " << mysqlpp::quote << (ci->forbidreason ? ci->forbidreason : "") << ", " <<  ci->bantype << ", '";
		query << mlockon << "', '" << mlockoff << "', '";
		query << mlockparams << "', " << mysqlpp::quote << (ci->entry_message ? ci->entry_message : "") << ", ";
		query << ci->memos.memomax << ", " << mysqlpp::quote << (ci->bi ? ci->bi->nick : "") << ", '" << GetBotFlags(ci->botflags);
		query << "', " << ci->capsmin << ", " << ci->capspercent << ", " << ci->floodlines;
		query << ", " << ci->floodsecs << ", " << ci->repeattimes << ") ";
		query << "ON DUPLICATE KEY UPDATE founder=VALUES(founder), successor=VALUES(successor), descr=VALUES(descr), url=VALUES(url),  email=VALUES(email), time_registered=VALUES(time_registered), last_used=VALUES(last_used), last_topic=VALUES(last_topic), last_topic_setter=VALUES(last_topic_setter),  last_topic_time=VALUES(last_topic_time), flags=VALUES(flags), forbidby=VALUES(forbidby), forbidreason=VALUES(forbidreason), bantype=VALUES(bantype), mlock_on=VALUES(mlock_on), mlock_off=VALUES(mlock_off), mlock_params=VALUES(mlock_params), entry_message=VALUES(entry_message), memomax=VALUES(memomax), botnick=VALUES(botnick), botflags=VALUES(botflags), capsmin=VALUES(capsmin), capspercent=VALUES(capspercent), floodlines=VALUES(floodlines), floodsecs=VALUES(floodsecs), repeattimes=VALUES(repeattimes)";
		ExecuteQuery(query);
	}

	void OnChanSuspend(ChannelInfo *ci)
	{
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_cs_info` SET `flags` = '" << BuildFlagsList(ci) << "' WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_info` SET `forbidby` = " << mysqlpp::quote << ci->forbidby << " WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_info` SET `forbidreason` = " << mysqlpp::quote << ci->forbidreason << " WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
	}

	void OnAkickAdd(ChannelInfo *ci, AutoKick *ak)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_cs_akick` (channel, flags, mask, reason, creator, created, last_used) VALUES(";
		query << mysqlpp::quote << ci->name << ", '";
		if (ak->HasFlag(AK_ISNICK))
			query << "ISNICK ";
		if (ak->HasFlag(AK_STUCK))
			query << "STUCK ";
		query << "', " << mysqlpp::quote << (ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask) << ", ";
		query << mysqlpp::quote << ak->reason << ", " << mysqlpp::quote << ak->creator << ", " << ak->addtime;
		query << ", " << ak->last_used << ")";
		ExecuteQuery(query);
	}

	void OnAkickDel(ChannelInfo *ci, AutoKick *ak)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_cs_akick` WHERE `channel`= " << mysqlpp::quote << ci->name << " AND `mask` = " << mysqlpp::quote << (ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask);
		ExecuteQuery(query);
	}

	void OnBotCreate(BotInfo *bi)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES(";
		query << mysqlpp::quote << bi->nick << ", " << mysqlpp::quote << bi->user << ", " << mysqlpp::quote << bi->host << ", ";
		query << mysqlpp::quote << bi->real << ", '" << GetBotServFlags(bi) << "', " << bi->created << ", " << bi->chancount << ") ";
		query << "ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(chancount)";
		ExecuteQuery(query);
	}

	void OnBotChange(BotInfo *bi)
	{
		OnBotCreate(bi);
	}

	void OnBotDelete(BotInfo *bi)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_bs_core` WHERE `nick` = " << mysqlpp::quote << bi->nick;
		ExecuteQuery(query);
		query << "UPDATE `anope_cs_info` SET `botnick` = '' WHERE `botnick` = " << mysqlpp::quote << bi->nick;
		ExecuteQuery(query);
	}

	EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi)
	{
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_cs_info` SET `botnick` = " << mysqlpp::quote << bi->nick << " WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		return EVENT_CONTINUE;
	}

	EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci)
	{
		mysqlpp::Query query(me->Con);
		query << "UPDATE `anope_cs_info` SET `botnick` = '' WHERE `name` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
		return EVENT_CONTINUE;
	}

	void OnBadWordAdd(ChannelInfo *ci, BadWord *bw)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_bs_badwords` (channel, word, type) VALUES(" << mysqlpp::quote << ci->name << ", " << mysqlpp::quote << bw->word << ", '";
		switch (bw->type)
		{
			case BW_SINGLE:
				query << "SINGLE";
				break;
			case BW_START:
				query << "START";
				break;
			case BW_END:
				query << "END";
				break;
			default:
				query << "ANY";
		}
		query << "') ON DUPLICATE KEY UPDATE channel=VALUES(channel), word=VALUES(word), type=VALUES(type)";
		ExecuteQuery(query);
	}

	void OnBadWordDel(ChannelInfo *ci, BadWord *bw)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_bs_badwords` WHERE `channel` = " << mysqlpp::quote << ci->name << " AND `word` = " << mysqlpp::quote << bw->word << " AND `type` = '";
		switch (bw->type)
		{
			case BW_SINGLE:
				query << "SINGLE";
				break;
			case BW_START:
				query << "START";
				break;
			case BW_END:
				query << "END";
				break;
			default:
				query << "ANY";
		}
		query << "'";
		ExecuteQuery(query);
	}

	void OnMemoSend(User *, NickCore *nc, Memo *m)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_ms_info` (receiver, number, flags, time, sender, text, serv) VALUES(";
		query << mysqlpp::quote << nc->display << ", " << m->number << ", '" << BuildFlagsList(m) << "', " << m->time << ", ";
		query << mysqlpp::quote << m->sender << ", " << mysqlpp::quote << m->text << ", 'NICK')";
		ExecuteQuery(query);
	}

	void OnMemoSend(User *, ChannelInfo *ci, Memo *m)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_ms_info` (receiver, number, flags, time, sender, text, serv) VALUES(";
		query << mysqlpp::quote << ci->name << ", " << m->number << ", '" << BuildFlagsList(m) << "', " << m->time << ", ";
		query << mysqlpp::quote << m->sender << ", " << mysqlpp::quote << m->text << ", 'CHAN')";
		ExecuteQuery(query);
	}

	void OnMemoDel(NickCore *nc, MemoInfo *mi, int number)
	{
		mysqlpp::Query query(me->Con);
		if (number)
			query << "DELETE FROM `anope_ms_info` WHERE `receiver` = " << mysqlpp::quote << nc->display << " AND `number` = " << number;
		else
			query << "DELETE FROM `anope_ms_info` WHERE `receiver` = " << mysqlpp::quote << nc->display;
		ExecuteQuery(query);
	}

	void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, int number)
	{
		mysqlpp::Query query(me->Con);
		if (number)
			query << "DELETE FROM `anope_ms_info` WHERE `receiver` = " << mysqlpp::quote << ci->name << " AND `number` = " << number;
		else
			query << "DELETE FROM `anope_ms_info` WHERE `receiver` = " << mysqlpp::quote << ci->name;
		ExecuteQuery(query);
	}

	EventReturn OnAddAkill(User *, XLine *ak)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_os_akills` (user, host, xby, reason, seton, expire) VALUES(";
		query << mysqlpp::quote << ak->GetUser().c_str() << ", " << mysqlpp::quote << ak->GetHost().c_str() << ", " << mysqlpp::quote << ak->By.c_str();
		query << ", " << mysqlpp::quote << ak->Reason << ", " << ak->Created << ", " << ak->Expires << ")";
		ExecuteQuery(query);
		return EVENT_CONTINUE;
	}

	void OnDelAkill(User *, XLine *ak)
	{
		mysqlpp::Query query(me->Con);
		if (ak)
			query << "DELETE FROM `anope_os_akills` WHERE `host` = " << mysqlpp::quote << ak->GetHost().c_str();
		else
			query << "TRUNCATE TABLE `anope_os_akills`";
		ExecuteQuery(query);
	}

	EventReturn OnExceptionAdd(User *, Exception *ex)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_os_exceptions` (mask, slimit, who, reason, time, expires) VALUES(";
		query << mysqlpp::quote << ex->mask << ", " << ex->limit << ", " << mysqlpp::quote << ex->who << ", ";
		query << mysqlpp::quote << ex->reason << ", " << ex->time << ", " << ex->expires << ")";
		ExecuteQuery(query);
		return EVENT_CONTINUE;
	}

	void OnExceptionDel(User *, Exception *ex)
	{
		mysqlpp::Query query(me->Con);
		query << "DELETE FROM `anope_os_exceptions` WHERE `mask` = " << mysqlpp::quote << ex->mask;
		ExecuteQuery(query);
	}

	EventReturn OnAddXLine(User *, XLine *x, XLineType Type)
	{
		mysqlpp::Query query(me->Con);
		query << "INSERT DELAYED INTO `anope_os_sxlines` (type, mask, xby, reason, seton, expire) VALUES('";
		query << (Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) << "', ";
		query << mysqlpp::quote << x->Mask.c_str() << ", " << mysqlpp::quote << x->By.c_str() << ", " << mysqlpp::quote << x->Reason;
		query << ", " << x->Created << ", " << x->Expires << ")";
		ExecuteQuery(query);
		return EVENT_CONTINUE;
	}

	void OnDelXLine(User *, XLine *x, XLineType Type)
	{
		mysqlpp::Query query(me->Con);
		if (x)
		{
			query << "DELETE FROM `anope_os_xlines` WHERE `mask` = " << mysqlpp::quote << x->Mask.c_str() << " AND `type` = '";
			query << (Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) << "'";
		}
		else
			query << "DELETE FROM `anope_os_xlines` WHERE `type` = '" << (Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) << "'";
		ExecuteQuery(query);
	}
};

MODULE_INIT(DBMySQLWrite)
