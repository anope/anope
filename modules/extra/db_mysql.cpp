#include "module.h"
#include "sql.h"

struct NickAliasFlagInfo
{
	Anope::string Name;
	NickNameFlag Flag;
};

NickAliasFlagInfo NickAliasFlags[] = {
	{"FORBIDDEN", NS_FORBIDDEN},
	{"NOEXPIRE", NS_NO_EXPIRE},
	{"", static_cast<NickNameFlag>(-1)}
};

struct NickCoreFlagInfo
{
	Anope::string Name;
	NickCoreFlag Flag;
};

NickCoreFlagInfo NickCoreFlags[] = {
	{"KILLPROTECT", NI_KILLPROTECT},
	{"SECURE", NI_SECURE},
	{"MSG", NI_MSG},
	{"MEMO_HARDMAX", NI_MEMO_HARDMAX},
	{"MEMO_SIGNON", NI_MEMO_SIGNON},
	{"MEMO_RECEIVE", NI_MEMO_RECEIVE},
	{"PRIVATE", NI_PRIVATE},
	{"HIDE_EMAIL", NI_HIDE_EMAIL},
	{"HIDE_MASK", NI_HIDE_MASK},
	{"HIDE_QUIT", NI_HIDE_QUIT},
	{"KILL_QUICK", NI_KILL_QUICK},
	{"KILL_IMMED", NI_KILL_IMMED},
	{"MEMO_MAIL", NI_MEMO_MAIL},
	{"HIDE_STATUS", NI_HIDE_STATUS},
	{"SUSPENDED", NI_SUSPENDED},
	{"AUTOOP", NI_AUTOOP},
	{"FORBIDDEN", NI_FORBIDDEN},
	{"", static_cast<NickCoreFlag>(-1)}
};

struct BotServFlagInfo
{
	Anope::string Name;
	BotFlag Flag;
};

BotServFlagInfo BotServFlags[] = {
	{"PRIVATE", BI_PRIVATE},
	{"", static_cast<BotFlag>(-1)}
};

struct BotFlagInfo
{
	Anope::string Name;
	BotServFlag Flag;
};

BotFlagInfo BotFlags[] = {
	{"DONTKICKOPS", BS_DONTKICKOPS},
	{"DONTKICKVOICES", BS_DONTKICKVOICES},
	{"FANTASY", BS_FANTASY},
	{"SYMBIOSIS", BS_SYMBIOSIS},
	{"GREET", BS_GREET},
	{"NOBOT", BS_NOBOT},
	{"KICK_BOLDS", BS_KICK_BOLDS},
	{"KICK_COLORS", BS_KICK_COLORS},
	{"KICK_REVERSES", BS_KICK_REVERSES},
	{"KICK_UNDERLINES", BS_KICK_UNDERLINES},
	{"KICK_BADWORDS", BS_KICK_BADWORDS},
	{"KICK_CAPS", BS_KICK_CAPS},
	{"KICK_FLOOD", BS_KICK_FLOOD},
	{"KICK_REPEAT", BS_KICK_REPEAT},
	{"KICK_ITALICS", BS_KICK_ITALICS},
	{"", static_cast<BotServFlag>(-1)}
};

struct ChannelFlagInfo
{
	Anope::string Name;
	ChannelInfoFlag Flag;
};

ChannelFlagInfo ChannelFlags[] = {
	{"KEEPTOPIC", CI_KEEPTOPIC},
	{"SECUREOPS", CI_SECUREOPS},
	{"PRIVATE", CI_PRIVATE},
	{"TOPICLOCK", CI_TOPICLOCK},
	{"RESTRICTED", CI_RESTRICTED},
	{"PEACE", CI_PEACE},
	{"SECURE", CI_SECURE},
	{"FORBIDDEN", CI_FORBIDDEN},
	{"NO_EXPIRE", CI_NO_EXPIRE},
	{"MEMO_HARDMAX", CI_MEMO_HARDMAX},
	{"OPNOTICE", CI_OPNOTICE},
	{"SECUREFOUNDER", CI_SECUREFOUNDER},
	{"SIGNKICK", CI_SIGNKICK},
	{"SIGNKICK_LEVEL", CI_SIGNKICK_LEVEL},
	{"XOP", CI_XOP},
	{"SUSPENDED", CI_SUSPENDED},
	{"PERSIST", CI_PERSIST},
	{"", static_cast<ChannelInfoFlag>(-1)}
};

struct MemoFlagInfo
{
	Anope::string Name;
	MemoFlag Flag;
};

MemoFlagInfo MemoFlags[] = {
	{"UNREAD", MF_UNREAD},
	{"RECEIPT", MF_RECEIPT},
	{"NOTIFYS", MF_NOTIFYS},
	{"", static_cast<MemoFlag>(-1)}
};

static std::vector<Anope::string> MakeVector(const Anope::string &buf)
{
	Anope::string s;
	spacesepstream sep(buf);
	std::vector<Anope::string> params;

	while (sep.GetToken(s))
	{
		if (s[0] == ':')
		{
			s.erase(s.begin());
			if (!s.empty() && !sep.StreamEnd())
				params.push_back(s + " " + sep.GetRemaining());
			else if (!s.empty())
				params.push_back(s);
		}
		else
			params.push_back(s);
	}

	return params;
}

static Anope::string BuildFlagsList(ChannelInfo *ci)
{
	Anope::string ret;

	for (int i = 0; ChannelFlags[i].Flag != -1; ++i)
		if (ci->HasFlag(ChannelFlags[i].Flag))
			ret += " " + ChannelFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static Anope::string BuildFlagsList(NickAlias *na)
{
	Anope::string ret;

	for (int i = 0; NickAliasFlags[i].Flag != -1; ++i)
		if (na->HasFlag(NickAliasFlags[i].Flag))
			ret += " " + NickAliasFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static Anope::string BuildFlagsList(NickCore *nc)
{
	Anope::string ret;

	for (int i = 0; NickCoreFlags[i].Flag != -1; ++i)
		if (nc->HasFlag(NickCoreFlags[i].Flag))
			ret += " " + NickCoreFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static Anope::string BuildFlagsList(Memo *m)
{
	Anope::string ret;

	for (int i = 0; MemoFlags[i].Flag != -1; ++i)
		if (m->HasFlag(MemoFlags[i].Flag))
			ret += " " + MemoFlags[i].Name;

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static Anope::string MakeMLock(ChannelInfo *ci, bool status)
{
	Anope::string ret;

	if ((!Me || !Me->IsSynced()) && ci->GetExtRegular(status ? "db_mlock_modes_on" : "db_mlock_modes_off", ret))
		;
	else
	{
		for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
		{
			ChannelMode *cm = it->second;
			if (ci->HasMLock(cm->Name, status))
				ret += " " + cm->NameAsString;
		}

		if (!ret.empty())
			ret.erase(ret.begin());
	}

	return ret;
}

static inline Anope::string GetMLockOn(ChannelInfo *ci)
{
	return MakeMLock(ci, true);
}

static inline Anope::string GetMLockOff(ChannelInfo *ci)
{
	return MakeMLock(ci, false);
}

static Anope::string GetMLockParams(ChannelInfo *ci)
{
	Anope::string ret;

	std::vector<std::pair<Anope::string, Anope::string> > oldparams;;
	if ((!Me || !Me->IsSynced()) && ci->GetExtRegular("db_mlp", oldparams))
	{
		for (std::vector<std::pair<Anope::string, Anope::string> >::iterator it = oldparams.begin(), it_end = oldparams.end(); it != it_end; ++it)
		{
			ret += " " + it->first + " " + it->second;
		}
	}
	else
	{
		for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
		{
			ChannelMode *cm = it->second;

			Anope::string param;
			if (ci->GetParam(cm->Name, param))
				ret += " " + cm->NameAsString + " " + param;
		}
	}

	if (!ret.empty())
		ret.erase(ret.begin());

	return ret;
}

static Anope::string GetBotFlags(Flags<BotServFlag>& Flags)
{
	Anope::string buf;

	for (int i = 0; BotFlags[i].Flag != -1; ++i)
		if (Flags.HasFlag(BotFlags[i].Flag))
			buf += " " + BotFlags[i].Name;

	if (!buf.empty())
		buf.erase(buf.begin());

	return buf;
}

static Anope::string GetBotServFlags(BotInfo *bi)
{
	Anope::string buf;

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

static void Write(const Anope::string &data);
static void WriteNickMetadata(const Anope::string &key, const Anope::string &data);
static void WriteCoreMetadata(const Anope::string &key, const Anope::string &data);
static void WriteChannelMetadata(const Anope::string &key, const Anope::string &data);
static void WriteBotMetadata(const Anope::string &key, const Anope::string &data);

class CommandSQLSync : public Command
{
 public:
	CommandSQLSync() : Command("SQLSYNC", 0, 0, "operserv/sqlsync")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params);

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_SYNC);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_SQLSYNC);
	}
};

class MySQLInterface : public SQLInterface
{
 public:
	MySQLInterface(Module *o) : SQLInterface(o) { }

	void OnResult(const SQLResult &r);

	void OnError(const SQLResult &r);
};

class DBMySQL;
static DBMySQL *me;
class DBMySQL : public Module
{
 private:
	CommandSQLSync commandsqlsync;
	MySQLInterface interface;
	service_reference<SQLProvider> SQL;

 public:
	time_t lastwarn;
	bool ro;

	void RunQuery(const Anope::string &query)
	{
		if (SQL)
		{
			if (readonly && this->ro)
			{
				readonly = this->ro = false;
				ircdproto->SendGlobops(OperServ, "Found SQL again, going out of readonly mode...");
			}

			SQL->Run(&interface, query);
		}
		else
		{
			if (Anope::CurTime - Config->UpdateTimeout > lastwarn)
			{
				ircdproto->SendGlobops(OperServ, "Unable to locate SQL reference, is m_mysql loaded? Going to readonly...");
				readonly = this->ro = true;
				this->lastwarn = Anope::CurTime;
			}
		}
	}

	const Anope::string Escape(const Anope::string &query)
	{
		return SQL ? SQL->Escape(query) : query;
	}

	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), interface(this), SQL("mysql/main")
	{
		me = this;

		this->SetType(DATABASE);

		this->lastwarn = 0;
		this->ro = false;

		Implementation i[] = {
			I_OnLoadDatabase, I_OnServerConnect
		};
		ModuleManager::Attach(i, this, 2);

		this->AddCommand(OperServ, &commandsqlsync);

		if (uplink_server)
			OnServerConnect();
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

	EventReturn OnLoadDatabase()
	{
		if (!SQL)
		{
			Log() << "Error, unable to find service reference for SQL, is m_mysql loaded and configured properly?";
			return EVENT_CONTINUE;
		}

		SQLResult r = SQL->RunQuery("SELECT * FROM `anope_ns_core`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = new NickCore(r.Get(i, "display"));
			nc->pass = r.Get(i, "pass");
			nc->email = r.Get(i, "email");
			nc->greet = r.Get(i, "greet");

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			while (sep.GetToken(buf))
			{
				for (int j = 0; NickCoreFlags[j].Flag != -1; ++j)
				{
					if (NickCoreFlags[j].Name == buf)
					{
						nc->SetFlag(NickCoreFlags[j].Flag);
						break;
					}
				}
			}

			nc->language = r.Get(i, "language");
			nc->channelcount = r.Get(i, "channelcount").is_number_only() ? convertTo<int>(r.Get(i, "channelcount")) : 0;
			nc->memos.memomax = r.Get(i, "memomax").is_number_only() ? convertTo<int>(r.Get(i, "memomax")) : 20;
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ns_access`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = findcore(r.Get(i, "display"));
			if (!nc)
			{
				Log() << "MySQL: Got NickCore access entry for nonexistant core " << r.Get(i, "display");
				continue;
			}

			nc->AddAccess(r.Get(i, "access"));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ns_core_metadata`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = findcore(r.Get(i, "display"));
			if (!nc)
			{
				Log() << "MySQL: Got NickCore access entry for nonexistant core " << r.Get(i, "display");
				continue;
			}

			EventReturn MOD_RESULT;;
			std::vector<Anope::string> Params = MakeVector(r.Get(i, "value"));
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nc, r.Get(i, "name"), Params));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ns_alias`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = findcore(r.Get(i, "display"));
			if (!nc)
			{
				Log() << "MySQL: Got NickAlias for nick " << r.Get(i, "nick") << " with nonexistant core " << r.Get(i, "display");
				continue;
			}

			NickAlias *na = new NickAlias(r.Get(i, "nick"), nc);
			na->last_quit = r.Get(i, "last_quit");
			na->last_realname = r.Get(i, "last_realname");
			na->last_usermask = r.Get(i, "last_usermask");
			na->time_registered = r.Get(i, "time_registered").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time_registered")) : Anope::CurTime;
			na->last_seen = r.Get(i, "last_seen").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_seen")) : Anope::CurTime;

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			while (sep.GetToken(buf))
			{
				for (int j = 0; NickAliasFlags[j].Flag != -1; ++j)
				{
					if (NickAliasFlags[j].Name == buf)
					{
						na->SetFlag(NickAliasFlags[j].Flag);
						break;
					}
				}
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ns_alias_metadata`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickAlias *na = findnick(r.Get(i, "nick"));
			if (!na)
			{
				Log() << "MySQL: Got metadata for nonexistant nick " << r.Get(i, "nick");
				continue;
			}

			EventReturn MOD_RESULT;
			std::vector<Anope::string> Params = MakeVector(r.Get(i, "value"));
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, na->nick, Params));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_bs_core`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			BotInfo *bi = findbot(r.Get(i, "nick"));
			if (!bi)
				bi = new BotInfo(r.Get(i, "nick"), r.Get(i, "user"), r.Get(i, "host"));
			bi->realname = r.Get(i, "rname");
			bi->created = r.Get(i, "created").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "creeated")) : Anope::CurTime;

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			while (sep.GetToken(buf))
			{
				for (int j = 0; BotServFlags[j].Flag != -1; ++j)
				{
					if (buf == BotServFlags[j].Name)
					{
						bi->SetFlag(BotServFlags[j].Flag);
						break;
					}
				}
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_bs_info_metadata`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			BotInfo *bi = findbot(r.Get(i, "botname"));
			if (!bi)
			{
				Log() << "MySQL: BotInfo metadata for nonexistant bot " << r.Get(i, "botname");
				continue;
			}

			EventReturn MOD_RESULT;
			std::vector<Anope::string> Params = MakeVector(r.Get(i, "value"));
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(bi, bi->nick, Params));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_info`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = NULL;

			if (!r.Get(i, "founder").empty())
			{
				nc = findcore(r.Get(i, "founder"));
				if (!nc)
				{
					Log() << "MySQL: Channel " << r.Get(i, "name") << " with nonexistant founder " << r.Get(i, "founder");
					continue;
				}

				ChannelInfo *ci = new ChannelInfo(r.Get(i, "name"));
				ci->founder = nc;
				if (!r.Get(i, "successor").empty())
					ci->successor = findcore(r.Get(i, "successor"));
				ci->desc = r.Get(i, "descr");
				ci->time_registered = r.Get(i, "time_registered").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time_registered")) : Anope::CurTime;
				ci->last_used = r.Get(i, "last_used").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_used")) : Anope::CurTime;
				ci->last_topic = r.Get(i, "last_topic");
				ci->last_topic_setter = r.Get(i, "last_topic_setter");
				ci->last_topic_time = r.Get(i, "last_topic_time").is_number_only() ? convertTo<int>(r.Get(i, "last_topic_time")) : Anope::CurTime;
				ci->forbidby = r.Get(i, "forbidby");
				ci->forbidreason = r.Get(i, "forbidreason");
				ci->bantype = r.Get(i, "bantype").is_number_only() ? convertTo<int>(r.Get(i, "bantype")) : 2;
				ci->entry_message = r.Get(i, "entry_message");
				ci->memos.memomax = r.Get(i, "memomax").is_number_only() ? convertTo<int>(r.Get(i, "memomax")) : 20;
				ci->capsmin = r.Get(i, "capsmin").is_number_only() ? convertTo<int>(r.Get(i, "capsmin")) : 0;
				ci->capspercent = r.Get(i, "capspercent").is_number_only() ? convertTo<int>(r.Get(i, "capspercent")) : 0;
				ci->floodlines = r.Get(i, "floodlines").is_number_only() ? convertTo<int>(r.Get(i, "floodlines")) : 0;
				ci->floodsecs = r.Get(i, "floodsecs").is_number_only() ? convertTo<int>(r.Get(i, "floodsecs")) : 0;
				ci->repeattimes = r.Get(i, "repeattimes").is_number_only() ? convertTo<int>(r.Get(i, "repeattimes")) : 0;
				ci->bi = findbot(r.Get(i, "botnick"));
				if (ci->bi && !r.Get(i, "botflags").empty())
				{
					spacesepstream sep(r.Get(i, "botflags"));
					Anope::string buf;
					while (sep.GetToken(buf))
					{
						for (int j = 0; BotFlags[j].Flag != -1; ++j)
						{
							if (buf == BotFlags[j].Name)
							{
								ci->botflags.SetFlag(BotFlags[j].Flag);
								break;
							}
						}
					}
				}

				if (!r.Get(i, "mlock_on").empty())
				{
					std::vector<Anope::string> modes;

					spacesepstream sep(r.Get(i, "mlock_on"));
					Anope::string buf;
					while (sep.GetToken(buf))
						modes.push_back(buf);

					ci->Extend("db_mlock_modes_on", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));
				}
				if (!r.Get(i, "mlock_off").empty())
				{
					std::vector<Anope::string> modes;

					spacesepstream sep(r.Get(i, "mlock_off"));
					Anope::string buf;
					while (sep.GetToken(buf))
						modes.push_back(buf);

					ci->Extend("db_mlock_modes_off", new ExtensibleItemRegular<std::vector<Anope::string> >(modes));
				}
				if (!r.Get(i, "mlock_params").empty())
				{
					std::vector<std::pair<Anope::string, Anope::string> > mlp;

					spacesepstream sep(r.Get(i, "mlock_params"));
					Anope::string buf, buf2;
					while (sep.GetToken(buf) && sep.GetToken(buf2))
						mlp.push_back(std::make_pair(Anope::string(buf), Anope::string(buf2)));

					ci->Extend("db_mlp", new ExtensibleItemRegular<std::vector<std::pair<Anope::string, Anope::string> > >(mlp));
				}

				if (!r.Get(i, "flags").empty())
				{
					spacesepstream sep(r.Get(i, "flags"));
					Anope::string buf;

					while (sep.GetToken(buf))
					{
						for (int j = 0; ChannelFlags[j].Flag != -1; ++j)
						{
							if (buf == ChannelFlags[j].Name)
							{
								ci->SetFlag(ChannelFlags[j].Flag);
								break;
							}
						}
					}
				}
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_ttb`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel ttb for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			ci->ttb[atoi(r.Get(i, "ttb_id").c_str())] = atoi(r.Get(i, "value").c_str());
		}

		r = SQL->RunQuery("SELECT * FROM `anope_bs_badwords`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel badwords entry for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			BadWordType BWTYPE = BW_ANY;
			if (r.Get(i, "type").equals_cs("SINGLE"))
				BWTYPE = BW_SINGLE;
			else if (r.Get(i, "type").equals_cs("START"))
				BWTYPE = BW_START;
			else if (r.Get(i, "type").equals_cs("END"))
				BWTYPE = BW_END;
			ci->AddBadWord(r.Get(i, "word"), BWTYPE);
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_access`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel access entry for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			NickCore *nc = findcore(r.Get(i, "display"));
			if (!nc)
			{
				Log() << "MySQL: Channel access entry for " << ci->name << " with nonexistant nick " << r.Get(i, "display");
				continue;
			}

			ci->AddAccess(nc, atoi(r.Get(i, "level").c_str()), r.Get(i, "creator"), (r.Get(i, "last_seen").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_seen")) : Anope::CurTime));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_akick`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel access entry for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			NickCore *nc = NULL;
			spacesepstream sep(r.Get(i, "flags"));
			Anope::string flag, mask;
			bool stuck = false;
			while (sep.GetToken(flag))
			{
				if (flag.equals_cs("ISNICK"))
					nc = findcore(r.Get(i, "mask"));
				else if (flag.equals_cs("STUCK"))
					stuck = true;

				AutoKick *ak;
				if (nc)
					ak = ci->AddAkick(r.Get(i, "creator"), nc, r.Get(i, "reason"), atol(r.Get(i, "created").c_str()), atol(r.Get(i, "last_used").c_str()));
				else
					ak = ci->AddAkick(r.Get(i, "creator"), r.Get(i, "mask"), r.Get(i, "reason"), atol(r.Get(i, "created").c_str()), atol(r.Get(i, "last_used").c_str()));
				if (stuck)
					ak->SetFlag(AK_STUCK);
				if (nc)
					ak->SetFlag(AK_ISNICK);
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_levels`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel level entry for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			ci->levels[atoi(r.Get(i, "position").c_str())] = atoi(r.Get(i, "level").c_str());
		}

		r = SQL->RunQuery("SELECT * FROM `anope_cs_info_metadata");
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel metadata for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			EventReturn MOD_RESULT;
			std::vector<Anope::string> Params = MakeVector(r.Get(i, "value"));
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, ci->name, Params));
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ns_request`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickRequest *nr = new NickRequest(r.Get(i, "nick"));
			nr->password = r.Get(i, "passcode");
			nr->password = r.Get(i, "password");
			nr->email = r.Get(i, "email");
			nr->requested = r.Get(i, "requested").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "requested")) : Anope::CurTime;
		}

		r = SQL->RunQuery("SELECT * FROM `anope_ms_info`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			MemoInfo *mi = NULL;
			if (r.Get(i, "serv").equals_cs("NICK"))
			{
				NickCore *nc = findcore(r.Get(i, "receiver"));
				if (nc)
					mi = &nc->memos;
			}
			else if (r.Get(i, "serv").equals_cs("CHAN"))
			{
				ChannelInfo *ci = cs_findchan(r.Get(i, "receiver"));
				if (ci)
					mi = &ci->memos;
			}
			if (mi)
			{
				Memo *m = new Memo();
				mi->memos.push_back(m);
				m->sender = r.Get(i, "sender");
				if (mi->memos.size() > 1)
				{
					m->number = mi->memos[mi->memos.size() - 1]->number + 1;
					if (m->number < 1)
					{
						for (unsigned j = 0; j < mi->memos.size(); ++j)
							mi->memos[j]->number = j + 1;
					}
				}
				else
					m->number = 1;
				m->time = r.Get(i, "time").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time")) : Anope::CurTime;
				m->text = r.Get(i, "text");

				if (!r.Get(i, "flags").empty())
				{
					spacesepstream sep(r.Get(i, "flags"));
					Anope::string buf;
					while (sep.GetToken(buf))
					{
						for (unsigned j = 0; MemoFlags[j].Flag != -1; ++j)
						{
							if (MemoFlags[j].Name == buf)
							{
								m->SetFlag(MemoFlags[j].Flag);
								break;
							}
						}
					}
				}
			}
		}

		if (SQLine)
		{
			r = SQL->RunQuery("SELECT * FROM `anope_os_akills`");
			for (int i = 0; i < r.Rows(); ++i)
			{
				Anope::string user = r.Get(i, "user");
				Anope::string host = r.Get(i, "host");
				Anope::string by = r.Get(i, "by");
				Anope::string reason = r.Get(i, "reason");
				time_t seton = r.Get(i, "seton").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "seton")) : Anope::CurTime;
				time_t expires = r.Get(i, "expires").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "expires")) : Anope::CurTime;

				XLine *x = SGLine->Add(NULL, NULL, user + "@" + host, expires, reason);
				if (x)
				{
					x->By = by;
					x->Created = seton;
				}
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_os_xlines`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			Anope::string mask = r.Get(i, "mask");
			Anope::string by = r.Get(i, "xby");
			Anope::string reason = r.Get(i, "reason");
			time_t seton = r.Get(i, "seton").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "seton")) : Anope::CurTime;
			time_t expires = r.Get(i, "expires").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "expires")) : Anope::CurTime;

			XLine *x = NULL;
			if (SNLine && r.Get(i, "type").equals_cs("SNLINE"))
				SNLine->Add(NULL, NULL, mask, expires, reason);
			else if (SQLine && r.Get(i, "type").equals_cs("SQLINE"))
				SQLine->Add(NULL, NULL, mask, expires, reason);
			else if (SZLine && r.Get(i, "type").equals_cs("SZLINE"))
				SZLine->Add(NULL, NULL, mask, expires, reason);
			if (x)
			{
				x->By = by;
				x->Created = seton;
			}
		}

		r = SQL->RunQuery("SELECT * FROM `anope_extra`");
		for (int i = 0; i < r.Rows(); ++i)
		{
			std::vector<Anope::string> params = MakeVector(r.Get(i, "data"));
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		}

		return EVENT_STOP;
	}

	EventReturn OnSaveDatabase()
	{
		this->RunQuery("TRUNCATE TABLE `anope_os_core`");

		this->RunQuery("INSERT DELAYED INTO `anope_os_core` (maxusercnt, maxusertime, akills_count, snlines_count, sqlines_count, szlines_count) VALUES( " + stringify(maxusercnt) + ", " + stringify(maxusertime) + ", " + stringify(SGLine ? SGLine->GetCount() : 0) + ", " + stringify(SQLine ? SQLine->GetCount() : 0) + ", " + stringify(SNLine ? SNLine->GetCount() : 0) + ", " + stringify(SZLine ? SZLine->GetCount() : 0) + ")");
		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			CurCore = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteCoreMetadata, CurCore));
		}

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			CurNick = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteNickMetadata, CurNick));
		}

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			CurChannel = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteChannelMetadata, CurChannel));
		}

		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			CurBot = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteBotMetadata, CurBot));

			/* This is for the core bots, bots added by users are already handled by an event */
			this->RunQuery("INSERT DELAYED INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES('" + this->Escape(CurBot->nick) + "', '" + this->Escape(CurBot->GetIdent()) + "', '" + this->Escape(CurBot->host) + "', '" + this->Escape(CurBot->realname) + "', '" + GetBotServFlags(CurBot) + "', " + stringify(CurBot->created) + ", " + stringify(CurBot->chancount) + ") ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(created)");
		}

		FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		return EVENT_CONTINUE;
	}

	void OnPostCommand(User *u, const Anope::string &service, const Anope::string &command, const std::vector<Anope::string> &params)
	{
		if (service == Config->s_NickServ)
		{
			if (u->Account() && ((command.equals_ci("SET") && !params.empty()) || (command.equals_ci("SASET") && u->Account()->HasCommand("nickserv/saset") && params.size() > 1)))
			{
				Anope::string cmd = (command == "SET" ? params[0] : params[1]);
				NickCore *nc = (command == "SET" ? u->Account() : findcore(params[0]));
				if (!nc)
					return;
				if (cmd.equals_ci("PASSWORD") && params.size() > 1)
				{
					this->RunQuery("UPDATE `anope_ns_core` SET `pass` = '" + this->Escape(nc->pass) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
				}
				else if (cmd.equals_ci("LANGUAGE") && params.size() > 1)
				{
					this->RunQuery("UPDATE `anope_ns_core` SET `language` = '" + this->Escape(nc->language) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
				}
				else if (cmd.equals_ci("EMAIL"))
				{
					this->RunQuery("UPDATE `anope_ns_core` SET `email` = '" + this->Escape(nc->email) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
				}
				else if (cmd.equals_ci("GREET"))
				{
					this->RunQuery("UPDATE `anope_ns_core` SET `greet` = '" + this->Escape(nc->greet) + " WHERE `display` = '" + this->Escape(nc->display) + "'");
				}
				else if (cmd.equals_ci("KILL") || cmd.equals_ci("SECURE") || cmd.equals_ci("PRIVATE") || cmd.equals_ci("MSG") || cmd.equals_ci("HIDE") || cmd.equals_ci("AUTOOP"))
				{
					this->RunQuery("UPDATE `anope_ns_core` SET `flags` = '" + BuildFlagsList(nc) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
				}
			}
		}
		else if (service == Config->s_ChanServ)
		{
			if (command.equals_ci("SET") && u->Account() && params.size() > 1)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (!ci)
					return;
				if (!u->Account()->HasPriv("chanserv/set") && check_access(u, ci, CA_SET))
					return;
				if (params[1].equals_ci("FOUNDER") && ci->founder)
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `founder` = '" + this->Escape(ci->founder->display) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("SUCCESSOR"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `successor` = '" + this->Escape(ci->successor ? ci->successor->display : "") + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("DESC"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `descr` = '" + this->Escape(ci->desc) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("ENTRYMSG"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `entry_message` = '" + this->Escape(ci->entry_message) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("MLOCK"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `mlock_on` = '" + GetMLockOn(ci) + "' WHERE `name` = " + this->Escape(ci->name) + "'");
					this->RunQuery("UPDATE `anope_cs_info` SET `mlock_off` = '" + GetMLockOff(ci) + "' WHERE `name` = " + this->Escape(ci->name) + "'");
					this->RunQuery("UPDATE `anope_cs_info` SET `mlock_params` = '" + GetMLockParams(ci) + "' WHERE `name` = " + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("BANTYPE"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `bantype` = " + stringify(ci->bantype) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
				else if (params[1].equals_ci("KEEPTOPIC") || params[1].equals_ci("TOPICLOCK") || params[1].equals_ci("PRIVATE") || params[1].equals_ci("SECUREOPS") || params[1].equals_ci("SECUREFOUNDER") || params[1].equals_ci("RESTRICTED") || params[1].equals_ci("SECURE") || params[1].equals_ci("SIGNKICK") || params[1].equals_ci("OPNOTICE") || params[1].equals_ci("XOP") || params[1].equals_ci("PEACE") || params[1].equals_ci("PERSIST") || params[1].equals_ci("NOEXPIRE"))
				{
					SQL->RunQuery("UPDATE `anope_cs_info` SET `flags` = '" + BuildFlagsList(ci) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
			}
		}
		else if (service == Config->s_BotServ)
		{
			if (command.equals_ci("KICK") && params.size() > 2)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (!ci)
					return;
				if (!check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
					return;
				if (params[1].equals_ci("BADWORDS") || params[1].equals_ci("BOLDS") || params[1].equals_ci("CAPS") || params[1].equals_ci("COLORS") || params[1].equals_ci("FLOOD") || params[1].equals_ci("REPEAT") || params[1].equals_ci("REVERSES") || params[1].equals_ci("UNDERLINES"))
				{
					if (params[2].equals_ci("ON") || params[2].equals_ci("OFF"))
					{
						for (int i = 0; i < TTB_SIZE; ++i)
						{
							this->RunQuery("INSERT DELAYED INTO `anope_cs_ttb` (channel, ttb_id, value) VALUES('" + this->Escape(ci->name) + "', " + stringify(i) + ", " + stringify(ci->ttb[i]) + ") ON DUPLICATE KEY UPDATE channel=VALUES(channel), ttb_id=VALUES(ttb_id), value=VALUES(value)");
						}
						this->RunQuery("UPDATE `anope_cs_info` SET `botflags` = '" + GetBotFlags(ci->botflags) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");

						if (params[1].equals_ci("CAPS"))
						{
							this->RunQuery("UPDATE `anope_cs_info` SET `capsmin` = " + stringify(ci->capsmin) + ", `capspercent` = " + stringify(ci->capspercent) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
						}
						else if (params[1].equals_ci("FLOOD"))
						{
							this->RunQuery("UPDATE `anope_cs_info` SET `floodlines` = " + stringify(ci->floodlines) + ", `floodsecs` = " + stringify(ci->floodsecs) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
						}
						else if (params[1].equals_ci("REPEAT"))
						{
							this->RunQuery("UPDATE `anope_cs_info` SET `repeattimes` = " + stringify(ci->repeattimes) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
						}
					}
				}
			}
			else if (command.equals_ci("SET") && params.size() > 2)
			{
				ChannelInfo *ci = cs_findchan(params[0]);
				if (ci && !check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
					return;
				BotInfo *bi = NULL;
				if (!ci)
					bi = findbot(params[0]);
				if (bi && params[1].equals_ci("PRIVATE") && u->Account()->HasPriv("botserv/set/private"))
				{
					this->RunQuery("UPDATE `anope_bs_core` SET `flags` = '" + GetBotServFlags(bi) + "' WHERE `nick` = '" + this->Escape(bi->nick) + "'");
				}
				else if (!ci)
					return;
				else if (params[1].equals_ci("DONTKICKOPS") || params[1].equals_ci("DONTKICKVOICES") || params[1].equals_ci("FANTASY") || params[1].equals_ci("GREET") || params[1].equals_ci("SYMBIOSIS") || params[1].equals_ci("NOBOT"))
				{
					this->RunQuery("UPDATE `anope_cs_info` SET `botflags` = '" + GetBotFlags(ci->botflags) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
				}
			}
		}
	}

	void OnNickAddAccess(NickCore *nc, const Anope::string &entry)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ns_access` (display, access) VALUES('" + this->Escape(nc->display) + "', '" + this->Escape(entry) + "')");
	}

	void OnNickEraseAccess(NickCore *nc, const Anope::string &entry)
	{
		this->RunQuery("DELETE FROM `anope_ns_access` WHERE `display` = '" + this->Escape(nc->display) + "' AND `access` = '" + this->Escape(entry) + "'");
	}

	void OnNickClearAccess(NickCore *nc)
	{
		this->RunQuery("DELETE FROM `anope_ns_access` WHERE `display` = '" + this->Escape(nc->display) + "'");
	}

	void OnDelCore(NickCore *nc)
	{
		this->RunQuery("DELETE FROM `anope_cs_access` WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("DELETE FROM `anope_cs_akick` WHERE `mask` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("DELETE FROM `anope_ns_access` WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("DELETE FROM `anope_ns_alias` WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("DELETE FROM `anope_ns_core` WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("DELETE FROM `anope_ms_info` WHERE `receiver` = '" + this->Escape(nc->display) + "'");
	}

	void OnNickForbidden(NickAlias *na)
	{
		this->RunQuery("UPDATE `anope_ns_alias` SET `flags` = '" + BuildFlagsList(na) + "' WHERE `nick` = '" + this->Escape(na->nick) + "'");
	}

	void OnNickGroup(User *u, NickAlias *)
	{
		OnNickRegister(findnick(u->nick));
	}

	void OnMakeNickRequest(NickRequest *nr)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ns_request` (nick, passcode, password, email, requested) VALUES('" + this->Escape(nr->nick) + "', '" + this->Escape(nr->passcode) + "', '" + this->Escape(nr->password) + "', '" + this->Escape(nr->email) + "', " + stringify(nr->requested) + ")");
	}

	void OnDelNickRequest(NickRequest *nr)
	{
		this->RunQuery("DELETE FROM `anope_ns_request` WHERE `nick` = '" + this->Escape(nr->nick) + "'");
	}

	void InsertAlias(NickAlias *na)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ns_alias` (nick, last_quit, last_realname, last_usermask, time_registered, last_seen, flags, display) VALUES('" +
			this->Escape(na->nick) + "', '" + this->Escape(na->last_quit) + "', '" +
			this->Escape(na->last_realname) + "', '" + this->Escape(na->last_usermask) + "', " + stringify(na->time_registered) + ", " + stringify(na->last_seen) +
			", '" + BuildFlagsList(na) + "', '" + this->Escape(na->nc->display) + "') " + "ON DUPLICATE KEY UPDATE last_quit=VALUES(last_quit), "
			"last_realname=VALUES(last_realname), last_usermask=VALUES(last_usermask), time_registered=VALUES(time_registered), last_seen=VALUES(last_seen), "
			"flags=VALUES(flags), display=VALUES(display)");
	}

	void InsertCore(NickCore *nc)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ns_core` (display, pass, email, greet, flags, language, channelcount, memomax) VALUES('" +
			this->Escape(nc->display) + "', '" + this->Escape(nc->pass) + "', '" +
			this->Escape(nc->email) + "', '" + this->Escape(nc->greet) + "', '" +
			BuildFlagsList(nc) + "', " + stringify(nc->language) + ", " + stringify(nc->channelcount) + ", " +
			stringify(nc->memos.memomax) + ") " +
			"ON DUPLICATE KEY UPDATE pass=VALUES(pass), email=VALUES(email), greet=VALUES(greet), flags=VALUES(flags), language=VALUES(language), " +
			"channelcount=VALUES(channelcount), memomax=VALUES(memomax)");
	}

	void OnNickRegister(NickAlias *na)
	{
		this->InsertAlias(na);
		this->InsertCore(na->nc);
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay)
	{
		this->RunQuery("UPDATE `anope_ns_core` SET `display` = '" + this->Escape(newdisplay) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_ns_alias` SET `display` = '" + this->Escape(newdisplay) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_ns_access` SET `display` = '" + this->Escape(newdisplay) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_cs_access` SET `display` = '" + this->Escape(newdisplay) + "' WHERE `display` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_cs_info` SET `founder` = '" + this->Escape(newdisplay) + "' WHERE `founder` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_cs_info` SET `successor` = '" + this->Escape(newdisplay) + "' WHERE `successor` = '" + this->Escape(nc->display) + "'");
		this->RunQuery("UPDATE `anope_ms_info` SET `receiver` = '" + this->Escape(newdisplay) + "' WHERE `receiver` = '" + this->Escape(nc->display) + "'");
	}

	void OnNickSuspend(NickAlias *na)
	{
		this->RunQuery("UPDATE `anope_ns_core` SET `flags` = '" + BuildFlagsList(na->nc) + "' WHERE `display` = '" + this->Escape(na->nc->display) + "'");
	}

	void OnAccessAdd(ChannelInfo *ci, User *u, NickAlias *na, int level)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES (" + stringify(level) + ", '" + this->Escape(na->nc->display) + "', '" + this->Escape(ci->name) + "', " + stringify(Anope::CurTime) + ", '" + this->Escape(u->nick) + "')");
	}

	void OnAccessDel(ChannelInfo *ci, User *u, NickCore *nc)
	{
		this->RunQuery("DELETE FROM `anope_cs_access` WHERE `display` = '" + this->Escape(nc->display) + "' AND `channel` = '" + this->Escape(ci->name) + "'");
	}

	void OnAccessChange(ChannelInfo *ci, User *u, NickAlias *na, int level)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES (" + stringify(level) + ", '" + this->Escape(na->nc->display) + "', '" + this->Escape(ci->name) + "', " + stringify(Anope::CurTime) + ", '" + this->Escape(u->nick) + "') ON DUPLICATE KEY UPDATE level=VALUES(level), display=VALUES(display), channel=VALUES(channel), last_seen=VALUES(last_seen), creator=VALUES(creator)");
	}

	void OnAccessClear(ChannelInfo *ci, User *u)
	{
		this->RunQuery("DELETE FROM `anope_cs_access` WHERE `channel` = '" + this->Escape(ci->name) + "'");
	}

	void OnLevelChange(User *u, ChannelInfo *ci, int pos, int what)
	{
		if (pos >= 0)
			this->RunQuery("UPDATE `anope_cs_levels` SET `level` = " + stringify(what) + " WHERE `channel` = '" + this->Escape(ci->name) + "' AND `position` = " + stringify(pos));
		else
			for (int i = 0; i < CA_SIZE; ++i)
				this->RunQuery("UPDATE `anope_cs_levels` SET `level` = " + stringify(ci->levels[i]) + " WHERE `channel` = '" + this->Escape(ci->name) + "' AND `position` = " + stringify(i));
	}

	void OnChanForbidden(ChannelInfo *ci)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_cs_info` (name, time_registered, last_used, flags, forbidby, forbidreason) VALUES ('" +
			this->Escape(ci->name) + "', " + stringify(ci->time_registered) + ", " + stringify(ci->last_used) + ", '" + BuildFlagsList(ci) + "', '" +  this->Escape(ci->forbidby) + "', '"
			+ this->Escape(ci->forbidreason) + "')");
	}

	void OnDelChan(ChannelInfo *ci)
	{
		this->Escape("DELETE FROM `anope_cs_access` WHERE `channel` = '" + this->Escape(ci->name) + "'");
		this->Escape("DELETE FROM `anope_cs_akick` WHERE `channel` = '" + this->Escape(ci->name) + "'");
		this->Escape("DELETE FROM `anope_cs_info` WHERE `name` = '" + this->Escape(ci->name) + "'");
		this->Escape("DELETE FROM `anope_cs_levels` WHERE `channel` = '" + this->Escape(ci->name) + "'");
		this->Escape("DELETE From `anope_cs_ttb` WHERE `channel` = '" + this->Escape(ci->name) + "'");
		this->Escape("DELETE FROM `anope_bs_badwords` WHERE `channel` = '" + this->Escape(ci->name) + "'");
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		Anope::string flags = BuildFlagsList(ci), mlockon = GetMLockOn(ci), mlockoff = GetMLockOff(ci), mlockparams = GetMLockParams(ci);
		this->RunQuery("INSERT DELAYED INTO `anope_cs_info` (name, founder, successor, descr, time_registered, last_used, last_topic,  last_topic_setter, last_topic_time, flags, forbidby, forbidreason, bantype, mlock_on, mlock_off, mlock_params, entry_message, memomax, botnick, botflags, capsmin, capspercent, floodlines, floodsecs, repeattimes) VALUES('" +
			this->Escape(ci->name) + "', '" + this->Escape(ci->founder ? ci->founder->display : "") + "', '" +
			this->Escape(ci->successor ? ci->successor->display : "") + "', '" + this->Escape(ci->desc) + "', " +
			stringify(ci->time_registered) + ", " + stringify(ci->last_used) + ", '" + this->Escape(ci->last_topic) + "', '" +
			this->Escape(ci->last_topic_setter) + "', " + stringify(ci->last_topic_time) + ", '" + flags + "', '" + 
			this->Escape(ci->forbidby) + "', '" + this->Escape(ci->forbidreason) + "', " +  stringify(ci->bantype) + ", '" +
			mlockon + "', '" + mlockoff + "', '" + mlockparams + "', '" + this->Escape(ci->entry_message) + "', " +
			stringify(ci->memos.memomax) + ", '" + this->Escape(ci->bi ? ci->bi->nick : "") + "', '" + GetBotFlags(ci->botflags) +
			"', " + stringify(ci->capsmin) + ", " + stringify(ci->capspercent) + ", " + stringify(ci->floodlines) + ", " + stringify(ci->floodsecs) + ", " + stringify(ci->repeattimes) + ") " +
			"ON DUPLICATE KEY UPDATE founder=VALUES(founder), successor=VALUES(successor), descr=VALUES(descr), time_registered=VALUES(time_registered), last_used=VALUES(last_used), last_topic=VALUES(last_topic), last_topic_setter=VALUES(last_topic_setter),  last_topic_time=VALUES(last_topic_time), flags=VALUES(flags), forbidby=VALUES(forbidby), forbidreason=VALUES(forbidreason), bantype=VALUES(bantype), mlock_on=VALUES(mlock_on), mlock_off=VALUES(mlock_off), mlock_params=VALUES(mlock_params), entry_message=VALUES(entry_message), memomax=VALUES(memomax), botnick=VALUES(botnick), botflags=VALUES(botflags), capsmin=VALUES(capsmin), capspercent=VALUES(capspercent), floodlines=VALUES(floodlines), floodsecs=VALUES(floodsecs), repeattimes=VALUES(repeattimes)");
	}

	void OnChanSuspend(ChannelInfo *ci)
	{
		this->RunQuery("UPDATE `anope_cs_info` SET `flags` = '" + BuildFlagsList(ci) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
		this->RunQuery("UPDATE `anope_cs_info` SET `forbidby` = '" + this->Escape(ci->forbidby) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
		this->RunQuery("UPDATE `anope_cs_info` SET `forbidreason` = '" + this->Escape(ci->forbidreason) + " WHERE `name` = '" + this->Escape(ci->name) + "'");
	}

	void OnAkickAdd(ChannelInfo *ci, AutoKick *ak)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_cs_akick` (channel, flags, mask, reason, creator, created, last_used) VALUES('" + this->Escape(ci->name) + "', '" +
			(ak->HasFlag(AK_ISNICK) ? "ISNICK " : "") + (ak->HasFlag(AK_STUCK) ? "STUCK " : "") + "', '" +
			this->Escape(ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask) + "', '" + this->Escape(ak->reason) + "', '" +
			this->Escape(ak->creator) + "', " + stringify(ak->addtime) + ", " + stringify(ak->last_used) + ")");
	}

	void OnAkickDel(ChannelInfo *ci, AutoKick *ak)
	{
		this->RunQuery("DELETE FROM `anope_cs_akick` WHERE `channel`= '" + this->Escape(ci->name) + "' AND `mask` = '" + (ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask));
	}

	void OnBotCreate(BotInfo *bi)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES('" +
			this->Escape(bi->nick) + "', '" + this->Escape(bi->GetIdent()) + "', '" + this->Escape(bi->host) + "', '" +
			this->Escape(bi->realname) + "', '" + GetBotServFlags(bi) + "', " + stringify(bi->created) + ", " + stringify(bi->chancount) + ") " +
			"ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(chancount)");
	}

	void OnBotChange(BotInfo *bi)
	{
		OnBotCreate(bi);
	}

	void OnBotDelete(BotInfo *bi)
	{
		this->RunQuery("DELETE FROM `anope_bs_core` WHERE `nick` = '" + this->Escape(bi->nick) + "'");
		this->RunQuery("UPDATE `anope_cs_info` SET `botnick` = '' WHERE `botnick` = '" + this->Escape(bi->nick) + "'");
	}

	EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi)
	{
		this->RunQuery("UPDATE `anope_cs_info` SET `botnick` = '" + this->Escape(bi->nick) + "' WHERE `name` = '" + this->Escape(ci->name) + "'");
		return EVENT_CONTINUE;
	}

	EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci)
	{
		this->RunQuery("UPDATE `anope_cs_info` SET `botnick` = '' WHERE `name` = '" + this->Escape(ci->name) + "'");
		return EVENT_CONTINUE;
	}

	void OnBadWordAdd(ChannelInfo *ci, BadWord *bw)
	{
		Anope::string query = "INSERT DELAYED INTO `anope_bs_badwords` (channel, word, type) VALUES('" + this->Escape(ci->name) + "', '" + this->Escape(bw->word) + "', '";
		switch (bw->type)
		{
			case BW_SINGLE:
				query += "SINGLE";
				break;
			case BW_START:
				query += "START";
				break;
			case BW_END:
				query += "END";
				break;
			default:
				query += "ANY";
		}
		query += "') ON DUPLICATE KEY UPDATE channel=VALUES(channel), word=VALUES(word), type=VALUES(type)";
		this->RunQuery(query);
	}

	void OnBadWordDel(ChannelInfo *ci, BadWord *bw)
	{
		Anope::string query = "DELETE FROM `anope_bs_badwords` WHERE `channel` = '" + this->Escape(ci->name) + "' AND `word` = '" + this->Escape(bw->word) + " AND `type` = '";
		switch (bw->type)
		{
			case BW_SINGLE:
				query += "SINGLE";
				break;
			case BW_START:
				query += "START";
				break;
			case BW_END:
				query += "END";
				break;
			default:
				query += "ANY";
		}
		query += "'";
		this->RunQuery(query);
	}

	void OnMemoSend(User *, NickCore *nc, Memo *m)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ms_info` (receiver, number, flags, time, sender, text, serv) VALUES('" +
			this->Escape(nc->display) + "', " + stringify(m->number) + ", '" + BuildFlagsList(m) + "', " + stringify(m->time) + ", '" +
			this->Escape(m->sender) + "', '" + this->Escape(m->text) + "', 'NICK')");
	}

	void OnMemoSend(User *, ChannelInfo *ci, Memo *m)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_ms_info` (receiver, number, flags, time, sender, text, serv) VALUES('" +
			this->Escape(ci->name) + "', " + stringify(m->number) + ", '" + BuildFlagsList(m) + "', " + stringify(m->time) + ", '" +
			this->Escape(m->sender) + "', '" + this->Escape(m->text) + "', 'CHAN')");
	}

	void OnMemoDel(NickCore *nc, MemoInfo *mi, int number)
	{
		if (number)
			this->RunQuery("DELETE FROM `anope_ms_info` WHERE `receiver` = '" + this->Escape(nc->display) + "' AND `number` = " + stringify(number));
		else
			this->RunQuery("DELETE FROM `anope_ms_info` WHERE `receiver` = '" + this->Escape(nc->display) + "'");
	}

	void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, int number)
	{
		if (number)
			this->RunQuery("DELETE FROM `anope_ms_info` WHERE `receiver` = '" + this->Escape(ci->name) + "' AND `number` = " + stringify(number));
		else
			this->RunQuery("DELETE FROM `anope_ms_info` WHERE `receiver` = '" + this->Escape(ci->name) + "'");
	}

	EventReturn OnAddAkill(User *, XLine *ak)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_os_akills` (user, host, xby, reason, seton, expire) VALUES('" +
			this->Escape(ak->GetUser()) + "', '" + this->Escape(ak->GetHost()) + "', '" + this->Escape(ak->By) + "', '" +
			this->Escape(ak->Reason) + "', " + stringify(ak->Created) + ", " + stringify(ak->Expires) + ")");
		return EVENT_CONTINUE;
	}

	void OnDelAkill(User *, XLine *ak)
	{
		if (ak)
			this->RunQuery("DELETE FROM `anope_os_akills` WHERE `host` = '" + this->Escape(ak->GetHost()) + "'");
		else
			this->RunQuery("TRUNCATE TABLE `anope_os_akills`");
	}

	EventReturn OnExceptionAdd(User *, Exception *ex)
	{
		this->RunQuery("INSERT DELAYED INTO `anope_os_exceptions` (mask, slimit, who, reason, time, expires) VALUES('" +
			this->Escape(ex->mask) + "', " + stringify(ex->limit) + ", '" + this->Escape(ex->who) + "', '" + this->Escape(ex->reason) + "', " +
			stringify(ex->time) + ", " + stringify(ex->expires) + ")");
		return EVENT_CONTINUE;
	}

	void OnExceptionDel(User *, Exception *ex)
	{
		this->RunQuery("DELETE FROM `anope_os_exceptions` WHERE `mask` = '" + this->Escape(ex->mask) + "'");
	}

	EventReturn OnAddXLine(User *, XLine *x, XLineType Type)
	{
		this->RunQuery(Anope::string("INSERT DELAYED INTO `anope_os_sxlines` (type, mask, xby, reason, seton, expire) VALUES('") +
			(Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) + "', '" +
			this->Escape(x->Mask) + "', '" + this->Escape(x->By) + "', '" + this->Escape(x->Reason) + "', " +
			stringify(x->Created) + ", " + stringify(x->Expires) + ")");
		return EVENT_CONTINUE;
	}

	void OnDelXLine(User *, XLine *x, XLineType Type)
	{
		if (x)
			this->RunQuery("DELETE FROM `anope_os_xlines` WHERE `mask` = '" + this->Escape(x->Mask) + "' AND `type` = '" +
				(Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) + "'");
		else
			this->RunQuery(Anope::string("DELETE FROM `anope_os_xlines` WHERE `type` = '") + (Type == X_SNLINE ? "SNLINE" : (Type == X_SQLINE ? "SQLINE" : "SZLINE")) + "'");
	}
};

void MySQLInterface::OnResult(const SQLResult &r)
{
	Log(LOG_DEBUG) << "MySQL successfully executed query: " << r.GetQuery();
}

void MySQLInterface::OnError(const SQLResult &r)
{
	if (!r.GetQuery().empty())
		Log(LOG_DEBUG) << "Error executing query " << r.GetQuery() << ": " << r.GetError();
	else
		Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
}


static void Write(const Anope::string &data)
{
	me->RunQuery("INSERT DELAYED INTO `anope_extra` (data) VALUES('" + me->Escape(data) + "')");
}

static void WriteNickMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurNick)
		throw CoreException(Anope::string("WriteNickMetadata without a nick to write"));

	me->RunQuery("INSERT DELAYED INTO `anope_ns_alias_metadata` (nick, name, value) VALUES('" + me->Escape(CurNick->nick) + "', '" + me->Escape(key) + "', '" + me->Escape(data) + "')");
}

static void WriteCoreMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurCore)
		throw CoreException(Anope::string("WritCoreMetadata without a core to write"));

	me->RunQuery("INSERT DELAYED INTO `anope_ns_core_metadata` (nick, name, value) VALUES('" + me->Escape(CurCore->display) + "', '" + me->Escape(key) + "', '" + me->Escape(data) + "')");
}

static void WriteChannelMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurChannel)
		throw CoreException(Anope::string("WriteChannelMetadata without a channel to write"));

	me->RunQuery("INSERT DELAYED INTO `anope_cs_info_metadata` (channel, name, value) VALUES('" + me->Escape(CurChannel->name) + "', '" + me->Escape(key) + "', '" + me->Escape(data) + "')");
}

static void WriteBotMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurBot)
		throw CoreException(Anope::string("WriteBotMetadata without a bot to write"));

	me->RunQuery("INSERT DELAYED INTO `anope_bs_info_metadata` (botname, name, value) VALUES('" + me->Escape(CurBot->nick) + "', '" + me->Escape(key) + "', '" + me->Escape(data) + "')");
}

static void SaveDatabases()
{
	me->RunQuery("TRUNCATE TABLE `anope_ns_alias`");

	for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		me->InsertAlias(it->second);

	me->RunQuery("TRUNCATE TABLE `anope_ns_core`");
	me->RunQuery("TRUNCATE TABLE `anope_ms_info`");

	for (nickcore_map::const_iterator nit = NickCoreList.begin(), nit_end = NickCoreList.end(); nit != nit_end; ++nit)
	{
		NickCore *nc = nit->second;

		for (std::vector<Anope::string>::iterator it = nc->access.begin(), it_end = nc->access.end(); it != it_end; ++it)
		{
			me->RunQuery("INSERT DELAYED INTO `anope_ns_access` (display, access) VALUES(" + me->Escape(nc->display) + ", " + me->Escape(*it) + ")");
		}

		for (unsigned j = 0, end = nc->memos.memos.size(); j < end; ++j)
		{
			Memo *m = nc->memos.memos[j];

			me->OnMemoSend(NULL, nc, m);
		}

		me->InsertCore(nc);
	}


	me->RunQuery("TRUNCATE TABLE `anope_bs_core`");

	for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		me->OnBotCreate(it->second);

	me->RunQuery("TRUNCATE TABLE `anope_cs_info`");
	me->RunQuery("TRUNCATE TABLE `anope_bs_badwords`");
	me->RunQuery("TRUNCATE TABLE `anope_cs_access`");
	me->RunQuery("TRUNCATE TABLE `anope_cs_akick`");
	me->RunQuery("TRUNCATE TABLE `anope_cs_levels`");

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

			me->RunQuery(Anope::string("INSERT DELAYED INTO `anope_cs_access` (level, display, channel, last_seen, creator) VALUES('") + access->level + "', " + me->Escape(access->nc->display) + ", " + me->Escape(ci->name) + ", " + access->last_seen + ", " + me->Escape(access->creator) + ") ON DUPLICATE KEY UPDATE level=VALUES(level), last_seen=VALUES(last_seen), creator=VALUES(creator)");
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);

			me->OnAkickAdd(ci, ak);
		}

		for (int k = 0; k < CA_SIZE; ++k)
		{
			me->RunQuery("INSERT DELAYED INTO `anope_cs_levels` (channel, position, level) VALUES(" + me->Escape(ci->name) + ", '" + stringify(k) + "', '" + stringify(ci->levels[k]) + "') ON DUPLICATE KEY UPDATE position=VALUES(position), level=VALUES(level)");
		}

		for (unsigned j = 0, end = ci->memos.memos.size(); j < end; ++j)
		{
			Memo *m = ci->memos.memos[j];

			me->OnMemoSend(NULL, ci, m);
		}
	}

	me->RunQuery("TRUNCATE TABLE `anope_ns_request`");

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

	for (unsigned i = 0, end = exceptions.size(); i < end; ++i)
		me->OnExceptionAdd(NULL, exceptions[i]);
}

CommandReturn CommandSQLSync::Execute(User *u, const std::vector<Anope::string> &params)
{
	SaveDatabases();
	u->SendMessage(OperServ, OPER_SYNC_UPDATED);
	return MOD_CONT;
}

MODULE_INIT(DBMySQL)

