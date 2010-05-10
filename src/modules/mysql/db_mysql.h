#include "module.h"

struct NickAliasFlagInfo
{
	std::string Name;
	NickNameFlag Flag;
};

NickAliasFlagInfo NickAliasFlags[] = {
	{"FORBIDDEN", NS_FORBIDDEN},
	{"NOEXPIRE", NS_NO_EXPIRE},
	{"", static_cast<NickNameFlag>(-1)}
};

struct NickCoreFlagInfo
{
	std::string Name;
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

struct ChannelModeInfo
{
	std::string Name;
	ChannelModeName Mode;
};

ChannelModeInfo ChannelModes[] = {
	{"CMODE_BLOCKCOLOR", CMODE_BLOCKCOLOR},
	{"CMODE_FLOOD", CMODE_FLOOD},
	{"CMODE_INVITE", CMODE_INVITE},
	{"CMODE_KEY", CMODE_KEY},
	{"CMODE_LIMIT", CMODE_LIMIT},
	{"CMODE_MODERATED", CMODE_MODERATED},
	{"CMODE_NOEXTERNAL", CMODE_NOEXTERNAL},
	{"CMODE_PRIVATE", CMODE_PRIVATE},
	{"CMODE_REGISTERED", CMODE_REGISTERED},
	{"CMODE_SECRET", CMODE_SECRET},
	{"CMODE_TOPIC", CMODE_TOPIC},
	{"CMODE_AUDITORIUM", CMODE_AUDITORIUM},
	{"CMODE_SSL", CMODE_SSL},
	{"CMODE_ADMINONLY", CMODE_ADMINONLY},
	{"CMODE_NOCTCP", CMODE_NOCTCP},
	{"CMODE_FILTER", CMODE_FILTER},
	{"CMODE_NOKNOCK", CMODE_NOKNOCK},
	{"CMODE_REDIRECT", CMODE_REDIRECT},
	{"CMODE_REGMODERATED", CMODE_REGMODERATED},
	{"CMODE_NONICK", CMODE_NONICK},
	{"CMODE_OPERONLY", CMODE_OPERONLY},
	{"CMODE_NONICK", CMODE_NONICK},
	{"CMODE_REGISTEREDONLY", CMODE_REGISTEREDONLY},
	{"CMODE_STRIPCOLOR", CMODE_STRIPCOLOR},
	{"CMODE_NONOTICE", CMODE_NONOTICE},
	{"CMODE_NOINVITE", CMODE_NOINVITE},
	{"CMODE_ALLINVITE", CMODE_ALLINVITE},
	{"CMODE_BLOCKCAPS", CMODE_BLOCKCAPS},
	{"CMODE_PERM", CMODE_PERM},
	{"CMODE_NICKFLOOD", CMODE_NICKFLOOD},
	{"CMODE_JOINFLOOD", CMODE_JOINFLOOD},
	{"CMODE_NOREJOIN", CMODE_NOREJOIN},
	{"", static_cast<ChannelModeName>(-1)}
};

struct BotFlagInfo
{
	std::string Name;
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
	{"", static_cast<BotServFlag>(-1)}
};

struct ChannelFlagInfo
{
	std::string Name;
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

struct BotServFlagInfo
{
	std::string Name;
	BotFlag Flag;
};

BotServFlagInfo BotServFlags[] = {
	{"PRIVATE", BI_PRIVATE},
	{"CHANSERV", BI_CHANSERV},
	{"BOTSERV", BI_BOTSERV},
	{"HOSTSERV", BI_HOSTSERV},
	{"OPERSERV", BI_OPERSERV},
	{"MEMOSERV", BI_MEMOSERV},
	{"NICKSERV", BI_NICKSERV},
	{"GLOBAL", BI_GLOBAL},
	{"", static_cast<BotFlag>(-1)}
};

struct MemoFlagInfo
{
	std::string Name;
	MemoFlag Flag;
};

MemoFlagInfo MemoFlags[] = {
	{"UNREAD", MF_UNREAD},
	{"RECEIPT", MF_RECEIPT},
	{"NOTIFYS", MF_NOTIFYS},
	{"", static_cast<MemoFlag>(-1)}
};

#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>

inline std::string SQLAssign(const mysqlpp::String& s) { return s.c_str(); }

class DBMySQL;
static DBMySQL *Me;

bool ExecuteQuery(mysqlpp::Query& query)
{
	Alog(LOG_DEBUG) << "MySQL: " << query.str();
	
	if (!query.execute())
	{
		Alog() << "MySQL: error executing query: " << query.error();
		return false;
	}

	return true;
}

mysqlpp::StoreQueryResult StoreQuery(mysqlpp::Query& query)
{
	Alog(LOG_DEBUG) << "MySQL: " << query.str();

	mysqlpp::StoreQueryResult result = query.store();
	if (!result)
	{
		Alog() << "MySQL: error executing query: " << query.error();
	}
	return result;
}

class DBMySQL : public Module
{
 private:
	bool LoadConfig()
	{
		ConfigReader config;

		Database = config.ReadValue("mysql", "database", "anope", 0);
		Server = config.ReadValue("mysql", "server", "127.0.0.1", 0);
		SQLUser = config.ReadValue("mysql", "username", "anope", 0);
		Password = config.ReadValue("mysql", "password", "", 0);
		Port = config.ReadInteger("mysql", "port", "3306", 0, true);
		Delay = config.ReadInteger("mysql", "updatedelay", "60", 0, true);

		return !Password.empty();
	}

 public:
 	mysqlpp::Connection *Con;
	mysqlpp::NoExceptions *Ne;

 	std::string Database;
	std::string Server;
	std::string SQLUser;
	std::string Password;
	unsigned int Port;
	unsigned int Delay;

	DBMySQL(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Me = this;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(DATABASE);

		if (!LoadConfig())
			throw ModuleException("Couldn't load config");

		Con = new mysqlpp::Connection(false);
		Ne = new mysqlpp::NoExceptions(Con);
		if (!Con->connect(Database.c_str(), Server.c_str(), SQLUser.c_str(), Password.c_str(), Port))
		{
			std::string Error = "MySQL: Error connecting to SQL server: ";
			Error += Con->error();
			delete Con;
			throw ModuleException(Error.c_str());
		}
		
		mysqlpp::Query query(Con);
		query << "SET NAMES 'utf8'";
		ExecuteQuery(query);
	}

	virtual ~DBMySQL()
	{
		delete Ne;
		delete Con;
	}
};

