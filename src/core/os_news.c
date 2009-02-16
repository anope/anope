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


void myOperServHelp(User *u);

struct newsmsgs {
	int16 type;
	const char *name;
	int msgs[MSG_MAX + 1];
};

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

static int *findmsgs(int16 type, const char **type_name)
{
	int i;
	for (i = 0; i < lenof(msgarray); i++) {
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
	CommandReturn DoList(User *u, std::vector<std::string> &params, short type, int *msgs)
	{
		int i, count = 0;
		char timebuf[64];
		struct tm *tm;

		for (i = 0; i < nnews; ++i)
		{
			if (news[i].type == type)
			{
				if (!count)
					notice_lang(s_OperServ, u, msgs[MSG_LIST_HEADER]);
				tm = localtime(&news[i].time);
				strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, tm);
				notice_lang(s_OperServ, u, msgs[MSG_LIST_ENTRY], news[i].num, timebuf, *news[i].who ? news[i].who : "<unknown>", news[i].text);
				++count;
			}
		}
		if (!count)
			notice_lang(s_OperServ, u, msgs[MSG_LIST_NONE]);
		else
			notice_lang(s_OperServ, u, END_OF_ANY_LIST, "News");

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, std::vector<std::string> &params, short type, int *msgs)
	{
		const char *text = params.size() > 1 ? params[1].c_str() : NULL;
		int n;

		if (!text)
			this->OnSyntaxError(u);
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

	CommandReturn DoDel(User *u, std::vector<std::string> &params, short type, int *msgs)
	{
		const char *text = params.size() > 1 ? params[1].c_str() : NULL;
		int i, num;

		if (!text)
			this->OnSyntaxError(u);
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
					/* Reset the order - #0000397 */
					for (i = 0; i < nnews; ++i)
					{
						if (news[i].type == type && news[i].num > num)
							--news[i].num;
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

	CommandReturn DoNews(User *u, std::vector<std::string> &params, short type)
	{
		int is_servadmin = is_services_admin(u);
		const char *cmd = params[0].c_str();
		const char *type_name;
		int *msgs;

		msgs = findmsgs(type, &type_name);
		if (!msgs)
		{
			alog("news: Invalid type to do_news()");
			return MOD_CONT;
		}

		if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params, type, msgs);
		else if (!stricmp(cmd, "ADD"))
		{
			if (is_servadmin)
				return this->DoAdd(u, params, type, msgs);
			else
				notice_lang(s_OperServ, u, PERMISSION_DENIED);
		}
		else if (!stricmp(cmd, "DEL"))
		{
			if (is_servadmin)
				return this->DoDel(u, params, type, msgs);
			else
				notice_lang(s_OperServ, u, PERMISSION_DENIED);
		}
		else
			this->OnSyntaxError(u);

		return MOD_CONT;
	}
 public:
	NewsBase(const std::string &newstype) : Command(newstype, 1, 2)
	{
	}

	virtual ~NewsBase()
	{
	}

	virtual CommandReturn Execute(User *u, std::vector<std::string> &params) = 0;

	virtual bool OnHelp(User *u, const std::string &subcommand) = 0;

	virtual void OnSyntaxError(User *u) = 0;
};

class CommandOSLogonNews : public NewsBase
{
 public:
	CommandOSLogonNews() : NewsBase("LOGONNEWS")
	{
		this->help_param1 = NULL;

		this->UpdateHelpParam();
	}

	~CommandOSLogonNews()
	{
		delete [] this->help_param1;
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoNews(u, params, NEWS_LOGON);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_admin(u))
			return false;

		notice_help(s_OperServ, u, NEWS_HELP_LOGON, this->help_param1);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "LOGONNEWS", NEWS_LOGON_SYNTAX);
	}

	void UpdateHelpParam()
	{
		if (this->help_param1)
			delete [] this->help_param1;

		char buf[BUFSIZE];

		snprintf(buf, BUFSIZE, "%d", NewsCount),
		this->help_param1 = sstrdup(buf);
	}
} *OSLogonNews = NULL;

class CommandOSOperNews : public NewsBase
{
 public:
	CommandOSOperNews() : NewsBase("OPERNEWS")
	{
		this->help_param1 = NULL;

		this->UpdateHelpParam();
	}

	~CommandOSOperNews()
	{
		delete [] this->help_param1;
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoNews(u, params, NEWS_OPER);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_admin(u))
			return false;

		notice_help(s_OperServ, u, NEWS_HELP_OPER, this->help_param1);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "OPERNEWS", NEWS_OPER_SYNTAX);
	}

	void UpdateHelpParam()
	{
		if (this->help_param1)
			delete [] this->help_param1;

		char buf[BUFSIZE];

		snprintf(buf, BUFSIZE, "%d", NewsCount),
		this->help_param1 = sstrdup(buf);
	}
} *OSOperNews = NULL;

class CommandOSRandomNews : public NewsBase
{
 public:
	CommandOSRandomNews() : NewsBase("RANDOMNEWS")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return this->DoNews(u, params, NEWS_RANDOM);
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_admin(u))
			return false;

		notice_help(s_OperServ, u, NEWS_HELP_RANDOM);
		return true;
	}

	void OnSyntaxError(User *u)
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

		OSLogonNews = new CommandOSLogonNews();
		this->AddCommand(OPERSERV, OSLogonNews, MOD_UNIQUE);
		OSOperNews = new CommandOSOperNews();
		this->AddCommand(OPERSERV, OSOperNews, MOD_UNIQUE);
		this->AddCommand(OPERSERV, new CommandOSRandomNews(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}

	void reload_config(bool starting)
	{
		OSLogonNews->UpdateHelpParam();
		OSOperNews->UpdateHelpParam();
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_admin(u))
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_LOGONNEWS);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_OPERNEWS);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RANDOMNEWS);
	}
}


MODULE_INIT("os_news", OSNews)
