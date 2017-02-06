/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/operserv/news.h"

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
	MSG_DELETED_ALL,
	MSG_SIZE
};

static struct NewsMessages
{
	NewsType type;
	Anope::string name;
	const char *msgs[MSG_SIZE];
} msgarray[] = {
	{NEWS_LOGON, "LOGON",
	 {_("LOGONNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Logon news items:"),
	  _("There is no logon news."),
	  _("Added new logon news item."),
	  _("Logon news item #{0} not found!"),
	  _("Logon news item #{0} deleted."),
	  _("No logon news items to delete!"),
	  _("All logon news items deleted.")}
	 },
	{NEWS_OPER, "OPER",
	 {_("OPERNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Oper news items:"),
	  _("There is no oper news."),
	  _("Added new oper news item."),
	  _("Oper news item #{0} not found!"),
	  _("Oper news item #{0} deleted."),
	  _("No oper news items to delete!"),
	  _("All oper news items deleted.")}
	 },
	{NEWS_RANDOM, "RANDOM",
	 {_("RANDOMNEWS {ADD|DEL|LIST} [\037text\037|\037num\037]\002"),
	  _("Random news items:"),
	  _("There is no random news."),
	  _("Added new random news item."),
	  _("Random news item #{0} not found!"),
	  _("Random news item #{0} deleted."),
	  _("No random news items to delete!"),
	  _("All random news items deleted.")}
	 }
};

class NewsItemImpl : public NewsItem
{
	friend class NewsItemType;

	Serialize::Storage<NewsType> type;
	Serialize::Storage<Anope::string> text, who;
	Serialize::Storage<time_t> time;

 public:
	using NewsItem::NewsItem;

	NewsType GetNewsType() override;
	void SetNewsType(NewsType) override;

	Anope::string GetText() override;
	void SetText(const Anope::string &) override;

	Anope::string GetWho() override;
	void SetWho(const Anope::string &) override;

	time_t GetTime() override;
	void SetTime(const time_t &) override;
};

class NewsItemType : public Serialize::Type<NewsItemImpl>
{
 public:
	Serialize::Field<NewsItemImpl, NewsType> type;
	Serialize::Field<NewsItemImpl, Anope::string> text;
	Serialize::Field<NewsItemImpl, Anope::string> who;
	Serialize::Field<NewsItemImpl, time_t> time;

	NewsItemType(Module *me) : Serialize::Type<NewsItemImpl>(me)
		, type(this, "type", &NewsItemImpl::type)
		, text(this, "text", &NewsItemImpl::text)
		, who(this, "who", &NewsItemImpl::who)
		, time(this, "time", &NewsItemImpl::time)
	{
	}
};

NewsType NewsItemImpl::GetNewsType()
{
	return Get(&NewsItemType::type);
}

void NewsItemImpl::SetNewsType(NewsType t)
{
	Set(&NewsItemType::type, t);
}

Anope::string NewsItemImpl::GetText()
{
	return Get(&NewsItemType::text);
}

void NewsItemImpl::SetText(const Anope::string &t)
{
	Set(&NewsItemType::text, t);
}

Anope::string NewsItemImpl::GetWho()
{
	return Get(&NewsItemType::who);
}

void NewsItemImpl::SetWho(const Anope::string &w)
{
	Set(&NewsItemType::who, w);
}

time_t NewsItemImpl::GetTime()
{
	return Get(&NewsItemType::time);
}

void NewsItemImpl::SetTime(const time_t &t)
{
	Set(&NewsItemType::time, t);
}

static const char **findmsgs(NewsType type)
{
	for (unsigned i = 0; i < sizeof(msgarray) / sizeof(*msgarray); ++i)
		if (msgarray[i].type == type)
			return msgarray[i].msgs;
	return NULL;
}

class NewsBase : public Command
{
 protected:
	void DoList(CommandSource &source, NewsType ntype, const char **msgs)
	{
		std::vector<NewsItem *> list = Serialize::GetObjects<NewsItem *>();

		if (list.empty())
		{
			source.Reply(msgs[MSG_LIST_NONE]);
			return;
		}

		ListFormatter lflist(source.GetAccount());
		lflist.AddColumn(_("Number")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Text"));

		unsigned int i = 1;
		for (NewsItem *n : list)
		{
			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(i++ + 1);
			entry["Creator"] = n->GetWho();
			entry["Created"] = Anope::strftime(n->GetTime(), NULL, true);
			entry["Text"] = n->GetText();
			lflist.AddEntry(entry);
		}

		source.Reply(msgs[MSG_LIST_HEADER]);

		std::vector<Anope::string> replies;
		lflist.Process(replies);

		for (i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of news list."));
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype, const char **msgs)
	{
		const Anope::string text = params.size() > 1 ? params[1] : "";

		if (text.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		NewsItem *ni = Serialize::New<NewsItem *>();
		ni->SetNewsType(ntype);
		ni->SetText(text);
		ni->SetTime(Anope::CurTime);
		ni->SetWho(source.GetNick());

		source.Reply(msgs[MSG_ADDED]);

		logger.Admin(source, _("{source} used {command} to add news item: {0}"), text);
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype, const char **msgs)
	{
		const Anope::string &text = params.size() > 1 ? params[1] : "";
		std::vector<NewsItem *> list = Serialize::GetObjects<NewsItem *>();

		if (text.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (list.empty())
		{
			source.Reply(msgs[MSG_LIST_NONE]);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		if (!text.equals_ci("ALL"))
		{
			try
			{
				unsigned num = convertTo<unsigned>(text);
				if (num > 0 && num <= list.size())
				{
					NewsItem *item = list[num - 1];
					source.Reply(msgs[MSG_DELETED], num);

					logger.Admin(source, _("{source} used {command} to delete news item {0}"), item->GetText());

					item->Delete();

					return;
				}
			}
			catch (const ConvertException &) { }

			source.Reply(msgs[MSG_DEL_NOT_FOUND], text.c_str());
		}
		else
		{
			for (NewsItem *n : list)
				n->Delete();
			source.Reply(msgs[MSG_DELETED_ALL]);

			logger.Admin(source, _("{source} used {command} to delete all news items"));
		}
	}

	void DoNews(CommandSource &source, const std::vector<Anope::string> &params, NewsType ntype)
	{
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
	}
 public:
	NewsBase(Module *creator, const Anope::string &newstype) : Command(creator, newstype, 1, 2)
	{
		this->SetSyntax(_("ADD \037text\037"));
		this->SetSyntax(_("DEL {\037num\037 | ALL}"));
		this->SetSyntax("LIST");
	}

	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_abstract;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_abstract;
};

class CommandOSLogonNews : public NewsBase
{
 public:
	CommandOSLogonNews(Module *creator) : NewsBase(creator, "operserv/logonnews")
	{
		this->SetDesc(_("Define messages to be shown to users at logon"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, NEWS_LOGON);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Edits or displays the list of logon news messages.  When a user connects to the network, these messages will be sent to them."
		               " However, no more than \002{0}\002 messages will be sent in order to avoid flooding the user."
		               " If there are more news messages, only the most recent will be sent."),
		               Config->GetModule(this->GetOwner())->Get<unsigned>("newscount", "3"));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, NEWS_OPER);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Edits or displays the list of oper news messages."
		               " When a user opers up (with the /OPER command), these messages will be sent to them."
		               " However, no more than \002{0}\002 messages will be sent in order to avoid flooding the user."
		               " If there are more news messages, only the most recent will be sent."),
		               Config->GetModule(this->GetOwner())->Get<unsigned>("newscount", "3"));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, NEWS_RANDOM);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Edits or displays the list of random news messages.  When a user connects to the network, one (and only one) of the random news will be randomly chosen and sent to them."));
		return true;
	}
};

class OSNews : public Module
	, public EventHook<Event::UserModeSet>
	, public EventHook<Event::UserConnect>
{
	NewsItemType newsitemtype;

	CommandOSLogonNews commandoslogonnews;
	CommandOSOperNews commandosopernews;
	CommandOSRandomNews commandosrandomnews;

	Anope::string oper_announcer, announcer;
	unsigned int news_count;
	unsigned int cur_rand_news = 0;

	void DisplayNews(User *u, NewsType Type)
	{
		std::vector<NewsItem *> list = Serialize::GetObjects<NewsItem *>();
		if (list.empty())
			return;

		ServiceBot *bi = NULL;
		if (Type == NEWS_OPER)
			bi = ServiceBot::Find(Config->GetModule(this)->Get<Anope::string>("oper_announcer", "OperServ"), true);
		else
			bi = ServiceBot::Find(Config->GetModule(this)->Get<Anope::string>("announcer", "Global"), true);
		if (bi == NULL)
			return;

		Anope::string msg;
		if (Type == NEWS_LOGON)
			msg = _("[\002Logon News\002 - {0}] {1}");
		else if (Type == NEWS_OPER)
			msg = _("[\002Oper News\002 - {0}] {1}");
		else if (Type == NEWS_RANDOM)
			msg = _("[\002Random News\002 - {0}] {1}");
		else
			return;

		unsigned int start;
		unsigned int end;

		if (Type == NEWS_RANDOM)
		{
			/* Reset to head of list to get first random news value */
			if (cur_rand_news >= list.size())
				cur_rand_news = 0;

			/* only show one news entry */
			start = cur_rand_news;
			end = cur_rand_news + 1;

			++cur_rand_news;
		}
		else
		{
			if (list.size() < news_count)
				start = 0;
			else
				start = list.size() - news_count;

			end = start + news_count;

			if (end > list.size())
				end = list.size();
		}

		for (unsigned int i = start; i < end; ++i)
		{
			NewsItem *n = list[i];

			u->SendMessage(bi, msg.c_str(), Anope::strftime(n->GetTime(), u->Account(), true), n->GetText());
		}
	}

 public:
	OSNews(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserModeSet>(this)
		, EventHook<Event::UserConnect>(this)
		, newsitemtype(this)
		, commandoslogonnews(this)
		, commandosopernews(this)
		, commandosrandomnews(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		oper_announcer = conf->GetModule(this)->Get<Anope::string>("oper_announcer", "OperServ");
		announcer = conf->GetModule(this)->Get<Anope::string>("announcer", "Global");
		news_count = conf->GetModule(this)->Get<unsigned>("newscount", "3");
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (mname == "OPER")
			DisplayNews(u, NEWS_OPER);
	}

	void OnUserConnect(User *user, bool &) override
	{
		if (user->Quitting() || !user->server->IsSynced())
			return;

		DisplayNews(user, NEWS_LOGON);
		DisplayNews(user, NEWS_RANDOM);
	}
};

MODULE_INIT(OSNews)
