/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

#define lenof(a)        (sizeof(a) / sizeof(*(a)))

/* List of messages for each news type.  This simplifies message sending. */

enum
{
	MSG_SYNTAX,
	MSG_LIST_HEADER,
	MSG_LIST_NONE,
	MSG_ADD_SYNTAX,
	MSG_ADDED,
	MSG_DEL_SYNTAX,
	MSG_DEL_NOT_FOUND,
	MSG_DELETED,
	MSG_DEL_NONE,
	MSG_DELETED_ALL
};

struct NewsMessages msgarray[] = {
	{NEWS_LOGON, "LOGON",
	 {_("LOGONNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Logon news items:"),
	  _("There is no logon news."),
	  _("Syntax: \002LOGONNEWS ADD \037text\037\002"),
	  _("Added new logon news item (#%d)."),
	  _("Syntax: \002LOGONNEWS DEL {\037num\037 | ALL}"),
	  _("Logon news item #%s not found!"),
	  _("LOGONNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("No logon news items to delete!"),
	  _("All logon news items deleted.")}
	 },
	{NEWS_OPER, "OPER",
	 {_("OPERNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Oper news items:"),
	  _("There is no oper news."),
	  _("Syntax: \002OPERNEWS ADD \037text\037\002"),
	  _("Added new oper news item (#%d)."),
	  _("Syntax: \002OPERNEWS DEL {\037num\037 | ALL}"),
	  _("Oper news item #%s not found!"),
	  _("Oper news item #%d deleted."),
	  _("No oper news items to delete!"),
	  _("All oper news items deleted.")}
	 },
	{NEWS_RANDOM, "RANDOM",
	 {_("RANDOMNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Random news items:"),
	  _("There is no random news."),
	  _("Syntax: \002RANDOMNEWS ADD \037text\037\002"),
	  _("Added new random news item (#%d)."),
	  _("Syntax: \002RANDOMNEWS DEL {\037num\037 | ALL}"),
	  _("Random news item #%s not found!"),
	  _("Random news item #%d deleted."),
	  _("No random news items to delete!"),
	  _("All random news items deleted.")}
	 }
};

static void DisplayNews(User *u, NewsType Type)
{
	static unsigned current_news = 0;
	Anope::string msg;

	if (Type == NEWS_LOGON)
		msg = _("[\002Logon News\002 - %s] %s");
	else if (Type == NEWS_OPER)
		msg = _("[\002Oper News\002 - %s] %s");
	else if (Type == NEWS_RANDOM)
		msg = _("[\002Random News\002 - %s] %s");
	else
		throw CoreException("news: Invalid type (" + stringify(Type) + ") to display_news()");

	unsigned displayed = 0;
	bool NewsExists = false;
	for (unsigned i = 0, end = News.size(); i < end; ++i)
	{
		if (News[i]->type == Type)
		{
			NewsExists = true;

			if (Type == NEWS_RANDOM && i == current_news)
				continue;

			u->SendMessage(Type != NEWS_OPER && Global ? Global : OperServ, msg.c_str(), do_strftime(News[i]->time).c_str(), News[i]->Text.c_str());

			++displayed;

			if (Type == NEWS_RANDOM)
			{
				current_news = i;
				return;
			}
			else if (displayed >= Config->NewsCount)
				return;
		}

		/* Reset to head of list to get first random news value */
		if (i + 1 == News.size() && Type == NEWS_RANDOM && NewsExists)
			i = 0;
	}
}

static int add_newsitem(CommandSource &source, const Anope::string &text, NewsType type)
{
	int num = 0;

	for (unsigned i = News.size(); i > 0; --i)
		if (News[i - 1]->type == type)
		{
			num = News[i - 1]->num;
			break;
		}

	NewsItem *news = new NewsItem();
	news->type = type;
	news->num = num + 1;
	news->Text = text;
	news->time = Anope::CurTime;
	news->who = source.u->nick;

	News.push_back(news);

	return num + 1;
}

static int del_newsitem(unsigned num, NewsType type)
{
	int count = 0;

	for (unsigned i = News.size(); i > 0; --i)
		if (News[i - 1]->type == type && (num == 0 || News[i - 1]->num == num))
		{
			delete News[i - 1];
			News.erase(News.begin() + i - 1);
			++count;
		}

	return count;
}

static const char **findmsgs(NewsType type, Anope::string &type_name)
{
	for (unsigned i = 0; i < lenof(msgarray); ++i)
		if (msgarray[i].type == type)
		{
			type_name = msgarray[i].name;
			return msgarray[i].msgs;
		}
	return NULL;
}

class NewsBase : public Command
{
 protected:
	CommandReturn DoList(CommandSource &source, NewsType type, const char **msgs)
	{
		int count = 0;

		for (unsigned i = 0, end = News.size(); i < end; ++i)
			if (News[i]->type == type)
			{
				if (!count)
					source.Reply(msgs[MSG_LIST_HEADER]);
				source.Reply(_("%5d (%s by %s)\n""    %s"), News[i]->num, do_strftime(News[i]->time).c_str(), !News[i]->who.empty() ? News[i]->who.c_str() : "<unknown>", News[i]->Text.c_str());
				++count;
			}
		if (!count)
			source.Reply(msgs[MSG_LIST_NONE]);
		else
			source.Reply(_(END_OF_ANY_LIST), "News");

		return MOD_CONT;
	}

	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params, NewsType type, const char **msgs)
	{
		const Anope::string text = params.size() > 1 ? params[1] : "";
		int n;

		if (text.empty())
			this->OnSyntaxError(source, "ADD");
		else
		{
			if (readonly)
			{
				source.Reply(_(READ_ONLY_MODE));
				return MOD_CONT;
			}
			n = add_newsitem(source, text, type);
			if (n < 0)
				source.Reply(_("News list is full!"));
			else
				source.Reply(msgs[MSG_ADDED], n);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params, NewsType type, const char **msgs)
	{
		const Anope::string &text = params.size() > 1 ? params[1] : "";

		if (text.empty())
			this->OnSyntaxError(source, "DEL");
		else
		{
			if (readonly)
			{
				source.Reply(_(READ_ONLY_MODE));
				return MOD_CONT;
			}
			if (!text.equals_ci("ALL"))
			{
				try
				{
					unsigned num = convertTo<unsigned>(text);
					if (num > 0 && del_newsitem(num, type))
					{
						source.Reply(msgs[MSG_DELETED], num);
						for (unsigned i = 0, end = News.size(); i < end; ++i)
							if (News[i]->type == type && News[i]->num > num)
								--News[i]->num;
						return MOD_CONT;
					}
				}
				catch (const ConvertException &) { }

				source.Reply(msgs[MSG_DEL_NOT_FOUND], text.c_str());
			}
			else
			{
				if (del_newsitem(0, type))
					source.Reply(msgs[MSG_DELETED_ALL]);
				else
					source.Reply(msgs[MSG_DEL_NONE]);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoNews(CommandSource &source, const std::vector<Anope::string> &params, NewsType type)
	{
		const Anope::string &cmd = params[0];
		Anope::string type_name;

		const char **msgs = findmsgs(type, type_name);
		if (!msgs)
			throw CoreException("news: Invalid type to do_news()");

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, type, msgs);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params, type, msgs);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params, type, msgs);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}
 public:
	NewsBase(const Anope::string &newstype) : Command(newstype, 1, 2, "operserv/news")
	{
	}

	virtual ~NewsBase()
	{
	}

	virtual CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) = 0;

	virtual void OnSyntaxError(CommandSource &source, const Anope::string &subcommand) = 0;
};

class CommandOSLogonNews : public NewsBase
{
 public:
	CommandOSLogonNews() : NewsBase("LOGONNEWS")
	{
		this->SetDesc(_("Define messages to be shown to users at logon"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoNews(source, params, NEWS_LOGON);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002LOGONNEWS ADD \037text\037\002\n"
			"        \002LOGONNEWS DEL {\037num\037 | ALL}\002\n"
			"        \002LOGONNEWS LIST\002\n"
			" \n"
			"Edits or displays the list of logon news messages.  When a\n"
			"user connects to the network, these messages will be sent\n"
			"to them.  (However, no more than \002%d\002 messages will be\n"
			"sent in order to avoid flooding the user.  If there are\n"
			"more news messages, only the most recent will be sent.)\n"
			"NewsCount can be configured in services.conf.\n"
			" \n"
			"LOGONNEWS may only be used by Services Operators."), Config->NewsCount);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LOGONNEWS", _("LOGONNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"));
	}
};

class CommandOSOperNews : public NewsBase
{
 public:
	CommandOSOperNews() : NewsBase("OPERNEWS")
	{
		this->SetDesc(_("Define messages to be shown to users who oper"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoNews(source, params, NEWS_OPER);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002OPERNEWS ADD \037text\037\002\n"
				"        \002OPERNEWS DEL {\037num\037 | ALL}\002\n"
				"        \002OPERNEWS LIST\002\n"
				" \n"
				"Edits or displays the list of oper news messages.  When a\n"
				"user opers up (with the /OPER command), these messages will\n"
				"be sent to them.  (However, no more than \002%d\002 messages will\n"
				"be sent in order to avoid flooding the user.  If there are\n"
				"more news messages, only the most recent will be sent.)\n"
				"NewsCount can be configured in services.conf.\n"
				" \n"
				"OPERNEWS may only be used by Services Operators."), Config->NewsCount);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "OPERNEWS", _("OPERNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"));
	}
};

class CommandOSRandomNews : public NewsBase
{
 public:
	CommandOSRandomNews() : NewsBase("RANDOMNEWS")
	{
		this->SetDesc(_("Define messages to be randomly shown to users at logon"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoNews(source, params, NEWS_RANDOM);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002RANDOMNEWS ADD \037text\037\002\n"
				"        \002RANDOMNEWS DEL {\037num\037 | ALL}\002\n"
				"        \002RANDOMNEWS LIST\002\n"
				" \n"
				"Edits or displays the list of random news messages.  When a\n"
				"user connects to the network, one (and only one) of the\n"
				"random news will be randomly chosen and sent to them.\n"
				" \n"
				"RANDOMNEWS may only be used by Services Operators."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "RANDOMNEWS", _("RANDOMNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"));
	}
};

class OSNews : public Module
{
	CommandOSLogonNews commandoslogonnews;
	CommandOSOperNews commandosopernews;
	CommandOSRandomNews commandosrandomnews;

 public:
	OSNews(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandoslogonnews);
		this->AddCommand(OperServ, &commandosopernews);
		this->AddCommand(OperServ, &commandosrandomnews);

		Implementation i[] = { I_OnUserModeSet, I_OnUserConnect, I_OnDatabaseRead, I_OnDatabaseWrite };
		ModuleManager::Attach(i, this, 4);
	}

	~OSNews()
	{
		for (std::vector<NewsItem *>::iterator it = News.begin(), it_end = News.end(); it != it_end; ++it)
			delete *it;
		News.clear();
	}

	void OnUserModeSet(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
			DisplayNews(u, NEWS_OPER);
	}

	void OnUserConnect(User *u)
	{
		DisplayNews(u, NEWS_LOGON);
		DisplayNews(u, NEWS_RANDOM);
	}

	EventReturn OnDatabaseRead(const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("OS") && params.size() >= 7 && params[1].equals_ci("NEWS"))
		{
			NewsItem *n = new NewsItem();
			n->num = params[2].is_pos_number_only() ? convertTo<unsigned>(params[2]) : 0;
			n->time = params[3].is_number_only() ? convertTo<time_t>(params[3]) : 0;
			n->who = params[4];
			if (params[5].equals_ci("LOGON"))
				n->type = NEWS_LOGON;
			else if (params[5].equals_ci("RANDOM"))
				n->type = NEWS_RANDOM;
			else if (params[5].equals_ci("OPER"))
				n->type = NEWS_OPER;
			n->Text = params[6];
			News.push_back(n);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const Anope::string &))
	{
		for (std::vector<NewsItem *>::iterator it = News.begin(); it != News.end(); ++it)
		{
			NewsItem *n = *it;
			Anope::string ntype;
			if (n->type == NEWS_LOGON)
				ntype = "LOGON";
			else if (n->type == NEWS_RANDOM)
				ntype = "RANDOM";
			else if (n->type == NEWS_OPER)
				ntype = "OPER";
			Anope::string buf = "OS NEWS " + stringify(n->num) + " " + stringify(n->time) + " " + n->who + " " + ntype + " :" + n->Text;
			Write(buf);
		}
	}
};

MODULE_INIT(OSNews)
