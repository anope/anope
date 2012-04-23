/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "global.h"
#include "os_news.h"

/* List of messages for each news type.  This simplifies message sending. */

enum
{
	MSG_SYNTAX,
	MSG_LIST_HEADER,
	MSG_LIST_NONE,
	MSG_ADDED,
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
	  _("Added new logon news item."),
	  _("Logon news item #%s not found!"),
	  _("Logon news item #%d deleted."),
	  _("No logon news items to delete!"),
	  _("All logon news items deleted.")}
	 },
	{NEWS_OPER, "OPER",
	 {_("OPERNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Oper news items:"),
	  _("There is no oper news."),
	  _("Added new oper news item."),
	  _("Oper news item #%s not found!"),
	  _("Oper news item #%d deleted."),
	  _("No oper news items to delete!"),
	  _("All oper news items deleted.")}
	 },
	{NEWS_RANDOM, "RANDOM",
	 {_("RANDOMNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Random news items:"),
	  _("There is no random news."),
	  _("Added new random news item."),
	  _("Random news item #%s not found!"),
	  _("Random news item #%d deleted."),
	  _("No random news items to delete!"),
	  _("All random news items deleted.")}
	 }
};

class MyNewsService : public NewsService
{
	std::vector<NewsItem *> newsItems[3];
 public:
	MyNewsService(Module *m) : NewsService(m) { }

	~MyNewsService()
	{
		for (unsigned i = 0; i < 3; ++i)
			for (unsigned j = 0; j < newsItems[i].size(); ++j)
				newsItems[i][j]->destroy();
	}

	void AddNewsItem(NewsItem *n)
	{
		this->newsItems[n->type].push_back(n);
	}
	
	void DelNewsItem(NewsItem *n)
	{
		std::vector<NewsItem *> &list = this->GetNewsList(n->type);
		std::vector<NewsItem *>::iterator it = std::find(list.begin(), list.end(), n);
		if (it != list.end())
			list.erase(it);
		n->destroy();
	}

	std::vector<NewsItem *> &GetNewsList(NewsType t)
	{
		return this->newsItems[t];
	}
};

#define lenof(a)        (sizeof(a) / sizeof(*(a)))
static const char **findmsgs(NewsType type)
{
	for (unsigned i = 0; i < lenof(msgarray); ++i)
		if (msgarray[i].type == type)
			return msgarray[i].msgs;
	return NULL;
}

class NewsBase : public Command
{
	service_reference<NewsService> ns;

 protected:
	void DoList(CommandSource &source, NewsType ntype, const char **msgs)
	{
		std::vector<NewsItem *> &list = this->ns->GetNewsList(ntype);
		if (list.empty())
			source.Reply(msgs[MSG_LIST_NONE]);
		else
		{
			ListFormatter lflist;
			lflist.addColumn("Number").addColumn("Creator").addColumn("Created").addColumn("Text");

			for (unsigned i = 0, end = list.size(); i < end; ++i)
			{
				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Creator"] = list[i]->who;
				entry["Created"] = do_strftime(list[i]->time);
				entry["Text"] = list[i]->text;
				lflist.addEntry(entry);
			}

			source.Reply(msgs[MSG_LIST_HEADER]);

			std::vector<Anope::string> replies;
			lflist.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("End of \2news\2 list."));
		}

		return;
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype, const char **msgs)
	{
		const Anope::string text = params.size() > 1 ? params[1] : "";

		if (text.empty())
			this->OnSyntaxError(source, "ADD");
		else
		{
			if (readonly)
				source.Reply(READ_ONLY_MODE);

			NewsItem *news = new NewsItem();
			news->type = ntype;
			news->text = text;
			news->time = Anope::CurTime;
			news->who = source.u->nick;

			this->ns->AddNewsItem(news);

			source.Reply(msgs[MSG_ADDED]);
		}

		return;
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype, const char **msgs)
	{
		const Anope::string &text = params.size() > 1 ? params[1] : "";

		if (text.empty())
			this->OnSyntaxError(source, "DEL");
		else
		{
			std::vector<NewsItem *> &list = this->ns->GetNewsList(ntype);
			if (list.empty())
				source.Reply(msgs[MSG_LIST_NONE]);
			else
			{
				if (readonly)
					source.Reply(READ_ONLY_MODE);
				if (!text.equals_ci("ALL"))
				{
					try
					{
						unsigned num = convertTo<unsigned>(text);
						if (num > 0 && num <= list.size())
						{
							this->ns->DelNewsItem(list[num - 1]);
							source.Reply(msgs[MSG_DELETED], num);
							return;
						}
					}
					catch (const ConvertException &) { }

					source.Reply(msgs[MSG_DEL_NOT_FOUND], text.c_str());
				}
				else
				{
					for (unsigned i = list.size(); i > 0; --i)
						this->ns->DelNewsItem(list[i - 1]);
					source.Reply(msgs[MSG_DELETED_ALL]);
				}
			}
		}

		return;
	}

	void DoNews(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype)
	{
		if (!this->ns)
			return;

		const Anope::string &cmd = params[0];

		const char **msgs = findmsgs(ntype);
		if (!msgs)
			throw CoreException("news: Invalid type to do_news()");

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, ntype, msgs);
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params, ntype, msgs);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params, ntype, msgs);
		else
			this->OnSyntaxError(source, "");

		return;
	}
 public:
	NewsBase(Module *creator, const Anope::string &newstype) : Command(creator, newstype, 1, 2), ns("NewsService", "news")
	{
		this->SetSyntax(_("ADD \037text\037"));
		this->SetSyntax(_("DEL {\037num\037 | ALL}"));
		this->SetSyntax(_("LIST"));
	}

	virtual ~NewsBase()
	{
	}

	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) = 0;
};

class CommandOSLogonNews : public NewsBase
{
 public:
	CommandOSLogonNews(Module *creator) : NewsBase(creator, "operserv/logonnews")
	{
		this->SetDesc(_("Define messages to be shown to users at logon"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoNews(source, params, NEWS_LOGON);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Edits or displays the list of logon news messages.  When a\n"
			"user connects to the network, these messages will be sent\n"
			"to them.  (However, no more than \002%d\002 messages will be\n"
			"sent in order to avoid flooding the user.  If there are\n"
			"more news messages, only the most recent will be sent.)\n"
			"NewsCount can be configured in services.conf.\n"
			" \n"
			"LOGONNEWS may only be used by Services Operators."), Config->NewsCount);
		return true;
	}
};

class CommandOSOperNews : public NewsBase
{
 public:
	CommandOSOperNews(Module *creator) : NewsBase(creator, "operserv/opernews")
	{
		this->SetDesc(_("Define messages to be shown to users who oper"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoNews(source, params, NEWS_OPER);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Edits or displays the list of oper news messages.  When a\n"
				"user opers up (with the /OPER command), these messages will\n"
				"be sent to them.  (However, no more than \002%d\002 messages will\n"
				"be sent in order to avoid flooding the user.  If there are\n"
				"more news messages, only the most recent will be sent.)\n"
				"NewsCount can be configured in services.conf.\n"
				" \n"
				"OPERNEWS may only be used by Services Operators."), Config->NewsCount);
		return true;
	}
};

class CommandOSRandomNews : public NewsBase
{
 public:
	CommandOSRandomNews(Module *creator) : NewsBase(creator, "operserv/randomnews")
	{
		this->SetDesc(_("Define messages to be randomly shown to users at logon"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return this->DoNews(source, params, NEWS_RANDOM);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Edits or displays the list of random news messages.  When a\n"
				"user connects to the network, one (and only one) of the\n"
				"random news will be randomly chosen and sent to them.\n"
				" \n"
				"RANDOMNEWS may only be used by Services Operators."));
		return true;
	}
};

class OSNews : public Module
{
	SerializeType newsitem_type;
	MyNewsService newsservice;

	CommandOSLogonNews commandoslogonnews;
	CommandOSOperNews commandosopernews;
	CommandOSRandomNews commandosrandomnews;

	void DisplayNews(User *u, NewsType Type)
	{
		std::vector<NewsItem *> &newsList = this->newsservice.GetNewsList(Type);
		if (newsList.empty())
			return;

		Anope::string msg;
		if (Type == NEWS_LOGON)
			msg = _("[\002Logon News\002 - %s] %s");
		else if (Type == NEWS_OPER)
			msg = _("[\002Oper News\002 - %s] %s");
		else if (Type == NEWS_RANDOM)
			msg = _("[\002Random News\002 - %s] %s");

		static unsigned cur_rand_news = 0;
		unsigned displayed = 0;
		for (unsigned i = 0, end = newsList.size(); i < end; ++i)
		{
			if (Type == NEWS_RANDOM && i != cur_rand_news)
				continue;

			const BotInfo *gl = findbot(Config->Global);
			if (!gl && !BotListByNick->empty())
				gl = BotListByNick->begin()->second;
			const BotInfo *os = findbot(Config->OperServ);
			if (!os)
				os = gl;
			if (gl)
				u->SendMessage(Type != NEWS_OPER ? gl : os, msg.c_str(), do_strftime(newsList[i]->time).c_str(), newsList[i]->text.c_str());

			++displayed;

			if (Type == NEWS_RANDOM)
			{
				++cur_rand_news;
				break;
			}
			else if (displayed >= Config->NewsCount)
				break;
		}

		/* Reset to head of list to get first random news value */
		if (Type == NEWS_RANDOM && cur_rand_news >= newsList.size())
			cur_rand_news = 0;
	}

 public:
	OSNews(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		newsitem_type("NewsItem", NewsItem::unserialize), newsservice(this), commandoslogonnews(this), commandosopernews(this), commandosrandomnews(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnUserModeSet, I_OnUserConnect };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnUserModeSet(User *u, UserModeName Name) anope_override
	{
		if (Name == UMODE_OPER)
			DisplayNews(u, NEWS_OPER);
	}

	void OnUserConnect(dynamic_reference<User> &user, bool &) anope_override
	{
		if (!user || !user->server->IsSynced())
			return;

		DisplayNews(user, NEWS_LOGON);
		DisplayNews(user, NEWS_RANDOM);
	}
};

MODULE_INIT(OSNews)
