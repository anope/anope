#include "module.h"
#include "modules/sql.h"

class MySQLInterface : public SQL::Interface
{
 public:
	MySQLInterface(Module *o) : SQL::Interface(o) { }

	void OnResult(const SQL::Result &r) override
	{
	}

	void OnError(const SQL::Result &r) override
	{
		if (!r.GetQuery().query.empty())
			Log(LOG_DEBUG) << "m_irc2sql: Error executing query " << r.finished_query << ": " << r.GetError();
		else
			Log(LOG_DEBUG) << "m_irc2sql: Error executing query: " << r.GetError();
	}
};

class IRC2SQL : public Module
{
	ServiceReference<SQL::Provider> sql;
	MySQLInterface sqlinterface;
	SQL::Query query;
	std::vector<Anope::string> TableList, ProcedureList, EventList;
	Anope::string prefix, GeoIPDB;
	bool quitting, introduced_myself, ctcpuser, ctcpeob, firstrun;
	ServiceBot *StatServ;
	PrimitiveExtensibleItem<bool> versionreply;

	void RunQuery(const SQL::Query &q);
	void GetTables();

	bool HasTable(const Anope::string &table);
	bool HasProcedure(const Anope::string &table);
	bool HasEvent(const Anope::string &table);

	void CheckTables();

 public:
	IRC2SQL(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, EXTRA | VENDOR), sql("", ""), sqlinterface(this), versionreply(this, "CTCPVERSION")
	{
		firstrun = true;
		quitting = false;
		introduced_myself = false;
	}

	void OnShutdown() override;
	void OnReload(Configuration::Conf *config) override;
	void OnNewServer(Server *server) override;
	void OnServerQuit(Server *server) override;
	void OnUserConnect(User *u, bool &exempt) override;
	void OnUserQuit(User *u, const Anope::string &msg) override;
	void OnUserNickChange(User *u, const Anope::string &oldnick) override;
	void OnFingerprint(User *u) override;
	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override;
	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override;
	void OnUserLogin(User *u) override;
	void OnNickLogout(User *u) override;
	void OnSetDisplayedHost(User *u) override;

	void OnChannelCreate(Channel *c) override;
	void OnChannelDelete(Channel *c) override;
	void OnLeaveChannel(User *u, Channel *c) override;
	void OnJoinChannel(User *u, Channel *c) override;
	EventReturn OnChannelModeSet(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override;
	EventReturn OnChannelModeUnset(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override;

	void OnTopicUpdated(Channel *c, const Anope::string &user, const Anope::string &topic) override;

	void OnBotNotice(User *u, ServiceBot *bi, Anope::string &message) override;
};


