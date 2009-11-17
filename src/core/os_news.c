/* OperServ core functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"
#include "pseudo.h" // Remove once new dbs are added

/* List of messages for each news type.  This simplifies message sending. */

#define MSG_SYNTAX	0
#define MSG_LIST_HEADER	1
#define MSG_LIST_ENTRY	2
#define MSG_LIST_NONE	3
#define MSG_ADD_SYNTAX	4
#define MSG_ADD_FULL	5
#define MSG_ADDED	6
#define MSG_DEL_SYNTAX	7
#define MSG_DEL_NOT_FOUND 8
#define MSG_DELETED	9
#define MSG_DEL_NONE	10
#define MSG_DELETED_ALL	11
#define MSG_MAX		11

struct newsmsgs msgarray[] = {
	{NEWS_LOGON, "LOGON",
	 {NEWS_LOGON_SYNTAX,
	  NEWS_LOGON_LIST_HEADER,
	  NEWS_LOGON_LIST_ENTRY,
	  NEWS_LOGON_LIST_NONE,
	  NEWS_LOGON_ADD_SYNTAX,
	  NEWS_LOGON_ADD_FULL,
	  NEWS_LOGON_ADDED,
	  NEWS_LOGON_DEL_SYNTAX,
	  NEWS_LOGON_DEL_NOT_FOUND,
	  NEWS_LOGON_DELETED,
	  NEWS_LOGON_DEL_NONE,
	  NEWS_LOGON_DELETED_ALL}
	 },
	{NEWS_OPER, "OPER",
	 {NEWS_OPER_SYNTAX,
	  NEWS_OPER_LIST_HEADER,
	  NEWS_OPER_LIST_ENTRY,
	  NEWS_OPER_LIST_NONE,
	  NEWS_OPER_ADD_SYNTAX,
	  NEWS_OPER_ADD_FULL,
	  NEWS_OPER_ADDED,
	  NEWS_OPER_DEL_SYNTAX,
	  NEWS_OPER_DEL_NOT_FOUND,
	  NEWS_OPER_DELETED,
	  NEWS_OPER_DEL_NONE,
	  NEWS_OPER_DELETED_ALL}
	 },
	{NEWS_RANDOM, "RANDOM",
	 {NEWS_RANDOM_SYNTAX,
	  NEWS_RANDOM_LIST_HEADER,
	  NEWS_RANDOM_LIST_ENTRY,
	  NEWS_RANDOM_LIST_NONE,
	  NEWS_RANDOM_ADD_SYNTAX,
	  NEWS_RANDOM_ADD_FULL,
	  NEWS_RANDOM_ADDED,
	  NEWS_RANDOM_DEL_SYNTAX,
	  NEWS_RANDOM_DEL_NOT_FOUND,
	  NEWS_RANDOM_DELETED,
	  NEWS_RANDOM_DEL_NONE,
	  NEWS_RANDOM_DELETED_ALL}
	 }
};

#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", NewsDBName);	\
	break;						\
	}							\
} while (0)

void load_news()
{
	dbFILE *f;
	int i;
	uint16 n, type;
	uint32 tmp32;
	NewsItem *news;
	char *text;

	if (!(f = open_db(s_OperServ, NewsDBName, "r", NEWS_VERSION)))
		return;
	switch (i = get_file_version(f)) {
	case 9:
	case 8:
	case 7:
		SAFE(read_int16(&n, f));
		if (!n) {
			close_db(f);
			return;
		}
		for (i = 0; i < n; i++) {
			news = new NewsItem;

			SAFE(read_int16(&type, f));
			news->type = static_cast<NewsType>(type);
			SAFE(read_int32(&news->num, f));
			SAFE(read_string(&text, f));
			news->Text = text;
			delete [] text;
			SAFE(read_buffer(news->who, f));
			SAFE(read_int32(&tmp32, f));
			news->time = tmp32;

			News.push_back(news);
		}
		break;

	default:
		fatal("Unsupported version (%d) on %s", i, NewsDBName);
	}						   /* switch (ver) */

	close_db(f);
}

#undef SAFE

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", NewsDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", NewsDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)

void save_news()
{
	dbFILE *f;
	static time_t lastwarn = 0;

	if (!(f = open_db(s_OperServ, NewsDBName, "w", NEWS_VERSION)))
		return;
	SAFE(write_int16(News.size(), f));
	for (unsigned i = 0; i < News.size(); i++) {
		SAFE(write_int16(News[i]->type, f));
		SAFE(write_int32(News[i]->num, f));
		SAFE(write_string(News[i]->Text.c_str(), f));
		SAFE(write_buffer(News[i]->who, f));
		SAFE(write_int32(News[i]->time, f));
	}
	close_db(f);
}

#undef SAFE

static void DisplayNews(User *u, NewsType Type)
{
	int msg;
	static unsigned current_news = 0;

	if (Type == NEWS_LOGON)
		msg = NEWS_LOGON_TEXT;
	else if (Type == NEWS_OPER)
		msg = NEWS_OPER_TEXT;
	else if (Type == NEWS_RANDOM)
		msg = NEWS_RANDOM_TEXT;
	else
	{
		alog("news: Invalid type (%d) to display_news()", Type);
		return;
	}

	unsigned displayed = 0;
	bool NewsExists = false;
	for (unsigned i = 0; i < News.size(); ++i)
	{
		if (News[i]->type == Type)
		{
			tm *tm;
			char timebuf[64];

			NewsExists = true;

			if (Type == NEWS_RANDOM && i == current_news)
				continue;

			tm = localtime(&News[i]->time);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, tm);
			notice_lang(s_GlobalNoticer, u, msg, timebuf, News[i]->Text.c_str());

			++displayed;

			if (Type == NEWS_RANDOM)
			{
				current_news = i;
				return;
			}
			else if (displayed >= NewsCount)
				return;
		}

		/* Reset to head of list to get first random news value */
		if (i + 1 == News.size() && Type == NEWS_RANDOM && NewsExists)
			i = 0;
	}
}

static int add_newsitem(User * u, const char *text, NewsType type)
{
	int num = 0;

	for (unsigned i = News.size(); i > 0; --i)
	{
		if (News[i - 1]->type == type)
		{
			num = News[i - 1]->num;
			break;
		}
	}

	NewsItem *news = new NewsItem;
	news->type = type;
	news->num = num + 1;
	news->Text = text;
	news->time = time(NULL);
	strscpy(news->who, u->nick, NICKMAX);

	News.push_back(news);

	return num + 1;
}

static int del_newsitem(unsigned num, NewsType type)
{
	int count = 0;

	for (unsigned i = News.size(); i > 0; --i)
	{
		if (News[i - 1]->type == type && (num == 0 || News[i - 1]->num == num))
		{
			delete News[i - 1];
			News.erase(News.begin() + i - 1);
			++count;
		}
	}

	return count;
}

static int *findmsgs(NewsType type, const char **type_name)
{
	for (unsigned i = 0; i < lenof(msgarray); i++) {
		if (msgarray[i].type == type) {
			if (type_name)
				*type_name = msgarray[i].name;
			return msgarray[i].msgs;
		}
	}
	return NULL;
}

class NewsBase : public Command
{
 protected:
	CommandReturn DoList(User *u, NewsType type, int *msgs)
	{
		int count = 0;
		char timebuf[64];
		struct tm *tm;

		for (unsigned i = 0; i < News.size(); ++i)
		{
			if (News[i]->type == type)
			{
				if (!count)
					notice_lang(s_OperServ, u, msgs[MSG_LIST_HEADER]);
				tm = localtime(&News[i]->time);
				strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);
				notice_lang(s_OperServ, u, msgs[MSG_LIST_ENTRY], News[i]->num, timebuf, *News[i]->who ? News[i]->who : "<unknown>", News[i]->Text.c_str());
				++count;
			}
		}
		if (!count)
			notice_lang(s_OperServ, u, msgs[MSG_LIST_NONE]);
		else
			notice_lang(s_OperServ, u, END_OF_ANY_LIST, "News");

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params, NewsType type, int *msgs)
	{
		const char *text = params.size() > 1 ? params[1].c_str() : NULL;
		int n;

		if (!text)
			this->OnSyntaxError(u, "ADD");
		else
		{
			if (readonly)
			{
				notice_lang(s_OperServ, u, READ_ONLY_MODE);
				return MOD_CONT;
			}
			n = add_newsitem(u, text, type);
			if (n < 0)
				notice_lang(s_OperServ, u, msgs[MSG_ADD_FULL]);
			else
				notice_lang(s_OperServ, u, msgs[MSG_ADDED], n);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params, NewsType type, int *msgs)
	{
		const char *text = params.size() > 1 ? params[1].c_str() : NULL;
		unsigned num;

		if (!text)
			this->OnSyntaxError(u, "DEL");
		else
		{
			if (readonly)
			{
				notice_lang(s_OperServ, u, READ_ONLY_MODE);
				return MOD_CONT;
			}
			if (stricmp(text, "ALL"))
			{
				num = atoi(text);
				if (num > 0 && del_newsitem(num, type))
				{
					notice_lang(s_OperServ, u, msgs[MSG_DELETED], num);
					for (unsigned i = 0; i < News.size(); ++i)
					{
						if (News[i]->type == type && News[i]->num > num)
							--News[i]->num;
					}
				}
				else
					notice_lang(s_OperServ, u, msgs[MSG_DEL_NOT_FOUND], num);
			}
			else
			{
				if (del_newsitem(0, type))
					notice_lang(s_OperServ, u, msgs[MSG_DELETED_ALL]);
				else
					notice_lang(s_OperServ, u, msgs[MSG_DEL_NONE]);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoNews(User *u, const std::vector<ci::string> &params, NewsType type)
	{
		ci::string cmd = params[0];
		const char *type_name;
		int *msgs;

		msgs = findmsgs(type, &type_name);
		if (!msgs)
		{
			alog("news: Invalid type to do_news()");
			return MOD_CONT;
		}

		if (cmd == "LIST")
			return this->DoList(u, type, msgs);
		else if (cmd == "ADD")
			return this->DoAdd(u, params, type, msgs);
		else if (cmd == "DEL")
			return this->DoDel(u, params, type, msgs);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}
 public:
	NewsBase(const std::string &newstype) : Command(newstype, 1, 2, "operserv/news")
	{
	}

	virtual ~NewsBase()
	{
	}

	virtual CommandReturn Execute(User *u, const std::vector<ci::string> &params) = 0;

	virtual bool OnHelp(User *u, const ci::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u, const ci::string &subcommand) = 0;
};

class CommandOSLogonNews : public NewsBase
{
 public:
	CommandOSLogonNews() : NewsBase("LOGONNEWS")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoNews(u, params, NEWS_LOGON);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, NEWS_HELP_LOGON, NewsCount);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_OperServ, u, "LOGONNEWS", NEWS_LOGON_SYNTAX);
	}
};

class CommandOSOperNews : public NewsBase
{
 public:
	CommandOSOperNews() : NewsBase("OPERNEWS")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoNews(u, params, NEWS_OPER);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, NEWS_HELP_OPER, NewsCount);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_OperServ, u, "OPERNEWS", NEWS_OPER_SYNTAX);
	}
};

class CommandOSRandomNews : public NewsBase
{
 public:
	CommandOSRandomNews() : NewsBase("RANDOMNEWS")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoNews(u, params, NEWS_RANDOM);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, NEWS_HELP_RANDOM);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_OperServ, u, "RANDOMNEWS", NEWS_RANDOM_SYNTAX);
	}
};

class OSNews : public Module
{
 public:
	OSNews(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSLogonNews());
		this->AddCommand(OPERSERV, new CommandOSOperNews());
		this->AddCommand(OPERSERV, new CommandOSRandomNews());

		Implementation i[] = { I_OnOperServHelp, I_OnUserModeSet, I_OnUserConnect, I_OnSaveDatabase, I_OnPostLoadDatabases };
		ModuleManager::Attach(i, this, 5);
	}

	~OSNews()
	{
		save_news();
	}

	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_LOGONNEWS);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_OPERNEWS);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RANDOMNEWS);
	}

	void OnUserModeSet(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
		{
			DisplayNews(u, NEWS_OPER);
		}
	}

	void OnUserConnect(User *u)
	{
		DisplayNews(u, NEWS_LOGON);
		DisplayNews(u, NEWS_RANDOM);
	}

	void OnSaveDatabase()
	{
		/* This needs to be destroyed when new dbs are added... */
		save_news();
	}

	void OnPostLoadDatabases()
	{
		/* This needs to be destroyed when new dbs are added... */
		load_news();
	}
};

MODULE_INIT(OSNews)
