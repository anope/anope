/* RequiredLibraries: mysqlpp */

#include "db_mysql.h"

static std::vector<std::string> MakeVector(std::string buf)
{
	std::string s;
	spacesepstream sep(buf);
	std::vector<std::string> params;

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

static void LoadDatabase()
{
	mysqlpp::Query query(Me->Con);
	mysqlpp::StoreQueryResult qres;

	query << "SELECT * FROM `anope_ns_core`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc = new NickCore(SQLAssign(qres[i]["display"]));
			nc->pass = SQLAssign(qres[i]["pass"]);
			if (!qres[i]["email"].empty())
				nc->email = sstrdup(qres[i]["email"].c_str());
			if (!qres[i]["greet"].empty())
				nc->greet = sstrdup(qres[i]["greet"].c_str());
			if (!qres[i]["icq"].empty())
				nc->icq = atol(qres[i]["icq"].c_str());
			if (!qres[i]["url"].empty())
				nc->url = sstrdup(qres[i]["url"].c_str());

			spacesepstream sep(SQLAssign(qres[i]["flags"]));
			std::string buf;
			while (sep.GetToken(buf))
			{
				for (int j = 0; NickCoreFlags[j].Flag != -1; ++j)
				{
					if (NickCoreFlags[j].Name == buf)
					{
						nc->SetFlag(NickCoreFlags[j].Flag);
					}
				}
			}

			nc->language = atoi(qres[i]["language"].c_str());
			nc->channelcount = atoi(qres[i]["channelcount"].c_str());
			nc->memos.memomax = atoi(qres[i]["memomax"].c_str());
		}
	}

	query << "SELECT * FROM `anope_ns_access`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc = findcore(qres[i]["display"].c_str());
			if (!nc)
			{
				Alog() << "MySQL: Got NickCore access entry for nonexistant core " << qres[i]["display"];
				continue;
			}

			nc->AddAccess(SQLAssign(qres[i]["access"]));
		}
	}

	query << "SELECT * FROM `anope_ns_core_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc = findcore(qres[i]["display"].c_str());
			if (!nc)
			{
				Alog() << "MySQL: Got NickCore access entry for nonexistant core " << qres[i]["display"];
				continue;
			}
			try
			{
				EventReturn MOD_RESULT;
				std::vector<std::string> Params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nc, SQLAssign(qres[i]["name"]), Params));
			}
			catch (const char *err)
			{
				Alog() << "[db_mysql_read]: " << err;
			}
		}
	}

	query << "SELECT * FROM `anope_ns_alias`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc = findcore(qres[i]["display"].c_str());
			if (!nc)
			{
				Alog() << "MySQL: Got NickAlias for nick " << qres[i]["nick"] << " with nonexistant core " << qres[i]["display"];
				continue;
			}

			NickAlias *na = new NickAlias(SQLAssign(qres[i]["nick"]), nc);
			na->last_quit = sstrdup(qres[i]["last_quit"].c_str());
			na->last_realname = sstrdup(qres[i]["last_realname"].c_str());
			na->last_usermask = sstrdup(qres[i]["last_usermask"].c_str());
			na->time_registered = atol(qres[i]["time_registered"].c_str());
			na->last_seen = atol(qres[i]["last_seen"].c_str());

			spacesepstream sep(SQLAssign(qres[i]["flags"]));
			std::string buf;
			while (sep.GetToken(buf))
			{
				for (int j = 0; NickAliasFlags[j].Flag != -1; ++j)
				{
					if (NickAliasFlags[j].Name == buf)
					{
						na->SetFlag(NickAliasFlags[j].Flag);
					}
				}
			}
		}
	}

	query << "SELECT * FROM `anope_ns_alias_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickAlias *na = findnick(SQLAssign(qres[i]["nick"]));
			if (!na)
			{
				Alog() << "MySQL: Got metadata for nonexistant nick " << qres[i]["nick"];
				continue;
			}
			try
			{
				EventReturn MOD_RESULT;
				std::vector<std::string> Params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, SQLAssign(qres[i]["name"]), Params));
			}
			catch (const char *err)
			{
				Alog() << "[db_mysql_read]: " << err;
			}
		}
	}

	query << "SELECT * FROM `anope_bs_core`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			BotInfo *bi = new BotInfo(SQLAssign(qres[i]["nick"]), SQLAssign(qres[i]["user"]), SQLAssign(qres[i]["host"]), SQLAssign(qres[i]["rname"]));
			if (!qres[i]["flags"].empty())
			{
				spacesepstream sep(SQLAssign(qres[i]["flags"]));
				std::string buf;
				while (sep.GetToken(buf))
				{
					for (unsigned j = 0; BotServFlags[j].Flag != -1; ++j)
					{
						if (buf == BotServFlags[j].Name)
						{
							bi->SetFlag(BotServFlags[j].Flag);
							break;
						}
					}
				}
			}
			bi->created = atol(qres[i]["created"]);
			bi->chancount = atol(qres[i]["chancount"]);
		}
	}

	query << "SELECT * FROM `anope_bs_info_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			BotInfo *bi = findbot(SQLAssign(qres[i]["botname"]));
			if (!bi)
			{
				Alog() << "MySQL: BotInfo metadata for nonexistant bot " << qres[i]["botname"];
				continue;
			}

			try
			{
				EventReturn MOD_RESULT;
				std::vector<std::string> Params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(bi, SQLAssign(qres[i]["name"]), Params));
			}
			catch (const char *err)
			{
				Alog() << "[db_mysql_read]: " << err;
			}
		}
	}

	query << "SELECT * FROM `anope_cs_info`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc;
			if (!qres[i]["founder"].empty())
			{
				nc = findcore(qres[i]["founder"].c_str());
				if (!nc)
				{
					Alog() << "MySQL: Channel " << qres[i]["name"] << " with nonexistant founder " << qres[i]["founder"];
					continue;
				}
			}

			ChannelInfo *ci = new ChannelInfo(SQLAssign(qres[i]["name"]));
			ci->founder = nc;
			if (!qres[i]["successor"].empty())
				ci->successor = findcore(qres[i]["successor"].c_str());
			ci->desc = sstrdup(qres[i]["descr"].c_str());
			if (!qres[i]["url"].empty())
				ci->url = sstrdup(qres[i]["url"].c_str());
			if (!qres[i]["email"].empty())
				ci->email = sstrdup(qres[i]["email"].c_str());
			ci->time_registered = atol(qres[i]["time_registered"]);
			ci->last_used = atol(qres[i]["last_used"]);
			if (!qres[i]["last_topic"].empty())
				ci->last_topic = sstrdup(qres[i]["last_topic"].c_str());
			if (!qres[i]["last_topic_setter"].empty())
				ci->last_topic_setter = SQLAssign(qres[i]["last_topic_setter"]);
			if (!qres[i]["last_topic_time"].empty())
				ci->last_topic_time = atol(qres[i]["last_topic_time"].c_str());
			if (!qres[i]["flags"].empty())
			{
				std::string buf;
				spacesepstream sep(SQLAssign(qres[i]["flags"]));
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
			if (!qres[i]["forbidby"].empty())
				ci->forbidby = sstrdup(qres[i]["forbidby"].c_str());
			if (!qres[i]["forbidreason"].empty())
				ci->forbidreason = sstrdup(qres[i]["forbidreason"].c_str());
			ci->bantype = atoi(qres[i]["bantype"].c_str());
			if (!qres[i]["mlock_on"].empty())
			{
				std::string buf;
				spacesepstream sep(SQLAssign(qres[i]["mlock_on"]));
				while (sep.GetToken(buf))
				{
					for (int j = 0; ChannelModes[j].Mode != -1; ++j)
					{
						if (buf == ChannelModes[j].Name)
						{
							ci->SetMLock(ChannelModes[j].Mode, true);
							break;
						}
					}
				}
			}
			if (!qres[i]["mlock_off"].empty())
			{
				std::string buf;
				spacesepstream sep(SQLAssign(qres[i]["mlock_off"]));
				while (sep.GetToken(buf))
				{
					for (int j = 0; ChannelModes[j].Mode != -1; ++j)
					{
						if (buf == ChannelModes[j].Name)
						{
							ci->SetMLock(ChannelModes[j].Mode, false);
							break;
						}
					}
				}
			}
			if (!qres[i]["mlock_params"].empty())
			{
				std::string buf;
				spacesepstream sep(SQLAssign(qres[i]["mlock_params"]));
				while (sep.GetToken(buf))
				{
					for (int j = 0; ChannelModes[j].Mode != -1; ++j)
					{
						if (buf == ChannelModes[j].Name)
						{
							sep.GetToken(buf);
							ci->SetMLock(ChannelModes[j].Mode, true, buf);
							break;
						}
					}
				}
			}
			if (!qres[i]["entry_message"].empty())
				ci->entry_message = sstrdup(qres[i]["entry_message"].c_str());
			ci->memos.memomax = atoi(qres[i]["memomax"].c_str());
			if (!qres[i]["botnick"].empty())
				ci->bi = findbot(SQLAssign(qres[i]["botnick"]));
			if (ci->bi)
			{
				if (!qres[i]["botflags"].empty())
				{
					std::string buf;
					spacesepstream sep(SQLAssign(qres[i]["botflags"]));
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
			}
			if (!qres[i]["capsmin"].empty())
				ci->capsmin = atoi(qres[i]["capsmin"].c_str());
			if (!qres[i]["capspercent"].empty())
				ci->capspercent = atoi(qres[i]["capspercent"].c_str());
			if (!qres[i]["floodlines"].empty())
				ci->floodlines = atoi(qres[i]["floodlines"].c_str());
			if (!qres[i]["floodsecs"].empty())
				ci->floodsecs = atoi(qres[i]["floodsecs"].c_str());
			if (!qres[i]["repeattimes"].empty())
				ci->repeattimes = atoi(qres[i]["repeattimes"].c_str());
		}
	}

	query << "SELECT * FROM `anope_cs_ttb";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (!ci)
			{
				Alog() << "MySQL: Channel ttb for nonexistant channel " << qres[i]["channel"];
				continue;
			}

			ci->ttb[atoi(qres[i]["ttb_id"].c_str())] = atoi(qres[i]["value"].c_str());
		}
	}

	query << "SELECT * FROM `anope_bs_badwords`";
	qres = StoreQuery(query);
	
	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (!ci)
			{
				Alog() << "MySQL: Channel badwords entry for nonexistant channel " << qres[i]["channel"];
				continue;
			}
			
			BadWordType BWTYPE = BW_ANY;
			if (qres[i]["type"] == "SINGLE")
				BWTYPE = BW_SINGLE;
			else if (qres[i]["type"] == "START")
				BWTYPE = BW_START;
			else if (qres[i]["type"] == "END")
				BWTYPE = BW_END;
			ci->AddBadWord(SQLAssign(qres[i]["word"]), BWTYPE);
		}
	}

	query << "SELECT * FROM `anope_cs_access`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (!ci)
			{
				Alog() << "MySQL: Channel access entry for nonexistant channel " << qres[i]["channel"];
				continue;
			}
			NickCore *nc = findcore(qres[i]["display"]);
			if (!nc)
			{
				Alog() << "MySQL: Channel access entry for " << ci->name << " with nonexistant nick " << qres[i]["display"];
				continue;
			}

			ci->AddAccess(nc, atoi(qres[i]["level"]), SQLAssign(qres[i]["creator"]), atol(qres[i]["last_seen"]));
		}
	}

	query << "SELECT * FROM `anope_cs_levels`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (!ci)
			{
				Alog() << "MySQL: Channel level entry for nonexistant channel " << qres[i]["channel"];
				continue;
			}
			ci->levels[atoi(qres[i]["position"])] = atoi(qres[i]["level"]);
		}
	}

	query << "SELECT * FROM `anope_cs_info_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (!ci)
			{
				Alog() << "MySQL: Channel metadata for nonexistant channel " << qres[i]["channel"];
				continue;
			}
			try
			{
				EventReturn MOD_RESULT;
				std::vector<std::string> Params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, SQLAssign(qres[i]["name"]), Params));
			}
			catch (const char *err)
			{
				Alog() << "[db_mysql_read]: " << err;
			}
		}
	}

	query << "SELECT * FROM `anope_ns_request`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickRequest *nr = new NickRequest(qres[i]["nick"].c_str());
			nr->passcode = SQLAssign(qres[i]["passcode"]);
			nr->password = SQLAssign(qres[i]["password"]);
			nr->email = sstrdup(qres[i]["email"].c_str());
			nr->requested = atol(qres[i]["requested"].c_str());
		}
	}

	EventReturn MOD_RESULT;
	query << "SELECT * FROM `anope_extra`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			std::vector<std::string> params = MakeVector(SQLAssign(qres[i]["data"]));
			FOREACH_RESULT(I_OnDatabaseRead, OnDatabaseRead(params));
		}
	}

	query << "SELECT * FROM `anope_ns_core_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickCore *nc = findcore(qres[i]["nick"].c_str());
			if (nc)
			{
				std::vector<std::string> params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(nc, SQLAssign(qres[i]["name"]), params));
			}
		}
	}

	query << "SELECT * FROM `anope_ns_alias_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			NickAlias *na = findnick(SQLAssign(qres[i]["nick"]));
			if (na)
			{
				std::vector<std::string> params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(na, SQLAssign(qres[i]["name"]), params));
			}
		}
	}

	query << "SELECT * FROM `anope_cs_info_metadata`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["channel"]));
			if (ci)
			{
				std::vector<std::string> params = MakeVector(SQLAssign(qres[i]["value"]));
				FOREACH_RESULT(I_OnDatabaseReadMetadata, OnDatabaseReadMetadata(ci, SQLAssign(qres[i]["name"]), params));
			}
		}
	}

	query << "SELECT * FROM `anope_ms_info`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.num_rows(); ++i)
		{
			MemoInfo *mi = NULL;
			if (qres[i]["serv"] == "NICK")
			{
				NickCore *nc = findcore(qres[i]["receiver"].c_str());
				if (nc)
					mi = &nc->memos;
			}
			else if (qres[i]["serv"] == "CHAN")
			{
				ChannelInfo *ci = cs_findchan(SQLAssign(qres[i]["receiver"]));
				if (ci)
					mi = &ci->memos;
			}
			if (mi)
			{
				Memo *m = new Memo();
				mi->memos.push_back(m);
				m->sender = SQLAssign(qres[i]["sender"]);
				if (mi->memos.size() > 1)
				{
					m->number = mi->memos[mi->memos.size() - 2]->number + 1;
					if (m->number < 1)
					{
						for (unsigned j = 0; j < mi->memos.size(); ++j)
							mi->memos[j]->number = j + 1;
					}
				}
				else
				{
					m->number = 1;
				}
				m->time = atol(qres[i]["time"].c_str());
				m->text = sstrdup(qres[i]["text"].c_str());
				
				if (!qres[i]["flags"].empty())
				{
					spacesepstream sep(SQLAssign(qres[i]["flags"]));
					std::string buf;
					while (sep.GetToken(buf))
					{
						for (unsigned j = 0; MemoFlags[j].Flag != -1; ++j)
						{
							if (MemoFlags[j].Name == buf)
							{
								m->SetFlag(MemoFlags[j].Flag);
							}
						}
					}
				}
			}
		}
	}

	query << "SELECT * FROM `anope_os_akills`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.size(); ++i)
		{
			Akill *ak = new Akill;
			ak->user = sstrdup(qres[i]["user"].c_str());
			ak->host = sstrdup(qres[i]["host"].c_str());
			ak->by = sstrdup(qres[i]["xby"].c_str());
			ak->reason = sstrdup(qres[i]["reason"].c_str());
			ak->seton = atol(qres[i]["seton"].c_str());
			ak->expires = atol(qres[i]["expire"].c_str());
			slist_add(&akills, ak);
		}
	}

	query << "SELECT * FROM `anope_os_sxlines`";
	qres = StoreQuery(query);

	if (qres)
	{
		for (size_t i = 0; i < qres.size(); ++i)
		{
			SXLine *sx = new SXLine;
			sx->mask = sstrdup(qres[i]["mask"].c_str());
			sx->by = sstrdup(qres[i]["xby"].c_str());
			sx->reason = sstrdup(qres[i]["reason"].c_str());
			sx->seton = atol(qres[i]["seton"].c_str());
			sx->expires = atol(qres[i]["expires"].c_str());
			if (qres[i]["type"] == "SGLINE")
				slist_add(&sglines, sx);
			else if (qres[i]["type"] == "SQLINE")
				slist_add(&sqlines, sx);
			else if (qres[i]["type"] == "SZLINE")
				slist_add(&szlines, sx);
			else
				delete sx;
		}
	}
}

class DBMySQLRead : public DBMySQL
{
 public:
	DBMySQLRead(const std::string &modname, const std::string &creator) : DBMySQL(modname, creator)
	{
		Implementation i[] = { I_OnLoadDatabase };
		ModuleManager::Attach(i, this, 1);
	}

	~DBMySQLRead()
	{
	}

	EventReturn OnLoadDatabase()
	{
		LoadDatabase();

		/* No need to ever reload this again, although this should never be triggered again */
		ModuleManager::Detach(I_OnLoadDatabase, this);

		return EVENT_STOP;
	}
};

MODULE_INIT(DBMySQLRead)
