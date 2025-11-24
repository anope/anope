// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/operserv/news.h"

// TODO: msgarray breaks the format string checking
#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wformat-security"
#endif

namespace
{
	Anope::string TypeToString(OperServ::NewsType nt)
	{
		switch (nt)
		{
			case OperServ::NEWS_LOGON:
				return "LOGON";
			case OperServ::NEWS_RANDOM:
				return "RANDOM";
			case OperServ::NEWS_OPER:
				return "OPER";
		}
		return ""; // Should never happen.
	}

	OperServ::NewsType StringToType(const Anope::string &nt)
	{
		if (nt.equals_ci("LOGON") || nt.equals_ci("0"))
			return OperServ::NEWS_LOGON;
		if (nt.equals_ci("RANDOM") || nt.equals_ci("1"))
			return OperServ::NEWS_RANDOM;
		if (nt.equals_ci("OPER") || nt.equals_ci("2"))
			return OperServ::NEWS_OPER;

		return OperServ::NEWS_LOGON; // Should never happen.
	}
}

enum
{
	MSG_NEWS_SHORT,
	MSG_NEWS_LONG,
	MSG_LIST_HEADER,
	MSG_LIST_NONE,
	MSG_ADDED,
	MSG_DEL_NOT_FOUND,
	MSG_DELETED,
	MSG_DELETED_ALL,
	MSG_END,
};

struct NewsMessages final
{
	OperServ::NewsType type;
	Anope::string name;
	const char *msgs[MSG_END];
};

struct NewsMessages msgarray[] = {
	{OperServ::NEWS_LOGON, "LOGON",
	 {_("[\002Logon News\002] %s"),
	  _("[\002Logon News\002 - %s] %s"),
	  _("Logon news items:"),
	  _("There is no logon news."),
	  _("Added new logon news item."),
	  _("Logon news item #%s not found!"),
	  _("Logon news item #%u deleted."),
	  _("All logon news items deleted.")}
	 },
	{OperServ::NEWS_OPER, "OPER",
	 {_("[\002Oper News\002] %s"),
	  _("[\002Oper News\002 - %s] %s"),
	  _("Oper news items:"),
	  _("There is no oper news."),
	  _("Added new oper news item."),
	  _("Oper news item #%s not found!"),
	  _("Oper news item #%u deleted."),
	  _("All oper news items deleted.")}
	 },
	{OperServ::NEWS_RANDOM, "RANDOM",
	 {_("[\002Random News\002] %s"),
	  _("[\002Random News\002 - %s] %s"),
	  _("Random news items:"),
	  _("There is no random news."),
	  _("Added new random news item."),
	  _("Random news item #%s not found!"),
	  _("Random news item #%u deleted."),
	  _("All random news items deleted.")}
	 }
};

struct NewsItemType final
	: Serialize::Type
{
	NewsItemType()
		: Serialize::Type(OPERSERV_NEWS_ITEM_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *ni = static_cast<const OperServ::NewsItem *>(obj);
		data.Store("type", TypeToString(ni->type));
		data.Store("text", ni->text);
		data.Store("who", ni->who);
		data.Store("time", ni->time);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		if (!OperServ::news_service)
			return NULL;

		OperServ::NewsItem *ni;
		if (obj)
			ni = anope_dynamic_static_cast<OperServ::NewsItem *>(obj);
		else
			ni = new OperServ::NewsItem();

		Anope::string t;
		data["type"] >> t;
		ni->type = StringToType(t);
		data["text"] >> ni->text;
		data["who"] >> ni->who;
		data["time"] >> ni->time;

		if (!obj)
			OperServ::news_service->AddNewsItem(ni);
		return ni;
	}
};

class MyNewsService final
	: public OperServ::NewsService
{
	std::vector<OperServ::NewsItem *> newsItems[3];
public:
	MyNewsService(Module *m) : NewsService(m) { }

	~MyNewsService() override
	{
		for (const auto &newstype : newsItems)
		{
			for (const auto *newsitem : newstype)
				delete newsitem;
		}
	}

	OperServ::NewsItem *CreateNewsItem() override
	{
		return new OperServ::NewsItem();
	}

	void AddNewsItem(OperServ::NewsItem *n) override
	{
		this->newsItems[n->type].push_back(n);
	}

	void DelNewsItem(OperServ::NewsItem *n) override
	{
		auto &list = this->GetNewsList(n->type);
		auto it = std::find(list.begin(), list.end(), n);
		if (it != list.end())
			list.erase(it);
		delete n;
	}

	std::vector<OperServ::NewsItem *> &GetNewsList(OperServ::NewsType t) override
	{
		return this->newsItems[t];
	}
};

#define lenof(a)        (sizeof(a) / sizeof(*(a)))
static const char **findmsgs(OperServ::NewsType type)
{
	for (auto &msg : msgarray)
		if (msg.type == type)
			return msg.msgs;
	return NULL;
}

class NewsBase
	: public Command
{
protected:
	void DoList(CommandSource &source, OperServ::NewsType ntype, const char **msgs)
	{
		auto &list = OperServ::news_service->GetNewsList(ntype);
		if (list.empty())
			source.Reply(msgs[MSG_LIST_NONE]);
		else
		{
			ListFormatter lflist(source.GetAccount());
			lflist.AddColumn(_("Number")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Text"));
			lflist.SetFlexible(_("{number}: {text} -- created by {creator} on {created}"));

			for (unsigned i = 0, end = list.size(); i < end; ++i)
			{
				ListFormatter::ListEntry entry;
				entry["Number"] = Anope::ToString(i + 1);
				entry["Creator"] = list[i]->who;
				entry["Created"] = Anope::strftime(list[i]->time, NULL, true);
				entry["Text"] = list[i]->text;
				lflist.AddEntry(entry);
			}

			source.Reply(msgs[MSG_LIST_HEADER]);
			lflist.SendTo(source);
			source.Reply(_("End of news list."));
		}
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params, OperServ::NewsType ntype, const char **msgs)
	{
		const Anope::string text = params.size() > 1 ? params[1] : "";

		if (text.empty())
			this->OnSyntaxError(source, "ADD");
		else
		{
			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			auto *news = new OperServ::NewsItem();
			news->type = ntype;
			news->text = text;
			news->time = Anope::CurTime;
			news->who = source.GetNick();

			OperServ::news_service->AddNewsItem(news);

			source.Reply(msgs[MSG_ADDED]);
			Log(LOG_ADMIN, source, this) << "to add a news item";
		}
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params, OperServ::NewsType ntype, const char **msgs)
	{
		const Anope::string &text = params.size() > 1 ? params[1] : "";

		if (text.empty())
			this->OnSyntaxError(source, "DEL");
		else
		{
			auto list = OperServ::news_service->GetNewsList(ntype);
			if (list.empty())
				source.Reply(msgs[MSG_LIST_NONE]);
			else
			{
				if (Anope::ReadOnly)
					source.Reply(READ_ONLY_MODE);
				if (!text.equals_ci("ALL"))
				{
					unsigned num = Anope::Convert<unsigned>(text, 0);
					if (num > 0 && num <= list.size())
					{
						OperServ::news_service->DelNewsItem(list[num - 1]);
						source.Reply(msgs[MSG_DELETED], num);
						Log(LOG_ADMIN, source, this) << "to delete a news item";
						return;
					}

					source.Reply(msgs[MSG_DEL_NOT_FOUND], text.c_str());
				}
				else
				{
					for (unsigned i = list.size(); i > 0; --i)
						OperServ::news_service->DelNewsItem(list[i - 1]);
					source.Reply(msgs[MSG_DELETED_ALL]);
					Log(LOG_ADMIN, source, this) << "to delete all news items";
				}
			}
		}
	}

	void DoNews(CommandSource &source, const std::vector<Anope::string> &params, OperServ::NewsType ntype)
	{
		if (!OperServ::news_service)
			return;

		const Anope::string &cmd = params[0];

		const char **msgs = findmsgs(ntype);
		if (!msgs)
			throw CoreException("news: Invalid type to DoNews()");

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
	NewsBase(Module *creator, const Anope::string &newstype)
		: Command(creator, newstype, 1, 2)
	{
		this->SetSyntax(_("ADD \037text\037"));
		this->SetSyntax(_("DEL {\037num\037 | ALL}"));
		this->SetSyntax("LIST");
	}

	~NewsBase() override
	{
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override = 0;

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override = 0;
};

class CommandOSLogonNews final
	: public NewsBase
{
public:
	CommandOSLogonNews(Module *creator) : NewsBase(creator, "operserv/logonnews")
	{
		this->SetDesc(_("Define messages to be shown to users at logon"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, OperServ::NEWS_LOGON);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Edits or displays the list of logon news messages. When a "
				"user connects to the network, these messages will be sent "
				"to them. However, no more than \002%d\002 messages will be "
				"sent in order to avoid flooding the user. If there are "
				"more news messages, only the most recent will be sent."
			),
			Config->GetModule(this->owner).Get<unsigned>("newscount", "3"));
		return true;
	}
};

class CommandOSOperNews final
	: public NewsBase
{
public:
	CommandOSOperNews(Module *creator) : NewsBase(creator, "operserv/opernews")
	{
		this->SetDesc(_("Define messages to be shown to users who oper"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, OperServ::NEWS_OPER);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Edits or displays the list of oper news messages. When a "
				"user opers up (with the /OPER command), these messages will "
				"be sent to them. However, no more than \002%d\002 messages will "
				"be sent in order to avoid flooding the user. If there are "
				"more news messages, only the most recent will be sent."
			),
			Config->GetModule(this->owner).Get<unsigned>("newscount", "3"));
		return true;
	}
};

class CommandOSRandomNews final
	: public NewsBase
{
public:
	CommandOSRandomNews(Module *creator) : NewsBase(creator, "operserv/randomnews")
	{
		this->SetDesc(_("Define messages to be randomly shown to users at logon"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		return this->DoNews(source, params, OperServ::NEWS_RANDOM);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Edits or displays the list of random news messages. When a "
			"user connects to the network, one (and only one) of the "
			"random news will be randomly chosen and sent to them."
		));
		return true;
	}
};

static unsigned cur_rand_news = 0;

class OSNews final
	: public Module
{
	MyNewsService newsservice;
	NewsItemType newsitem_type;

	CommandOSLogonNews commandoslogonnews;
	CommandOSOperNews commandosopernews;
	CommandOSRandomNews commandosrandomnews;

	Anope::string oper_announcer, announcer;
	unsigned news_count;

	void DisplayNews(User *u, OperServ::NewsType Type)
	{
		auto &newsList = this->newsservice.GetNewsList(Type);
		if (newsList.empty())
			return;

		const auto &modconf = Config->GetModule(this);
		BotInfo *bi = NULL;
		if (Type == OperServ::NEWS_OPER)
			bi = BotInfo::Find(modconf.Get<const Anope::string>("oper_announcer", "OperServ"), true);
		else
			bi = BotInfo::Find(modconf.Get<const Anope::string>("announcer", "Global"), true);
		if (bi == NULL)
			return;

		const auto **msgs = findmsgs(Type);
		if (!msgs)
			return; // BUG

		int start = 0;

		if (Type != OperServ::NEWS_RANDOM)
		{
			start = newsList.size() - news_count;
			if (start < 0)
				start = 0;
		}

		const auto showdate = modconf.Get<bool>("showdate", "yes");
		for (unsigned i = start, end = newsList.size(); i < end; ++i)
		{
			if (Type == OperServ::NEWS_RANDOM && i != cur_rand_news)
				continue;

			const auto *news = newsList[i];
			if (showdate)
				u->SendMessage(bi, msgs[MSG_NEWS_LONG], Anope::strftime(news->time, u->Account(), true).c_str(), news->text.c_str());
			else
				u->SendMessage(bi, msgs[MSG_NEWS_SHORT], news->text.c_str());

			if (Type == OperServ::NEWS_RANDOM)
			{
				++cur_rand_news;
				break;
			}
		}

		/* Reset to head of list to get first random news value */
		if (Type == OperServ::NEWS_RANDOM && cur_rand_news >= newsList.size())
			cur_rand_news = 0;
	}

public:
	OSNews(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, newsservice(this)
		, commandoslogonnews(this)
		, commandosopernews(this)
		, commandosrandomnews(this)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		oper_announcer = conf.GetModule(this).Get<const Anope::string>("oper_announcer", "OperServ");
		announcer = conf.GetModule(this).Get<const Anope::string>("announcer", "Global");
		news_count = conf.GetModule(this).Get<unsigned>("newscount", "3");
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (mname == "OPER")
			DisplayNews(u, OperServ::NEWS_OPER);
	}

	void OnUserConnect(User *user, bool &) override
	{
		if (user->Quitting() || !user->server->IsSynced())
			return;

		DisplayNews(user, OperServ::NEWS_LOGON);
		DisplayNews(user, OperServ::NEWS_RANDOM);
	}
};

MODULE_INIT(OSNews)
