#include "module.h"
#include "../extra/sql.h"
#include "../commands/os_session.h"

static Anope::string ToString(const std::vector<Anope::string> &strings)
{
	Anope::string ret;

	for (unsigned i = 0; i < strings.size(); ++i)
		ret += " " + strings[i];
	
	if (!ret.empty())
		ret.erase(ret.begin());
	
	return ret;
}

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
	CommandSQLSync(Module *creator) : Command(creator, "operserv/sqlsync", 0, 0)
	{
		this->SetDesc(_("Import your databases to SQL"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params);

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command syncs your databases with SQL. You should\n"
				"only have to execute this command once, when you initially\n"
				"import your databases into SQL."));
		return true;
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
	MySQLInterface sqlinterface;
	service_reference<SQLProvider> SQL;

 public:
	service_reference<SessionService> SessionInterface;
	time_t lastwarn;
	bool ro;

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
					ircdproto->SendGlobops(bi, "Unable to locate SQL reference, is m_mysql loaded? Going to readonly...");
				readonly = this->ro = true;
				this->lastwarn = Anope::CurTime;
			}
		}
	}

	DBMySQL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), commandsqlsync(this), sqlinterface(this), SQL("mysql/main"), SessionInterface("session")
	{
		me = this;

		this->lastwarn = 0;
		this->ro = false;

		Implementation i[] = {
			I_OnLoadDatabase, I_OnServerConnect
		};
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		ModuleManager::RegisterService(&commandsqlsync);

		if (CurrentUplink)
			OnServerConnect();
	}

	void OnServerConnect()
	{
		Implementation i[] = {
			/* Misc */
			I_OnSaveDatabase, I_OnPostCommand,
			/* NickServ */
			I_OnNickAddAccess, I_OnNickEraseAccess, I_OnNickClearAccess,
			I_OnDelCore, I_OnNickForbidden, I_OnNickGroup,
			I_OnNickRegister, I_OnChangeCoreDisplay,
			I_OnNickSuspended, I_OnDelNick,
			/* ChanServ */
			I_OnAccessAdd, I_OnAccessDel, I_OnAccessClear, I_OnLevelChange,
			I_OnChanForbidden, I_OnDelChan, I_OnChanRegistered, I_OnChanSuspend,
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

	EventReturn OnLoadDatabase()
	{
		if (!SQL)
		{
			Log() << "Error, unable to find service reference for SQL, is m_mysql loaded and configured properly?";
			return EVENT_CONTINUE;
		}

		SQLQuery query;

		query = "SELECT * FROM `anope_ns_core`";
		SQLResult r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = new NickCore(r.Get(i, "display"));
			nc->pass = r.Get(i, "pass");
			nc->email = r.Get(i, "email");
			nc->greet = r.Get(i, "greet");

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			std::vector<Anope::string> flags;
			while (sep.GetToken(buf))
				flags.push_back(buf);
			nc->FromString(flags);

			nc->language = r.Get(i, "language");
			nc->memos.memomax = r.Get(i, "memomax").is_number_only() ? convertTo<int16>(r.Get(i, "memomax")) : 20;
		}

		query = "SELECT * FROM `anope_ns_access`";
		r = SQL->RunQuery(query);
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

		query = "SELECT * FROM `anope_ns_core_metadata`";
		r = SQL->RunQuery(query);
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

		query = "SELECT * FROM `anope_ns_alias`";
		r = SQL->RunQuery(query);
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
			na->last_realhost = r.Get(i, "last_realhost");
			na->time_registered = r.Get(i, "time_registered").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time_registered")) : Anope::CurTime;
			na->last_seen = r.Get(i, "last_seen").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_seen")) : Anope::CurTime;

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			std::vector<Anope::string> flags;
			while (sep.GetToken(buf))
				flags.push_back(buf);

			na->FromString(flags);
		}

		query = "SELECT * FROM `anope_ns_alias_metadata`";
		r = SQL->RunQuery(query);
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
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, r.Get(i, "name"), Params));
		}

		query = "SELECT * FROM `anope_hs_core`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickAlias *na = findnick(r.Get(i, "nick"));
			if (!na)
			{
				Log() << "MySQL: Got vhost entry for nonexistant nick " << r.Get(i, "nick");
				continue;
			}

			time_t creation = r.Get(i, "time").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time")) : Anope::CurTime;
			na->hostinfo.SetVhost(r.Get(i, "vident"), r.Get(i, "vhost"), r.Get(i, "creator"), creation);
		}

		query = "SELECT * FROM `anope_bs_core`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			BotInfo *bi = findbot(r.Get(i, "nick"));
			if (!bi)
				bi = new BotInfo(r.Get(i, "nick"), r.Get(i, "user"), r.Get(i, "host"));
			bi->realname = r.Get(i, "rname");
			bi->created = r.Get(i, "created").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "created")) : Anope::CurTime;

			spacesepstream sep(r.Get(i, "flags"));
			Anope::string buf;
			std::vector<Anope::string> flags;
			while (sep.GetToken(buf))
				flags.push_back(buf);

			bi->FromString(flags);
		}

		query = "SELECT * FROM `anope_bs_info_metadata`";
		r = SQL->RunQuery(query);
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
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(bi, r.Get(i, "name"), Params));
		}

		query = "SELECT * FROM `anope_cs_info`";
		r = SQL->RunQuery(query);
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
				ci->SetFounder(nc);
				if (!r.Get(i, "successor").empty())
					ci->successor = findcore(r.Get(i, "successor"));
				ci->desc = r.Get(i, "descr");
				ci->time_registered = r.Get(i, "time_registered").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time_registered")) : Anope::CurTime;
				ci->last_used = r.Get(i, "last_used").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_used")) : Anope::CurTime;
				ci->last_topic = r.Get(i, "last_topic");
				ci->last_topic_setter = r.Get(i, "last_topic_setter");
				ci->last_topic_time = r.Get(i, "last_topic_time").is_number_only() ? convertTo<int>(r.Get(i, "last_topic_time")) : Anope::CurTime;
				ci->bantype = r.Get(i, "bantype").is_number_only() ? convertTo<int>(r.Get(i, "bantype")) : 2;
				ci->memos.memomax = r.Get(i, "memomax").is_number_only() ? convertTo<int16>(r.Get(i, "memomax")) : 20;
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
					std::vector<Anope::string> flags;
					while (sep.GetToken(buf))
						flags.push_back(buf);

					ci->botflags.FromString(flags);
				}

				if (!r.Get(i, "flags").empty())
				{
					spacesepstream sep(r.Get(i, "flags"));
					Anope::string buf;
					std::vector<Anope::string> flags;
					while (sep.GetToken(buf))
						flags.push_back(buf);

					ci->FromString(flags);
				}
			}
		}

		query = "SELECT * FROM `anope_cs_ttb`";
		r = SQL->RunQuery(query);
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

		query = "SELECT * FROM `anope_bs_badwords`";
		r = SQL->RunQuery(query);
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

		query = "SELECT * FROM `anope_cs_access`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel access entry for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			const Anope::string &provider = r.Get(i, "provider"), &data = r.Get(i, "data");
			service_reference<AccessProvider> ap(provider);
			if (!ap)
			{
				Log() << "MySQL: Access entry for " << ci->name << " using nonexistant provider " << provider;
				continue;
			}

			ChanAccess *access = ap->Create();
			access->ci = ci;
			access->mask = r.Get(i, "mask");
			access->creator = r.Get(i, "creator");
			access->last_seen = r.Get(i, "last_seen").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "last_seen")) : Anope::CurTime;
			access->created = r.Get(i, "created").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "created")) : Anope::CurTime;
			access->Unserialize(data);
			ci->AddAccess(access);
		}

		query = "SELECT * FROM `anope_cs_akick`";
		r = SQL->RunQuery(query);
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
			while (sep.GetToken(flag))
			{
				if (flag.equals_cs("ISNICK"))
					nc = findcore(r.Get(i, "mask"));

				AutoKick *ak;
				if (nc)
					ak = ci->AddAkick(r.Get(i, "creator"), nc, r.Get(i, "reason"), atol(r.Get(i, "created").c_str()), atol(r.Get(i, "last_used").c_str()));
				else
					ak = ci->AddAkick(r.Get(i, "creator"), r.Get(i, "mask"), r.Get(i, "reason"), atol(r.Get(i, "created").c_str()), atol(r.Get(i, "last_used").c_str()));
				if (nc)
					ak->SetFlag(AK_ISNICK);
			}
		}

		query = "SELECT * FROM `anope_cs_levels`";
		r = SQL->RunQuery(query);
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

		query = "SELECT * FROM `anope_cs_info_metadata`";
		r = SQL->RunQuery(query);
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
			FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, r.Get(i, "name"), Params));
		}

		query = "SELECT * FROM `anope_cs_mlock`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
			{
				Log() << "MySQL: Channel mlock for nonexistant channel " << r.Get(i, "channel");
				continue;
			}

			std::vector<Anope::string> mlocks;
			ci->GetExtRegular("db_mlock", mlocks);

			Anope::string modestring = r.Get(i, "status") + " " + r.Get(i, "mode") + " " + r.Get(i, "setter") + " " + r.Get(i, "created") + " " + r.Get(i, "param");

			mlocks.push_back(modestring);

			ci->Extend("db_mlock", new ExtensibleItemRegular<std::vector<Anope::string> >(mlocks));
		}

		query = "SELECT * FROM `anope_ms_info`";
		r = SQL->RunQuery(query);
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
				m->time = r.Get(i, "time").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "time")) : Anope::CurTime;
				m->text = r.Get(i, "text");

				if (!r.Get(i, "flags").empty())
				{
					spacesepstream sep(r.Get(i, "flags"));
					Anope::string buf;
					std::vector<Anope::string> flags;
					while (sep.GetToken(buf))
						flags.push_back(buf);

					m->FromString(flags);
				}
			}
		}

		for (std::list<XLineManager *>::iterator it = XLineManager::XLineManagers.begin(), it_end = XLineManager::XLineManagers.end(); it != it_end; ++it)
		{
			XLineManager *xm = *it;

			query = "SELECT * FROM `anope_os_xlines` WHERE `type` = @type";
			query.setValue("type", Anope::string(xm->Type()));
			r = SQL->RunQuery(query);
			for (int i = 0; i < r.Rows(); ++i)
			{
				Anope::string user = r.Get(i, "user");
				Anope::string host = r.Get(i, "host");
				Anope::string by = r.Get(i, "xby");
				Anope::string reason = r.Get(i, "reason");
				time_t seton = r.Get(i, "seton").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "seton")) : Anope::CurTime;
				time_t expires = r.Get(i, "expires").is_pos_number_only() ? convertTo<time_t>(r.Get(i, "expires")) : Anope::CurTime;

				XLine *x = xm->Add(user + "@" + host, by, expires, reason);
				if (x)
					x->Created = seton;
			}
		}

		query = "SELECT * FROM `anope_os_exceptions`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			Anope::string mask = r.Get(i, "mask");
			unsigned limit = convertTo<unsigned>(r.Get(i, "slimit"));
			Anope::string creator = r.Get(i, "who");
			Anope::string reason = r.Get(i, "reason");
			time_t expires = convertTo<time_t>(r.Get(i, "expires"));

			if (SessionInterface)
			{
				Exception *e = new Exception();
				e->mask = mask;
				e->limit = limit;
				e->who = creator;
				e->reason = reason;
				e->time = expires;
				SessionInterface->AddException(e);
			}
		}

		query = "SELECT * FROM `anope_extra`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			std::vector<Anope::string> params = MakeVector(r.Get(i, "data"));
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		}

		query = "SELECT * FROM `anope_ns_core_metadata`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			NickCore *nc = findcore(r.Get(i, "nick"));
			if (!nc)
				continue;
			if (r.Get(i, "name") == "MEMO_IGNORE")
				nc->memos.ignores.push_back(r.Get(i, "value").ci_str());
		}

		query = "SELECT * FROM `anope_cs_info_metadata`";
		r = SQL->RunQuery(query);
		for (int i = 0; i < r.Rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(r.Get(i, "channel"));
			if (!ci)
				continue;
			if (r.Get(i, "name") == "MEMO_IGNORE")
				ci->memos.ignores.push_back(r.Get(i, "value").ci_str());
		}

		return EVENT_STOP;
	}

	EventReturn OnSaveDatabase()
	{
		SQLQuery query;
	
		query = "TRUNCATE TABLE `anope_os_core`";
		this->RunQuery(query);

		query = "INSERT INTO `anope_os_core` (maxusercnt, maxusertime) VALUES(@maxusercnt, @maxusertime)";
		query.setValue("maxusercnt", maxusercnt);
		query.setValue("maxusertime", maxusertime);
		this->RunQuery(query);

		query = "TRUNCATE TABLE `anope_ns_core_metadata`";
		this->RunQuery(query);
		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			CurCore = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteCoreMetadata, CurCore));
		}

		query = "TRUNCATE TABLE `anope_ns_alias_metadata`";
		this->RunQuery(query);
		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			CurNick = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteNickMetadata, CurNick));
		}

		query = "TRUNCATE TABLE `anope_cs_info_metadata`";
		this->RunQuery(query);
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			CurChannel = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteChannelMetadata, CurChannel));
		}

		query = "TRUNCATE TABLE `anope_bs_info_metadata`";
		this->RunQuery(query);
		for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			if (it->second->HasFlag(BI_CONF))
				continue;

			CurBot = it->second;
			FOREACH_MOD(I_OnDatabaseWriteMetadata, OnDatabaseWriteMetadata(WriteBotMetadata, CurBot));

			query = "INSERT INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES(@nick, @user, @host, @rname, @flags, @created, @chancount) ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(chancount)";
			query.setValue("nick", CurBot->nick);
			query.setValue("user", CurBot->GetIdent());
			query.setValue("host", CurBot->host);
			query.setValue("rname", CurBot->realname);
			query.setValue("flags", ToString(CurBot->ToString()));
			query.setValue("created", CurBot->created);
			query.setValue("chancount", CurBot->chancount);
			this->RunQuery(query);
		}

		query = "TRUNCATE TABLE `anope_extra`";
		this->RunQuery(query);
		FOREACH_MOD(I_OnDatabaseWrite, OnDatabaseWrite(Write));

		return EVENT_CONTINUE;
	}

	void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (command->name.find("nickserv/set/") == 0 || command->name.find("nickserv/saset/") == 0)
		{
			NickAlias *na = findnick(command->name.find("nickserv/set/") == 0 ? source.u->nick : params[1]);
			if (!na)
				return;

			if (command->name == "nickserv/set/password" || command->name == "nickserv/saset/password")
			{
				SQLQuery query("UPDATE `anope_ns_core` SET `pass` = @pass WHERE `display` = @display");
				query.setValue("pass", na->nc->pass);
				query.setValue("display", na->nc->display);
				this->RunQuery(query);
			}
			else if (command->name == "nickserv/set/language" || command->name == "nickserv/saset/language")
			{
				SQLQuery query("UPDATE `anope_ns_core` SET `language` = @language WHERE `display` = @display");
				query.setValue("language", na->nc->language);
				query.setValue("display", na->nc->display);
				this->RunQuery(query);
			}
			else if (command->name == "nickserv/set/email" || command->name == "nickserv/saset/email")
			{
				SQLQuery query("UPDATE `anope_ns_core` SET `email` = @email WHERE `display` = @display");
				query.setValue("email", na->nc->email);
				query.setValue("display", na->nc->display);
				this->RunQuery(query);
			}
			else if (command->name == "nickserv/set/greet" || command->name == "nickserv/saset/greet")
			{
				SQLQuery query("UPDATE `anope_ns_core` SET `greet` = @greet WHERE `display` = @display");
				query.setValue("greet", na->nc->greet);
				query.setValue("display", na->nc->display);
				this->RunQuery(query);
			}
			else
			{
				SQLQuery query("UPDATE `anope_ns_core` SET `flags` = @flags WHERE `display` = @display");
				query.setValue("flags", ToString(na->nc->ToString()));
				query.setValue("display", na->nc->display);
				this->RunQuery(query);
			}
		}
		else if (command->name.find("chanserv/set") == 0 || command->name.find("chanserv/saset") == 0)
		{
			ChannelInfo *ci = params.size() > 0 ? cs_findchan(params[0]) : NULL;
			if (!ci)
				return;

			if (command->name == "chanserv/set/founder" || command->name == "chanserv/saset/founder")
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `founder` = @founder WHERE `name` = @name");
				query.setValue("founder", ci->GetFounder() ? ci->GetFounder()->display : "");
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
			else if (command->name == "chanserv/set/successor" || command->name == "chanserv/saset/successor")
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `successor` = @successor WHERE `name` = @name");
				query.setValue("successor", ci->successor ? ci->successor->display : "");
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
			else if (command->name == "chanserv/set/desc" || command->name == "chanserv/saset/desc")
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `descr` = @descr WHERE `name` = @name");
				query.setValue("descr", ci->desc);
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
			else if (command->name == "chanserv/set/bantype" || command->name == "chanserv/saset/bantype")
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `bantype` = @bantype WHERE `name` = @name");
				query.setValue("bantype", ci->bantype);
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
			else
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `flags` = @flags WHERE `name` = @name");
				query.setValue("flags", ToString(ci->ToString()));
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
		}
		else if (command->name == "botserv/kick" && params.size() > 2)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			if (!ci)
				return;
			if (!ci->AccessFor(u).HasPriv(CA_SET) && !u->HasPriv("botserv/administration"))
				return;
			if (params[1].equals_ci("BADWORDS") || params[1].equals_ci("BOLDS") || params[1].equals_ci("CAPS") || params[1].equals_ci("COLORS") || params[1].equals_ci("FLOOD") || params[1].equals_ci("REPEAT") || params[1].equals_ci("REVERSES") || params[1].equals_ci("UNDERLINES"))
			{
				if (params[2].equals_ci("ON") || params[2].equals_ci("OFF"))
				{
					for (int i = 0; i < TTB_SIZE; ++i)
					{
						SQLQuery query("INSERT INTO `anope_cs_ttb` (channel, ttb_id, value) VALUES(@channel, @ttb_id, @value) ON DUPLICATE KEY UPDATE channel=VALUES(channel), ttb_id=VALUES(ttb_id), value=VALUES(value)");
						query.setValue("channel", ci->name);
						query.setValue("ttb_id", i);
						query.setValue("value", ci->ttb[i]);
						this->RunQuery(query);
					}

					{
						SQLQuery query("UPDATE `anope_cs_info` SET `botflags` = @botflags WHERE `name` = @name");
						query.setValue("botflags", ToString(ci->botflags.ToString()));
						query.setValue("name", ci->name);
						this->RunQuery(query);
					}

					if (params[1].equals_ci("CAPS"))
					{
						SQLQuery query("UPDATE `anope_cs_info` SET `capsmin` = @capsmin, `capspercent` = @capspercent WHERE `name` = @name");
						query.setValue("capsmin", ci->capsmin);
						query.setValue("capspercent", ci->capspercent);
						query.setValue("name", ci->name);
						this->RunQuery(query);
					}
					else if (params[1].equals_ci("FLOOD"))
					{
						SQLQuery query("UPDATE `anope_cs_info` SET `floodlines` = @floodlines, `floodsecs` = @floodsecs WHERE `name` = @name");
						query.setValue("floodlines", ci->floodlines);
						query.setValue("floodsecs", ci->floodsecs);
						query.setValue("name", ci->name);
						this->RunQuery(query);
					}
					else if (params[1].equals_ci("REPEAT"))
					{
						SQLQuery query("UPDATE `anope_cs_info` SET `repeattimes` = @ WHERE `name` = @");
						query.setValue("repeattimes", ci->repeattimes);
						query.setValue("name", ci->name);
						this->RunQuery(query);
					}
				}
			}
		}
		else if (command->name == "botserv/set" && params.size() > 1)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			if (ci && !ci->AccessFor(u).HasPriv(CA_SET) && !u->HasPriv("botserv/administration"))
				return;
			BotInfo *bi = NULL;
			if (!ci)
				bi = findbot(params[0]);
			if (bi && params[1].equals_ci("PRIVATE") && u->HasPriv("botserv/set/private"))
			{
				SQLQuery query("UPDATE `anope_bs_core` SET `flags` = @ WHERE `nick` = @");
				query.setValue("flags", ToString(bi->ToString()));
				query.setValue("nick", bi->nick);
				this->RunQuery(query);
			}
			else if (!ci)
				return;
			else if (params[1].equals_ci("DONTKICKOPS") || params[1].equals_ci("DONTKICKVOICES") || params[1].equals_ci("FANTASY") || params[1].equals_ci("GREET") || params[1].equals_ci("SYMBIOSIS") || params[1].equals_ci("NOBOT"))
			{
				SQLQuery query("UPDATE `anope_cs_info` SET `botflags` = @ WHERE `name` = @");
				query.setValue("botflags", ToString(ci->botflags.ToString()));
				query.setValue("name", ci->name);
				this->RunQuery(query);
			}
		}
		else if (command->name == "memoserv/ignore" && params.size() > 0)
		{
			Anope::string target = params[0];
			NickCore *nc = NULL;
			ChannelInfo *ci = NULL;
			if (target[0] != '#')
			{
				target = u->nick;
				nc = u->Account();
				if (!nc)
					return;
			}
			else
			{
				ci = cs_findchan(target);
				if (!ci || !ci->AccessFor(u).HasPriv(CA_MEMO))
					return;
			}

			MemoInfo *mi = ci ? &ci->memos : &nc->memos;
			Anope::string table = ci ? "anope_cs_info_metadata" : "anope_ns_core_metadata";
			Anope::string ename = ci ? "channel" : "nick";

			SQLQuery query("DELETE FROM `" + table + "` WHERE `" + ename + "` = @target AND `name` = 'MEMO_IGNORE'");
			query.setValue("target", target);
			this->RunQuery(query);

			query = "INSERT INTO `" + table + "` VALUES(" + ename + ", name, value) (@target, 'MEMO_IGNORE, @ignore)";
			query.setValue("target", target);
			for (unsigned j = 0; j < mi->ignores.size(); ++j)
			{
				query.setValue("ignore", mi->ignores[j]);
				this->RunQuery(query);
			}
		}
	}

	void OnNickAddAccess(NickCore *nc, const Anope::string &entry)
	{
		SQLQuery query("INSERT INTO `anope_ns_access` (display, access) VALUES(@display, @access)");
		query.setValue("display", nc->display);
		query.setValue("access", entry);
		this->RunQuery(query);
	}

	void OnNickEraseAccess(NickCore *nc, const Anope::string &entry)
	{
		SQLQuery query("DELETE FROM `anope_ns_access` WHERE `display` = @display AND `access` = @access");
		query.setValue("display", nc->display);
		query.setValue("access", entry);
		this->RunQuery(query);
	}

	void OnNickClearAccess(NickCore *nc)
	{
		SQLQuery query("DELETE FROM `anope_ns_access` WHERE `display` = @display");
		query.setValue("display", nc->display);
		this->RunQuery(query);
	}

	void OnDelCore(NickCore *nc)
	{
		SQLQuery query("DELETE FROM `anope_ns_core` WHERE `display` = @display");
		query.setValue("display", nc->display);
		this->RunQuery(query);
	}

	void OnNickForbidden(NickAlias *na)
	{
		SQLQuery query("UPDATE `anope_ns_alias` SET `flags` = @flags WHERE `nick` = @nick");
		query.setValue("flags", ToString(na->ToString()));
		query.setValue("nick", na->nick);
		this->RunQuery(query);
	}

	void OnNickGroup(User *u, NickAlias *)
	{
		OnNickRegister(findnick(u->nick));
	}

	void InsertAlias(NickAlias *na)
	{
		SQLQuery query("INSERT INTO `anope_ns_alias` (nick, last_quit, last_realname, last_usermask, last_realhost, time_registered, last_seen, flags, display) VALUES(@nick, @last_quit, @last_realname, @last_usermask, @last_realhost, @time_registered, @last_seen, @flags, @display) ON DUPLICATE KEY UPDATE last_quit=VALUES(last_quit), last_realname=VALUES(last_realname), last_usermask=VALUES(last_usermask), last_realhost=VALUES(last_realhost), time_registered=VALUES(time_registered), last_seen=VALUES(last_seen), flags=VALUES(flags), display=VALUES(display)");
		query.setValue("nick", na->nick);
		query.setValue("last_quit", na->last_quit);
		query.setValue("last_realname", na->last_realname);
		query.setValue("last_usermask", na->last_usermask);
		query.setValue("last_realhost", na->last_realhost);
		query.setValue("time_registered", na->time_registered);
		query.setValue("last_seen", na->last_seen);
		query.setValue("flags", ToString(na->ToString()));
		query.setValue("display", na->nc->display);
		this->RunQuery(query);
	}

	void InsertCore(NickCore *nc)
	{
		SQLQuery query("INSERT INTO `anope_ns_core` (display, pass, email, greet, flags, language, memomax) VALUES(@display, @pass, @email, @greet, @flags, @language, @memomax) ON DUPLICATE KEY UPDATE pass=VALUES(pass), email=VALUES(email), greet=VALUES(greet), flags=VALUES(flags), language=VALUES(language), memomax=VALUES(memomax)");
		query.setValue("display", nc->display);
		query.setValue("pass", nc->pass);
		query.setValue("email", nc->email);
		query.setValue("greet", nc->greet);
		query.setValue("flags", ToString(nc->ToString()));
		query.setValue("language", nc->language);
		query.setValue("memomax", nc->memos.memomax);
		this->RunQuery(query);
	}

	void OnNickRegister(NickAlias *na)
	{
		this->InsertCore(na->nc);
		this->InsertAlias(na);
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay)
	{
		SQLQuery query("UPDATE `anope_ns_core` SET `display` = @newdisplay WHERE `display` = @olddisplay");
		query.setValue("newdisplay", newdisplay);
		query.setValue("olddisplay", nc->display);
		this->RunQuery(query);
	}

	void OnNickSuspend(NickAlias *na)
	{
		SQLQuery query("UPDATE `anope_ns_core` SET `flags` = @flags WHERE `display` = @display");
		query.setValue("flags", ToString(na->nc->ToString()));
		query.setValue("display", na->nc->display);
		this->RunQuery(query);
	}

	void OnDelNick(NickAlias *na)
	{
		SQLQuery query("DELETE FROM `anope_ns_alias` WHERE `nick` = @nick");
		query.setValue("nick", na->nick);
		this->RunQuery(query);
	}

	void OnAccessAdd(ChannelInfo *ci, User *, ChanAccess *access)
	{
		SQLQuery query("INSERT INTO `anope_cs_access` (provider, data, mask, channel, last_seen, creator) VALUES (@provider, @data, @mask, @channel, @last_seen, @creator) ON DUPLICATE KEY UPDATE level=VALUES(level), display=VALUES(display), channel=VALUES(channel), last_seen=VALUES(last_seen), creator=VALUES(creator)");
		query.setValue("provider", access->provider->name);
		query.setValue("data", access->Serialize());
		query.setValue("mask", access->mask);
		query.setValue("channel", ci->name);
		query.setValue("last_seen", access->last_seen);
		query.setValue("creator", access->creator);
		this->RunQuery(query);
	}

	void OnAccessDel(ChannelInfo *ci, User *u, ChanAccess *access)
	{
		SQLQuery query("DELETE FROM `anope_cs_access` WHERE `mask` = @mask AND `channel` = @channel");
		query.setValue("mask", access->mask);
		query.setValue("channel", ci->name);
		this->RunQuery(query);
	}

	void OnAccessClear(ChannelInfo *ci, User *u)
	{
		SQLQuery query("DELETE FROM `anope_cs_access` WHERE `channel` = @channel");
		query.setValue("channel", ci->name);
		this->RunQuery(query);
	}

	void OnLevelChange(User *u, ChannelInfo *ci, int pos, int what)
	{
		if (pos >= 0)
		{
			SQLQuery query("UPDATE `anope_cs_levels` SET `level` = @level WHERE `channel` = @channel AND `position` = @pos");
			query.setValue("level", what);
			query.setValue("channel", ci->name);
			query.setValue("pos", pos);
			this->RunQuery(query);
		}
		else
		{
			SQLQuery query("INSERT INTO `anope_cs_levels` (level, channel, position) VALUES(@level, @channel, @pos) ON DUPLICATE KEY UPDATE level=VALUES(level), channel=VALUES(channel), position=VALUES(position)");
			query.setValue("channel", ci->name);
			for (int i = 0; i < CA_SIZE; ++i)
			{
				query.setValue("level", ci->levels[i]);
				query.setValue("pos", i);
				this->RunQuery(query);
			}
		}
	}

	void OnChanForbidden(ChannelInfo *ci)
	{
		SQLQuery query("INSERT INTO `anope_cs_info` (name, time_registered, last_used, flags) VALUES (@name, @time_registered, @last_used, @flags)");
		query.setValue("name", ci->name);
		query.setValue("time_registered", ci->time_registered);
		query.setValue("last_used", ci->last_used);
		query.setValue("flags", ToString(ci->ToString()));
		this->RunQuery(query);
	}

	void OnDelChan(ChannelInfo *ci)
	{
		SQLQuery query("DELETE FROM `anope_cs_info` WHERE `name` = @name");
		query.setValue("name", ci->name);
		this->RunQuery(query);
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		SQLQuery query("INSERT INTO `anope_cs_info` (name, founder, successor, descr, time_registered, last_used, last_topic,  last_topic_setter, last_topic_time, flags, bantype, memomax, botnick, botflags, capsmin, capspercent, floodlines, floodsecs, repeattimes) VALUES(@name, @founder, @successor, @descr, @time_registered, @last_used, @last_topic_text, @last_topic_setter, @last_topic_time, @flags, @bantype, @memomax, @botnick, @botflags, @capsmin, @capspercent, @floodlines, @floodsecs, @repeattimes) ON DUPLICATE KEY UPDATE founder=VALUES(founder), successor=VALUES(successor), descr=VALUES(descr), time_registered=VALUES(time_registered), last_used=VALUES(last_used), last_topic=VALUES(last_topic), last_topic_setter=VALUES(last_topic_setter),  last_topic_time=VALUES(last_topic_time), flags=VALUES(flags), bantype=VALUES(bantype), memomax=VALUES(memomax), botnick=VALUES(botnick), botflags=VALUES(botflags), capsmin=VALUES(capsmin), capspercent=VALUES(capspercent), floodlines=VALUES(floodlines), floodsecs=VALUES(floodsecs), repeattimes=VALUES(repeattimes)");
		query.setValue("name", ci->name);
		query.setValue("founder", ci->GetFounder() ? ci->GetFounder()->display : "");
		query.setValue("successor", ci->successor ? ci->successor->display : "");
		query.setValue("descr", ci->desc);
		query.setValue("time_registered", ci->time_registered);
		query.setValue("last_used", ci->last_used);
		query.setValue("last_topic_text", ci->last_topic);
		query.setValue("last_topic_setter", ci->last_topic_setter);
		query.setValue("last_topic_time", ci->last_topic_time);
		query.setValue("flags", ToString(ci->ToString()));
		query.setValue("bantype", ci->bantype);
		query.setValue("memomax", ci->memos.memomax);
		query.setValue("botnick", ci->bi ? ci->bi->nick : "");
		query.setValue("botflags", ToString(ci->botflags.ToString()));
		query.setValue("capsmin", ci->capsmin);
		query.setValue("capspercent", ci->capspercent);
		query.setValue("floodlines", ci->floodlines);
		query.setValue("floodsecs", ci->floodsecs);
		query.setValue("repeattimes", ci->repeattimes);
		this->RunQuery(query);

		for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
		{
			const ModeLock &ml = it->second;
			ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);

			if (cm != NULL)
			{
				query = "INSERT INTO `anope_cs_mlock` (channel, mode, status, setter, created, param) VALUES(@channel, @mode, @status, @setter, @created, @param) ON DUPLICATE KEY UPDATE channel=VALUES(channel), mode=VALUES(mode), status=VALUES(status), setter=VALUES(setter), created=VALUES(created), param=VALUES(param)";
				query.setValue("channel", ci->name);
				query.setValue("mode", cm->NameAsString());
				query.setValue("status", ml.set ? 1 : 0);
				query.setValue("setter", ml.setter);
				query.setValue("created", ml.created);
				query.setValue("param", ml.param);
				this->RunQuery(query);
			}
		}
	}

	void OnChanSuspend(ChannelInfo *ci)
	{
		SQLQuery query("UPDATE `anope_cs_info` SET `flags` = @flags WHERE `name` = @name");
		query.setValue("flags", ToString(ci->ToString()));
		query.setValue("name", ci->name);
		this->RunQuery(query);
	}

	void OnAkickAdd(ChannelInfo *ci, AutoKick *ak)
	{
		SQLQuery query("INSERT INTO `anope_cs_akick` (channel, flags, mask, reason, creator, created, last_used) VALUES(@channel, @flags, @mask, @reason, @creator, @created, @last_used)");
		query.setValue("channel", ci->name);
		query.setValue("flags", ak->HasFlag(AK_ISNICK) ? "ISNICK" : "");
		query.setValue("mask", ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask);
		query.setValue("reason", ak->reason);
		query.setValue("creator", ak->creator);
		query.setValue("created", ak->addtime);
		query.setValue("last_used", ak->last_used);
		this->RunQuery(query);
	}

	void OnAkickDel(ChannelInfo *ci, AutoKick *ak)
	{
		SQLQuery query("DELETE FROM `anope_cs_akick` WHERE `channel`= @mask AND `mask` = @mask");
		query.setValue("channel", ci->name);
		query.setValue("mask", ak->HasFlag(AK_ISNICK) ? ak->nc->display : ak->mask);
		this->RunQuery(query);
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm != NULL)
		{
			SQLQuery query("INSERT INTO `anope_cs_mlock` (channel, mode, status, setter, created, param) VALUES(@channel, @mode, @status, @setter, @created, @param) ON DUPLICATE KEY UPDATE channel=VALUES(channel), mode=VALUES(mode), status=VALUES(status), setter=VALUES(setter), created=VALUES(created), param=VALUES(param)");
			query.setValue("channel", ci->name);
			query.setValue("mode", cm->NameAsString());
			query.setValue("status", lock->set ? 1 : 0);
			query.setValue("setter", lock->setter);
			query.setValue("created", lock->created);
			query.setValue("param", lock->param);
			this->RunQuery(query);
		}
		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ChannelMode *mode, const Anope::string &param)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(mode->Name);
		if (cm != NULL)
		{
			SQLQuery query("DELETE FROM `anope_cs_mlock` WHERE `channel` = @channel AND `mode` = @mode AND `param` = @param");
			query.setValue("channel", ci->name);
			query.setValue("mode", mode->NameAsString());
			query.setValue("param", param);
			this->RunQuery(query);
		}
		return EVENT_CONTINUE;
	}

	void OnBotCreate(BotInfo *bi)
	{
		SQLQuery query("INSERT INTO `anope_bs_core` (nick, user, host, rname, flags, created, chancount) VALUES(@nick, @user, @host, @rname, @flags, @created, @chancount) ON DUPLICATE KEY UPDATE nick=VALUES(nick), user=VALUES(user), host=VALUES(host), rname=VALUES(rname), flags=VALUES(flags), created=VALUES(created), chancount=VALUES(chancount)");
		query.setValue("nick", bi->nick);
		query.setValue("user", bi->GetIdent());
		query.setValue("host", bi->host);
		query.setValue("rname", bi->realname);
		query.setValue("flags", ToString(bi->ToString()));
		query.setValue("created", bi->created);
		query.setValue("chancount", bi->chancount);
		this->RunQuery(query);
	}

	void OnBotChange(BotInfo *bi)
	{
		OnBotCreate(bi);
	}

	void OnBotDelete(BotInfo *bi)
	{
		SQLQuery query("UPDATE `anope_cs_info` SET `botnick` = '' WHERE `botnick` = @botnick");
		query.setValue("botnick", bi->nick);
		this->RunQuery(query);
	}

	EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi)
	{
		SQLQuery query("UPDATE `anope_cs_info` SET `botnick` = @botnick WHERE `name` = @channel");
		query.setValue("botnick", bi->nick);
		query.setValue("channel", ci->name);
		this->RunQuery(query);
		return EVENT_CONTINUE;
	}

	EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci)
	{
		SQLQuery query("UPDATE `anope_cs_info` SET `botnick` = '' WHERE `name` = @channel");
		query.setValue("channel", ci->name);
		this->RunQuery(query);
		return EVENT_CONTINUE;
	}

	void OnBadWordAdd(ChannelInfo *ci, BadWord *bw)
	{
		SQLQuery query("INSERT INTO `anope_bs_badwords` (channel, word, type) VALUES(@channel, @word, @type) ON DUPLICATE KEY UPDATE channel=VALUES(channel), word=VALUES(word), type=VALUES(type)");
		query.setValue("channel", ci->name);
		query.setValue("word", bw->word);
		switch (bw->type)
		{
			case BW_SINGLE:
				query.setValue("type", "SINGLE");
				break;
			case BW_START:
				query.setValue("type", "START");
				break;
			case BW_END:
				query.setValue("type", "END");
				break;
			default:
				query.setValue("type", "ANY");
		}
		this->RunQuery(query);
	}

	void OnBadWordDel(ChannelInfo *ci, BadWord *bw)
	{
		SQLQuery query("DELETE FROM `anope_bs_badwords` WHERE `channel` = @channel AND `word` = @word AND `type` = @type");
		query.setValue("channel", ci->name);
		query.setValue("word", bw->word);
		switch (bw->type)
		{
			case BW_SINGLE:
				query.setValue("type", "SINGLE");
				break;
			case BW_START:
				query.setValue("type", "START");
				break;
			case BW_END:
				query.setValue("type", "END");
				break;
			default:
				query.setValue("type", "ANY");
		}
		this->RunQuery(query);
	}

	void OnMemoSend(const Anope::string &source, const Anope::string &target, MemoInfo *mi, Memo *m)
	{
		const Anope::string &mtype = (!target.empty() && target[0] == '#' ? "CHAN" : "NICK");
		SQLQuery query("INSERT INTO `anope_ms_info` (receiver, flags, time, sender, text, serv) VALUES(@receiver, @flags, @time, @sender, @text, @serv)");
		query.setValue("receiver", target);
		query.setValue("flags", ToString(m->ToString()));
		query.setValue("time", m->time);
		query.setValue("sender", source);
		query.setValue("text", m->text);
		query.setValue("serv", mtype);
		this->RunQuery(query);
	}

	void OnMemoDel(const NickCore *nc, MemoInfo *mi, Memo *m)
	{
		SQLQuery query;

		if (m)
		{
			query = "DELETE FROM `anope_ms_info` WHERE `receiver` = @receiver AND `time` = @time";
			query.setValue("receiver", nc->display);
			query.setValue("time", m->time);
		}
		else
		{
			query = "DELETE FROM `anope_ms_info` WHERE `receiver` = @receiver";
			query.setValue("receiver", nc->display);
		}

		this->RunQuery(query);
	}

	void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, Memo *m)
	{
		SQLQuery query;

		if (m)
		{
			query = "DELETE FROM `anope_ms_info` WHERE `receiver` = @receiver AND `time` = @time";
			query.setValue("receiver", ci->name);
			query.setValue("time", m->time);
		}
		else
		{
			query = "DELETE FROM `anope_ms_info` WHERE `receiver` = @receiver";
			query.setValue("receiver", ci->name);
		}

		this->RunQuery(query);
	}

	EventReturn OnExceptionAdd(Exception *ex)
	{
		SQLQuery query("INSERT INTO `anope_os_exceptions` (mask, slimit, who, reason, time, expires) VALUES(@mask, @slimit, @who, @reason, @time, @expires)");
		query.setValue("mask", ex->mask);
		query.setValue("slimit", ex->limit);
		query.setValue("who", ex->who);
		query.setValue("reason", ex->reason);
		query.setValue("time", ex->time);
		query.setValue("expires", ex->expires);
		return EVENT_CONTINUE;
	}

	void OnExceptionDel(User *, Exception *ex)
	{
		SQLQuery query("DELETE FROM `anope_os_exceptions` WHERE `mask` = @mask");
		query.setValue("mask", ex->mask);
		this->RunQuery(query);
	}

	EventReturn OnAddXLine(XLine *x, XLineManager *xlm)
	{
		SQLQuery query("INSERT INTO `anope_os_xlines` (type, mask, xby, reason, seton, expire) VALUES(@type, @mask, @xby, @reason, @seton, @expire)");
		query.setValue("type", Anope::string(xlm->Type()));
		query.setValue("mask", x->Mask);
		query.setValue("xby", x->By);
		query.setValue("reason", x->Reason);
		query.setValue("seton", x->Created);
		query.setValue("expire", x->Expires);
		this->RunQuery(query);
		return EVENT_CONTINUE;
	}

	void OnDelXLine(User *, XLine *x, XLineManager *xlm)
	{
		SQLQuery query;

		if (x)
		{
			query = "DELETE FROM `anope_os_xlines` WHERE `mask` = @mask AND `type` = @type";
			query.setValue("mask", x->Mask);
			query.setValue("type", Anope::string(xlm->Type()));
		}
		else
		{
			query = "DELETE FROM `anope_os_xlines` WHERE `type` = @type";
			query.setValue("type", Anope::string(xlm->Type()));
		}

		this->RunQuery(query);
	}

	void OnDeleteVhost(NickAlias *na)
	{
		SQLQuery query("DELETE FROM `anope_hs_core` WHERE `nick` = @nick");
		query.setValue("nick", na->nick);
		this->RunQuery(query);
	}

	void OnSetVhost(NickAlias *na)
	{
		SQLQuery query("INSERT INTO `anope_hs_core` (nick, vident, vhost, creator, time) VALUES(@nick, @vident, @vhost, @creator, @time)");
		query.setValue("nick", na->nick);
		query.setValue("vident", na->hostinfo.GetIdent());
		query.setValue("vhost", na->hostinfo.GetHost());
		query.setValue("creator", na->hostinfo.GetCreator());
		query.setValue("time", na->hostinfo.GetTime());
		this->RunQuery(query);
	}
};

void MySQLInterface::OnResult(const SQLResult &r)
{
	Log(LOG_DEBUG) << "MySQL successfully executed query: " << r.finished_query;
}

void MySQLInterface::OnError(const SQLResult &r)
{
	if (!r.GetQuery().query.empty())
		Log(LOG_DEBUG) << "Error executing query " << r.finished_query << ": " << r.GetError();
	else
		Log(LOG_DEBUG) << "Error executing query: " << r.GetError();
}


static void Write(const Anope::string &data)
{
	SQLQuery query("INSERT INTO `anope_extra` (data) VALUES(@data)");
	query.setValue("data", data);
	me->RunQuery(query);
}

static void WriteNickMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurNick)
		throw CoreException("WriteNickMetadata without a nick to write");

	SQLQuery query("INSERT INTO `anope_ns_alias_metadata` (nick, name, value) VALUES(@nick, @name, @value)");
	query.setValue("nick", CurNick->nick);
	query.setValue("name", key);
	query.setValue("value", data);
	me->RunQuery(query);
}

static void WriteCoreMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurCore)
		throw CoreException("WritCoreMetadata without a core to write");

	SQLQuery query("INSERT INTO `anope_ns_core_metadata` (nick, name, value) VALUES(@nick, @name, @value)");
	query.setValue("nick", CurCore->display);
	query.setValue("name", key);
	query.setValue("value", data);
	me->RunQuery(query);
}

static void WriteChannelMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurChannel)
		throw CoreException("WriteChannelMetadata without a channel to write");

	SQLQuery query("INSERT INTO `anope_cs_info_metadata` (channel, name, value) VALUES(@channel, @name, @value)");
	query.setValue("channel", CurChannel->name);
	query.setValue("name", key);
	query.setValue("value", data);
	me->RunQuery(query);
}

static void WriteBotMetadata(const Anope::string &key, const Anope::string &data)
{
	if (!CurBot)
		throw CoreException("WriteBotMetadata without a bot to write");

	SQLQuery query("INSERT INTO `anope_bs_info_metadata` (botname, name, value) VALUES(@botname, @name, @value)");
	query.setValue("botname", CurBot->nick);
	query.setValue("name", key);
	query.setValue("value", data);
	me->RunQuery(query);
}

static void SaveDatabases()
{
	SQLQuery query;

	query = "TRUNCATE TABLE `anope_ns_core`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_ms_info`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_ns_alias`";
	me->RunQuery(query);

	for (nickcore_map::const_iterator nit = NickCoreList.begin(), nit_end = NickCoreList.end(); nit != nit_end; ++nit)
	{
		NickCore *nc = nit->second;

		me->InsertCore(nc);

		for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
		{
			me->InsertAlias(*it);
			if ((*it)->hostinfo.HasVhost())
				me->OnSetVhost(*it);
		}

		for (std::vector<Anope::string>::iterator it = nc->access.begin(), it_end = nc->access.end(); it != it_end; ++it)
		{
			query = "INSERT INTO `anope_ns_access` (display, access) VALUES(@display, @access)";
			query.setValue("display", nc->display);
			query.setValue("access", *it);
			me->RunQuery(query);
		}

		for (unsigned j = 0, end = nc->memos.memos.size(); j < end; ++j)
		{
			Memo *m = nc->memos.memos[j];

			me->OnMemoSend(m->sender, nc->display, &nc->memos, m);
		}
	}

	query = "TRUNCATE TABLE `anope_bs_core`";
	me->RunQuery(query);

	for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		me->OnBotCreate(it->second);

	query = "TRUNCATE TABLE `anope_cs_info`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_bs_badwords`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_cs_access`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_cs_akick`";
	me->RunQuery(query);

	query = "TRUNCATE TABLE `anope_cs_levels`";
	me->RunQuery(query);

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

			me->OnAccessAdd(ci, NULL, access);
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);

			me->OnAkickAdd(ci, ak);
		}

		me->OnLevelChange(NULL, ci, -1, -1);

		for (unsigned j = 0, end = ci->memos.memos.size(); j < end; ++j)
		{
			Memo *m = ci->memos.memos[j];

			me->OnMemoSend(m->sender, ci->name, &ci->memos, m);
		}
	}

	for (std::list<XLineManager *>::iterator it = XLineManager::XLineManagers.begin(), it_end = XLineManager::XLineManagers.end(); it != it_end; ++it)
		for (unsigned i = 0, end = (*it)->GetCount(); i < end; ++i)
			me->OnAddXLine((*it)->GetEntry(i), *it);

	if (me->SessionInterface)
		for (SessionService::ExceptionVector::iterator it = me->SessionInterface->GetExceptions().begin(); it != me->SessionInterface->GetExceptions().end(); ++it)
			me->OnExceptionAdd(*it);
}

void CommandSQLSync::Execute(CommandSource &source, const std::vector<Anope::string> &params)
{
	SaveDatabases();
	source.Reply(_("Updating MySQL."));
	return;
}

MODULE_INIT(DBMySQL)

